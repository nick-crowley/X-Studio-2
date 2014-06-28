#include "stdafx.h"
#include "CommandNode.h"
#include "GameObjectLibrary.h"
#include "ScriptObjectLibrary.h"
#include "ScriptFileReader.h"
#include "ExpressionParser.h"
#include "SyntaxLibrary.h"
#include "ScriptFile.h"
#include "PreferencesLibrary.h"
#include "CommandNodeList.h"

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
#ifdef VALIDATION
         /// <summary>An invisible node that functions as a jump target with address 'script_length+1'</summary>
         CommandNode  CommandNode::EndOfScript;
#endif
         // -------------------------------- CONSTRUCTION --------------------------------

         /// <summary>Create root node</summary>
         CommandNode::CommandNode()
            : Syntax(CommandSyntax::Unrecognised), 
              Condition(Conditional::NONE),
              Parent(nullptr), 
              JumpTarget(nullptr), 
              Index(EMPTY_JUMP), 
              LineNumber(0),
              Extent({0,0}),
              State(InputState::Raw),
              CmdComment(false)
         {}

         /// <summary>Create node for a hidden jump command</summary>
         /// <param name="parent">parent node</param>
         /// <param name="target">target node</param>
         /// <exception cref="Logic::ArgumentNullException">Parent or target is null</exception>
         CommandNode::CommandNode(const CommandNode& parent, const CommandNode* target)
            : Syntax(SyntaxLib.Find(CMD_HIDDEN_JUMP, GameVersion::Threat)),
              Condition(Conditional::NONE),
              JumpTarget(target),
              Parent(const_cast<CommandNode*>(&parent)),
              LineNumber(parent.LineNumber),
              Extent({0,0}),
              Index(EMPTY_JUMP),
              State(InputState::Raw),
              CmdComment(false)
         {
            REQUIRED(target);

            // Add empty jump parameter
            Parameters += ScriptParameter(ParameterSyntax::LabelNumberParameter, DataType::INTEGER, EMPTY_JUMP);
         }
         
         /// <summary>Create a replacement for a macro command. The line number/text from the macro are preserved</summary>
         /// <param name="macro">macro command - line number, parent, line text preserved</param>
         /// <param name="expanded">expanded command - syntax, parameters, condition preserved</param>
         /// <exception cref="Logic::AlgorithmException">macro command not a macro</exception>
         CommandNode::CommandNode(const CommandNode& macro, const CommandNode& expanded)
            : Syntax(expanded.Syntax),
              Condition(expanded.Condition),
              Parameters(expanded.Parameters),
              Postfix(expanded.Postfix),
              LineNumber(macro.LineNumber), 
              Extent(expanded.Extent), 
              LineText(expanded.LineText),
              Parent(macro.Parent), 
              JumpTarget(nullptr), 
              Index(EMPTY_JUMP),
              State(macro.State),
              CmdComment(false)
         {
            if (!macro.Is(CommandType::Macro))
               throw AlgorithmException(HERE, L"Cannot expand command from a non-macro");
         }
         
         /// <summary>Create node for a script command</summary>
         /// <param name="cnd">conditional.</param>
         /// <param name="syntax">command syntax.</param>
         /// <param name="params">parameters.</param>
         /// <param name="lex">lexer.</param>
         /// <param name="line">1-based line number</param>
         /// <param name="commented">Whether command comment</param>
         CommandNode::CommandNode(Conditional cnd, CommandSyntaxRef syntax, ParameterArray& params, 
                                  const CommandLexer& lex, UINT line, bool commented)
            : Syntax(syntax),
              Condition(cnd),
              Parameters(move(params)),
              LineNumber(line), 
              Extent(lex.Extent), 
              LineText(lex.Input),
              Parent(nullptr), 
              JumpTarget(nullptr), 
              Index(EMPTY_JUMP),
              State(InputState::Raw),
              CmdComment(commented)
         {}
         
         /// <summary>Create node for an expression</summary>
         /// <param name="cnd">conditional.</param>
         /// <param name="syntax">command syntax.</param>
         /// <param name="params">infix parameters and retVar</param>
         /// <param name="params">postfix parameters.</param>
         /// <param name="lex">lexer.</param>
         /// <param name="line">1-based line number</param>
         /// <param name="commented">Whether command comment</param>
         CommandNode::CommandNode(Conditional cnd, CommandSyntaxRef syntax, ParameterArray& infix, ParameterArray& postfix, 
                                  const CommandLexer& lex, UINT line, bool commented)
            : Syntax(syntax),
              Condition(cnd),
              Parameters(move(infix)),
              Postfix(move(postfix)),
              LineNumber(line), 
              Extent(lex.Extent), 
              LineText(lex.Input),
              Parent(nullptr), 
              JumpTarget(nullptr), 
              Index(EMPTY_JUMP),
              State(InputState::Raw),
              CmdComment(commented)
         {}

         CommandNode::~CommandNode()
         {}

         // ------------------------------- STATIC METHODS -------------------------------
         
         /// <summary>Get input state string</summary>
         const wchar*  CommandNode::GetString(InputState s)
         {
            static const wchar* str[] = {L"UNVERIFIED", L"VERIFIED", L"COMPILED"};
            return str[(UINT)s];
         }
         
         /// <summary>Checks whether a command is executable from a logic perspective</summary>
         CommandNode::NodeDelegate  CommandNode::isExecutableCommand = [](const CommandNodePtr& n) 
         { 
            return !n->CmdComment && (n->Is(CommandType::Standard) || n->Is(CMD_BREAK) || n->Is(CMD_CONTINUE)); 
         };

         /// <summary>Checks whether commands are standard</summary>
         CommandNode::NodeDelegate  CommandNode::isStandardCommand = [](const CommandNodePtr& n) 
         { 
            return n->Is(CommandType::Standard) && !n->CmdComment; 
         };
         
         /// <summary>Checks whether commands are compatible with 'skip-if' conditional</summary>
         CommandNode::NodeDelegate  CommandNode::isSkipIfCompatible = [](const CommandNodePtr& n) 
         { 
            switch (n->Logic)
            {
            case BranchLogic::None:    // Standard cmd
            case BranchLogic::Break:
            case BranchLogic::Continue:
               return true;
            }
            return false;
         };
         
         /// <summary>Finds any command or 'starting' conditional, but rejects 'middle' conditionals like else,else-if,end</summary>
         CommandNode::NodeDelegate  CommandNode::isConditionalEnd = [](const CommandNodePtr& n) 
         { 
            switch (n->Logic)
            {
            case BranchLogic::NOP:
            case BranchLogic::End:
            case BranchLogic::Else:
            case BranchLogic::ElseIf:
               return false;
            }
            return true;
         };

         /// <summary>Finds any executable command, ie. anything except END or NOP</summary>
         CommandNode::NodeDelegate  CommandNode::isConditionalAlternate = [](const CommandNodePtr& n) 
         { 
            switch (n->Logic)
            {
            case BranchLogic::NOP:
            case BranchLogic::End:
               return false;
            }
            return true;
         };
         
         // ------------------------------- PUBLIC METHODS -------------------------------
         
         /// <summary>Execute a visitor upon this node</summary>
         /// <param name="v">The visitor</param>
         void  CommandNode::Accept(Visitor& v)
         {
            if (!IsRoot())
               v.VisitNode(this);
            else
               v.VisitRoot(this);
         }

         /// <summary>Add child node</summary>
         /// <param name="cmd">The command node</param>
         /// <returns>Command node</returns>
         CommandNodePtr  CommandNode::Add(CommandNodePtr node)
         {
            // Set parent and append
            node->Parent = this;
            Children.push_back(node);
            return node;
         }

         /// <summary>Query command syntax ID</summary>
         /// <param name="id">Command ID</param>
         /// <returns>True if command is uncommented and has a matching ID, otherwise false</returns>
         bool  CommandNode::Is(UINT ID) const
         {
            return !CmdComment && Syntax.Is(ID);
         }

         /// <summary>Query command syntax type</summary>
         bool  CommandNode::Is(CommandType t) const
         {
            return Syntax.Is(t);
         }

         /// <summary>Query whether node has no children.</summary>
         /// <returns></returns>
         bool CommandNode::IsEmpty() const
         {
            return Children.empty();
         }

         /// <summary>Identifies branch logic</summary>
         BranchLogic  CommandNode::GetBranchLogic() const
         {
            // CmdComment
            if (CmdComment)
               return BranchLogic::NOP;

            // Command
            switch (Syntax.ID)
            {
            case CMD_END:      return BranchLogic::End;
            case CMD_ELSE:     return BranchLogic::Else;
            case CMD_BREAK:    return BranchLogic::Break;
            case CMD_CONTINUE: return BranchLogic::Continue;

            case CMD_COMMENT: 
            case CMD_NOP:     
               return BranchLogic::NOP;
            
            case MACRO_FOR_EACH:
            case MACRO_FOR_EACH_COUNTER:
            case MACRO_FOR_LOOP:
               return BranchLogic::While;
         
            // Conditional
            default:
               switch (Condition)
               {
               case Conditional::IF:      
               case Conditional::IF_NOT:        
                  return BranchLogic::If;

               case Conditional::WHILE:         
               case Conditional::WHILE_NOT:     
                  return BranchLogic::While;

               case Conditional::ELSE_IF:  
               case Conditional::ELSE_IF_NOT:   
                  return BranchLogic::ElseIf;

               case Conditional::SKIP_IF:  
               case Conditional::SKIP_IF_NOT:   
                  return BranchLogic::SkipIf;

               default:
                  return BranchLogic::None;
               }
            }
         }
         
         /// <summary>Get line text without indentation</summary>
         GuiString   CommandNode::GetLineCode() const
         {
            return LineText.TrimLeft(L" ");
         }

         // ------------------------------ PROTECTED METHODS -----------------------------
         
         // ------------------------------- PRIVATE METHODS ------------------------------

         /// <summary>Finds an ancestor with a given branch logic</summary>
         /// <returns>Parent if found, otherwise nullptr</returns>
         CommandNode*  CommandNode::FindAncestor(BranchLogic l) const
         {
            // Check for a parent 'while' command
            for (CommandNode* n = Parent; n != nullptr; n = n->Parent)
               if (n->Logic == l)
                  return n;

            // Not found
            return nullptr;
         }

         /// <summary>Find a child node by value</summary>
         /// <param name="child">desired child</param>
         /// <returns></returns>
         CommandNode::NodeIterator CommandNode::FindChild(const CommandNode* child) const
         {
            return find_if(Children.begin(), Children.end(), [child](const CommandNodePtr& n) {return child == n.get();} );
         }
         
         /// <summary>Finds the conditional or standard command following an if/else-if statement</summary>
         /// <returns></returns>
         CommandNode* CommandNode::FindConditionalAlternate() const
         {
            // Find next ELSE-IF,ELSE or Std Cmd
            auto node = FindSibling(isConditionalAlternate, L"alternate conditional");

            // ELSE: Since else is auxiliary, use first child.  NB: May have no children, or only auxiliary children
            if (node->Is(CMD_ELSE))
            {
               // No children: Use next std sibling of ELSE
               if (node->Children.empty())
                  node = node->FindNextCommand();
               
               // 1st child is not executable (Auxiliary): Use it's next std sibling
               else if (!isExecutableCommand(*node->Children.begin())) 
                  node = (*node->Children.begin())->FindNextCommand();
               else
                  // Default: Use first child
                  node = node->Children.begin()->get();
            }

            return node;
         }
         
         /// <summary>Finds the command to execute following a failed if/else-if statement</summary>
         /// <returns></returns>
         CommandNode* CommandNode::FindConditionalEnd() const
         {
#ifndef VALIDATION
            // Find next Std command that isn't ELSE-IF
            return FindSibling(isConditionalEnd, L"conditional end-point");
#else
            // EOF: Return sentinel
            if (IsRoot())
               return &EndOfScript;

            // Find next sibling node containing a standard command
            auto node = find_if(++Parent->FindChild(this), Parent->Children.cend(), isConditionalEnd);

            // Return if found, else recurse into parent
            return node != Parent->Children.cend() ? node->get() : Parent->FindConditionalEnd();
#endif
         }

         /// <summary>Find label definition</summary>
         /// <param name="name">Label name</param>
         /// <returns>Label definition command if found, otherwise nullptr</returns>
         CommandNode*  CommandNode::FindLabel(const wstring& name) const
         {
            // Check node
            if (Is(CMD_DEFINE_LABEL) && Parameters[0].Value.String == name)
               return const_cast<CommandNode*>(this);
            
            // Check children
            for (const auto& c : Children)
               if (CommandNode* label = c->FindLabel(name))
                  return label;

            // Not in current branch
            return nullptr;
         }
         
         /// <summary>Finds the next executable sibling</summary>
         /// <returns></returns>
         CommandNode* CommandNode::FindNextCommand() const
         {
            return FindSibling(isExecutableCommand, L"next executable command");
         }

         /// <summary>Finds the next sibling of this node</summary>
         /// <returns></returns>
         /// <exception cref="Logic::AlgorithmException">Executed on root node</exception>
         CommandNode* CommandNode::FindNextSibling() const
         {
            // Ensure not root
            if (IsRoot())
               throw AlgorithmException(HERE, L"Cannot retrieve next sibling of the root");

            // Find 'this' and return next (if any)
            auto node = Parent->FindChild(this);
            return ++node != Parent->Children.end() ? node->get() : nullptr;
         }

         /// <summary>Finds the prev sibling of this node</summary>
         /// <returns></returns>
         /// <exception cref="Logic::AlgorithmException">Executed on root node</exception>
         CommandNode* CommandNode::FindPrevSibling() const
         {
            // Ensure not root
            if (IsRoot())
               throw AlgorithmException(HERE, L"Cannot retrieve previous sibling of the root");

            // Find 'this' and return prev (if any)
            auto node = Parent->FindChild(this);
            return node != Parent->Children.begin() ? (--node)->get() : nullptr;
         }

         /// <summary>Finds the root node</summary>
         /// <returns>Root</returns>
         CommandNode*  CommandNode::FindRoot() const
         {
            CommandNode* n;
            for (n = const_cast<CommandNode*>(this); n->Parent != nullptr; n = n->Parent)
            {}
            return n;
         }
         
         /// <summary>Searches for a sibling or ancestral sibling with matching properties</summary>
         /// <param name="d">delegate for matching properties</param>
         /// <param name="help">help string on failure</param>
         /// <returns>First matching node</returns>
         /// <remarks>Does not examine the children of any nodes, searches 'up' and 'right' along the tree</remarks>
         /// <exception cref="Logic::AlgorithmException">Unable to find node</exception>
         CommandNode*  CommandNode::FindSibling(NodeDelegate d, const wchar* help) const
         {
            // Not found: Error
            if (IsRoot())
               throw AlgorithmException(HERE, VString(L"Cannot find %s", help));

            // Find next sibling node containing a standard command
            auto node = find_if(++Parent->FindChild(this), Parent->Children.cend(), d);

            // Return if found, else recurse into parent
            return node != Parent->Children.cend() ? node->get() : Parent->FindSibling(d, help);
         }
         
         /// <summary>Generates an expanded node: new syntax/condition.parameters, with parent/line-num/line-text of current node</summary>
         /// <param name="txt">Command text.</param>
         /// <param name="v">game version</param>
         /// <returns></returns>
         CommandNodePtr  CommandNode::ExpandCommand(const wstring& txt, GameVersion v)
         {
            // Generate new node
            return new CommandNode(*this, *ScriptParser::Generate(txt, v));
         }
         
         /// <summary>Generates the actual commands necessary to form the 'dim' macro</summary>
         /// <param name="script">script file.</param>
         /// <returns>List of expanded replacement commands</returns>
         /// <exception cref="Logic::AlgorithmException">macro parameters improperly verified</exception>
         /// <exception cref="Logic::InvalidOperationException">Not a 'dim' macro</exception>
         CommandNodeList  CommandNode::ExpandDimArray(ScriptFile& script)
         {
            CommandNodeList nodes;

            // Require 'DIM' command
            if (!Is(MACRO_DIM_ARRAY))
               throw InvalidOperationException(HERE, L"Command must be 'dim' macro");
            
            // Generate components
            auto size   = Parameters.size()-1;
            auto retVar = Parameters[0].Text.c_str();

            // Validate
            if (Parameters[0].Type != DataType::VARIABLE)
               throw AlgorithmException(HERE, L"'Dim' macro must be assigned to a variable");
            if (size < 1)
               throw AlgorithmException(HERE, L"'Dim' macro requires at least one element");

            // Generate '<retVar> = array alloc: size=<size>'
            VString cmd(L"%s = array alloc: size=%d", retVar, size);
            nodes += ExpandCommand(cmd, script.Game);

            // Element assignments
            for (UINT i = 0; i < size; ++i)
            {
               auto value = Parameters[i+1].Text.c_str();
               
               // Generate '<array>[i] = <val>' 
               cmd = VString(L"%s[%d] = %s", retVar, i, value);
               nodes += ExpandCommand(cmd, script.Game);
            }

            return nodes;
         }

         /// <summary>Generates the actual commands necessary to form the 'for loop' macro</summary>
         /// <param name="script">script file.</param>
         /// <returns>List of expanded replacement commands</returns>
         /// <exception cref="Logic::AlgorithmException">macro parameters improperly verified</exception>
         /// <exception cref="Logic::InvalidOperationException">Not a 'for loop' macro</exception>
         /// <remarks>Expands 'for $0 = $1 to $2 step $3' to
         ///
         /// (iterator) = (inital_value) ± (step_value)
         /// while (iterator) greater/less (final_value)
         /// (iterator) = (iterator) ± (step_value)</remarks>
         CommandNodeList  CommandNode::ExpandForLoop(ScriptFile& script)
         {
            CommandNodeList  nodes;
            ParameterArray   params,
                             postfix;

            // Require 'for $0 = $1 to $2 step $3'
            if (!Is(MACRO_FOR_LOOP))
               throw InvalidOperationException(HERE, L"Command must be 'for loop' macro");
            
            // Lookup components
            const wchar *iterator = Parameters[0].Text.c_str(),
                        *init_val = Parameters[1].Text.c_str(),
                        *last_val = Parameters[2].Text.c_str(),
                        *step_val = Parameters[3].Text.c_str();
            
            // Validate parameters
            if (Parameters[3].Type != DataType::INTEGER)
               throw AlgorithmException(HERE, L"Loop step must be an integer");

            // Determine direction
            int step = GuiString(step_val).ToInt();
            bool ascending = step > 0;

            // Init: (iterator) = (inital_value) ± (step_value)
            auto cmd = VString(L"%s = %s %s %d", iterator, init_val, (ascending ? L"-" : L"+"), (ascending ? step : -step));
            nodes += ExpandCommand(cmd, script.Game);

            // Guard: while (iterator) less/greater (final_value)
            cmd = VString(L"while %s %s %s", iterator, (ascending ? L"<" : L">"), last_val);
            nodes += ExpandCommand(cmd, script.Game);

            // Optimize using inc/dec if possible
            if (step == 1 || step == -1)
               // Advance: inc/dec (iterator)
               cmd = VString(L"%s %s", (ascending ? L"inc" : L"dec"), iterator);
            else 
               // Advance: (iterator) = (iterator) ± (step_value)
               cmd = VString(L"%s = %s %s %d", iterator, iterator, (ascending ? L"+" : L"-"), (ascending ? step : -step));
            
            // Add as child of 'while'
            nodes.back()->Add(ExpandCommand(cmd, script.Game));     

            // Move all children of 'for loop' to 'while' expression
            MoveChildren(*this, *nodes.back());

            return nodes;
         }
         
         /// <summary>Generates the actual commands necessary to form the 'foreach' macro</summary>
         /// <param name="script">script file.</param>
         /// <returns>List of expanded replacement commands</returns>
         /// <exception cref="Logic::AlgorithmException">macro parameters improperly verified</exception>
         /// <exception cref="Logic::InvalidOperationException">Not a 'foreach' macro</exception>
         /// <remarks>Expands 'for each $0 in array $1 [using counter $2]' to
         ///
         /// (iterator) = size of array (array)
         /// while (iterator)
         /// dec (iterator)
         /// (item_iterator) = (array)[(iterator)]</remarks>
         CommandNodeList  CommandNode::ExpandForEach(ScriptFile& script)
         {
            CommandNodeList  nodes;
            ParameterArray   params,
                             postfix;

            // Require 'for each $0 in array $1' or 'for each $0 in array $1 using counter $2'
            if (!Is(MACRO_FOR_EACH) && !Is(MACRO_FOR_EACH_COUNTER))
               throw InvalidOperationException(HERE, L"Command must be a 'foreach' macro");
            
            // Lookup components
            const wchar *item  = Parameters[0].Text.c_str(),
                        *array = Parameters[1].Text.c_str();

            // Iterator: Generate unique name if not specified by user
            GuiString iterator = Is(MACRO_FOR_EACH_COUNTER) ? Parameters[2].Text : IteratorNames.GetNext();
            if (!Is(MACRO_FOR_EACH_COUNTER))
               script.Variables.Add(iterator.TrimLeft(L"$"));

            // (iterator) = size of array (array)
            VString init(L"%s = size of array %s", iterator.c_str(), array);
            nodes += ExpandCommand(init, script.Game);

            // while (iterator)
            VString guard(L"while %s", iterator.c_str());
            nodes += ExpandCommand(guard, script.Game);     

            // dec (iterator)
            VString advance(L"dec %s", iterator.c_str());
            nodes.back()->Add( ExpandCommand(advance, script.Game) );    // Add as child of 'while'

            // (item_iterator) = (array)[(iterator)]
            VString access(L"%s = %s[%s]", item, array, iterator.c_str());
            nodes.back()->Add( ExpandCommand(access, script.Game) );     // Add as child of 'while'

            // Move all children of 'foreach' to 'while' expression
            MoveChildren(*this, *nodes.back());

            return nodes;
         }

         /// <summary>Expands macro commands</summary>
         /// <param name="script">script file.</param>
         /// <param name="errors">Errors collection.</param>
         void  CommandNode::ExpandMacros(ScriptFile& script, ErrorArray& errors)
         {
            try
            {
               // DEBUG:
               //Console << "ExpandMacros: " << LineNumber << ": " << LineText << ENDL;

               // Don't expand commented macros
               if (!IsRoot() && !CmdComment && Is(CommandType::Macro))
               {
                  CommandNodeList nodes;

                  // Generate replacement nodes
                  switch (Syntax.ID)
                  {
                  case MACRO_DIM_ARRAY:  nodes = ExpandDimArray(script);   break;
                  case MACRO_FOR_LOOP:   nodes = ExpandForLoop(script);    break;
                  
                  case MACRO_FOR_EACH_COUNTER:  
                  case MACRO_FOR_EACH:   nodes = ExpandForEach(script);    break;
                  }

                  // Insert following self. All children have been moved to expanded commands.
                  Parent->Children.insert(++Parent->FindChild(this), nodes.begin(), nodes.end());
                  return;
               }
            }
            catch (ExceptionBase& e) {
               errors += MakeError(GuiString(L"Macro expansion failed: ") + e.Message); 
            }
            
            // Recurse into children  [Allowing for in-place modification of child list]
            for (auto c = Children.begin(); c != Children.end(); )
            {
               // Expand macros
               (*c)->ExpandMacros(script, errors);

               // Macro: Delete 
               auto n = c++;
               if ((*n)->Is(CommandType::Macro))
                  Children.erase(n);
            }
         }

         
         /// <summary>Gets the last executable child.</summary>
         /// <returns></returns>
         /// <exception cref="Logic::AlgorithmException">No executable children</exception>
         CommandNode* CommandNode::GetLastExecutableChild() const
         {
            // Reverse linear search
            auto cmd = find_if(Children.rbegin(), Children.rend(), isExecutableCommand);

            // Failed: error
            if (cmd == Children.rend())
               throw AlgorithmException(HERE, L"Command doesn't have any executable children");

            return cmd->get();
         }
         
         /// <summary>Gets the name of the script (if any) called by this command</summary>
         /// <returns>Script name, or empty string if defined by variable</returns>
         wstring CommandNode::GetScriptCallName() const
         {
            // Find scriptName parameter
            auto callName = find_if(Parameters.begin(), Parameters.end(), [](const ScriptParameter& s) {return s.Syntax.Usage == ParameterUsage::ScriptName;} );
                     
            // Return name is present, empty string if missing (or a variable)
            return callName != Parameters.end() && callName->Type == DataType::STRING ? callName->Value.String : L"";
         }

         /// <summary>Determines whether has executable child</summary>
         /// <returns></returns>
         bool  CommandNode::HasExecutableChild() const
         {
            return any_of(Children.begin(), Children.end(), isExecutableCommand);
         }

         /// <summary>Inserts an unconditional jump command as the last child</summary>
         /// <param name="target">Command to target</param>
         /// <returns></returns>
         void  CommandNode::InsertJump(NodeIterator pos, const CommandNode* target)
         {
            Children.insert(pos, new CommandNode(*this, target));
         }
         
         /// <summary>Query whether node is root</summary>
         bool  CommandNode::IsRoot() const
         {
            return Parent == nullptr;
         }
         
         /// <summary>Create error for this entire line</summary>
         /// <param name="msg">error message</param>
         /// <returns></returns>
         ErrorToken  CommandNode::MakeError(const GuiString& msg) const
         {
            return ErrorToken(msg, LineNumber, LineText.substr(Extent.cpMin, Extent.cpMax-Extent.cpMin), Extent);
         }

         /// <summary>Create error for a token on this line</summary>
         /// <param name="msg">error message</param>
         /// <param name="tok">token</param>
         /// <returns></returns>
         ErrorToken  CommandNode::MakeError(const GuiString& msg, const ScriptToken& tok) const
         {
            return ErrorToken(msg, LineNumber, tok);
         }
         
         /// <summary>Transfer the children of one node to another.</summary>
         /// <param name="from">From.</param>
         /// <param name="to">To.</param>
         /// <exception cref="Logic::AlgorithmException">From and To specify the same node</exception>
         void CommandNode::MoveChildren(CommandNode& from, CommandNode& to)
         {
            if (&from == &to)
               throw AlgorithmException(HERE, L"Cannot transfer children to itself");

            // Move children 
            for (auto& c = from.Children.begin(); c != from.Children.end(); )
            {
               to.Add(*c);
               from.Children.erase(c++);
            }
         }

         /// <summary>Reverts a command comment with verification errors into an ordinary comment</summary>
         /// <param name="child">Child command to revert</param>
         /// <exception cref="Logic::ArgumentNullException">child is null</exception>
         /// <exception cref="Logic::InvalidOperationException">Child node not found</exception>
         void CommandNode::RevertCommandComment(CommandNode* child)
         {
            REQUIRED(child);

            // Ensure child exists
            if (FindChild(child) == Children.end())
               throw InvalidOperationException(HERE, L"Cannot find child");

            // Re-lex line text as a comment
            CommandLexer lex(L"*" + child->LineText);

            // Extract as comment parameter
            CommandSyntaxRef newSyntax = SyntaxLib.Find(CMD_COMMENT, GameVersion::Threat);
            ParameterArray params(ScriptParameter(newSyntax.Parameters[0], lex.Tokens[1]));

            // Generate new command + perform in-place replacement
            ReplaceChild(child, new CommandNode(Conditional::NONE, newSyntax, params, lex, child->LineNumber, false));
         }

         /// <summary>Replaces one child node with another.</summary>
         /// <param name="oldChild">existing child.</param>
         /// <param name="newChild">new replacement child.</param>
         /// <exception cref="Logic::ArgumentNullException">child is null</exception>
         /// <exception cref="Logic::InvalidOperationException">Child node not found</exception>
         void CommandNode::ReplaceChild(CommandNode* oldChild, CommandNode* newChild)
         {
            REQUIRED(oldChild);
            REQUIRED(newChild);

            // Linear find/replace existing child
            for (auto& c : Children)
               if (c.get() == oldChild)
               {
                  newChild->Parent = this;
                  return c.reset(newChild);
               }

            // Error: Not found
            throw InvalidOperationException(HERE, L"Cannot find existing child");
         }


      }
   }
}

