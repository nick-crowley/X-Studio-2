#include "stdafx.h"
#include "CommandTree.h"
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
         CommandTree  CommandTree::EndOfScript;
#endif
         // -------------------------------- CONSTRUCTION --------------------------------

         /// <summary>Create root node</summary>
         CommandTree::CommandTree()
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
         CommandTree::CommandTree(const CommandTree& parent, const CommandTree* target)
            : Syntax(SyntaxLib.Find(CMD_HIDDEN_JUMP, GameVersion::Threat)),
              Condition(Conditional::NONE),
              JumpTarget(target),
              Parent(const_cast<CommandTree*>(&parent)),
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
         CommandTree::CommandTree(const CommandTree& macro, const CommandTree& expanded)
            : Syntax(expanded.Syntax),
              Condition(expanded.Condition),
              Parameters(expanded.Parameters),
              Postfix(expanded.Postfix),
              LineNumber(macro.LineNumber), 
              Extent(macro.Extent), 
              LineText(macro.LineText),
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
         CommandTree::CommandTree(Conditional cnd, CommandSyntaxRef syntax, ParameterArray& params, 
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
         CommandTree::CommandTree(Conditional cnd, CommandSyntaxRef syntax, ParameterArray& infix, ParameterArray& postfix, 
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

         CommandTree::~CommandTree()
         {}

         // ------------------------------- STATIC METHODS -------------------------------
         
         /// <summary>Get input state string</summary>
         const wchar*  CommandTree::GetString(InputState s)
         {
            static const wchar* str[] = {L"UNVERIFIED", L"VERIFIED", L"COMPILED"};
            return str[(UINT)s];
         }
         
         /// <summary>Checks whether a command is executable from a logic perspective</summary>
         CommandTree::NodeDelegate  CommandTree::isExecutableCommand = [](const CommandNodePtr& n) 
         { 
            return !n->CmdComment && (n->Is(CommandType::Standard) || n->Is(CMD_BREAK) || n->Is(CMD_CONTINUE)); 
         };

         /// <summary>Checks whether commands are standard</summary>
         CommandTree::NodeDelegate  CommandTree::isStandardCommand = [](const CommandNodePtr& n) 
         { 
            return n->Is(CommandType::Standard) && !n->CmdComment; 
         };
         
         /// <summary>Checks whether commands are compatible with 'skip-if' conditional</summary>
         CommandTree::NodeDelegate  CommandTree::isSkipIfCompatible = [](const CommandNodePtr& n) 
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
         CommandTree::NodeDelegate  CommandTree::isConditionalEnd = [](const CommandNodePtr& n) 
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
         CommandTree::NodeDelegate  CommandTree::isConditionalAlternate = [](const CommandNodePtr& n) 
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
         
         /// <summary>Add child node</summary>
         /// <param name="cmd">The command node</param>
         /// <returns>Command node</returns>
         CommandNodePtr  CommandTree::Add(CommandNodePtr node)
         {
            // Set parent and append
            node->Parent = this;
            Children.push_back(node);
            return node;
         }

         /// <summary>Compiles the script.</summary>
         /// <param name="script">The script.</param>
         /// <exception cref="Logic::AlgorithmException">Error in linking algorithm</exception>
         void  CommandTree::Compile(ScriptFile& script, ErrorArray& errors)
         {
            // Expand macros
            ExpandMacros(script, errors);

            // Perform linking
            LinkCommands(errors);

            // Index commands
            UINT index = 0;
            IndexCommands(index);

#ifdef VALIDATION
            // Set address of EOF
            EndOfScript.Index = index;
#endif
            // Finalise linkage
            FinalizeLinkage(errors);
            
            // Compile commands
            GenerateCommands(script, errors);

            // Update state
            State = InputState::Compiled;
         }
         
         /// <summary>Finds all instances of a symbol.</summary>
         /// <param name="name">symbol.</param>
         /// <param name="type">type.</param>
         /// <param name="results">The results.</param>
         void  CommandTree::FindAll(const wstring& name, SymbolType type, SymbolList& results) const
         {
            bool comment = CmdComment || Is(CMD_COMMENT);

            // Perform Search
            switch (type)
            {
            // Label: Search for 'define label', 'goto label', 'gosub label'
            case SymbolType::Label:
               if ((Is(CMD_DEFINE_LABEL) || Is(CMD_GOTO_LABEL) || Is(CMD_GOTO_SUB)) && !Parameters.empty()) 
                  results.push_back(Symbol(Parameters[0].Token, SymbolType::Label, LineNumber, LineText, comment));
               break;

            // Variable: Search all commands
            case SymbolType::Variable:
               for (const auto& p : Parameters)
                  if (p.Type == DataType::VARIABLE && p.Value.Type == ValueType::String && p.Token.ValueText == name)
                     results.push_back(Symbol(p.Token, SymbolType::Variable, LineNumber, LineText, comment));
               break;
            }

            // Examine children
            for (const auto& cmd : Children)
               cmd->FindAll(name, type, results);
         }

         /// <summary>Query command syntax ID</summary>
         /// <param name="id">Command ID</param>
         /// <returns>True if command is uncommented and has a matching ID, otherwise false</returns>
         bool  CommandTree::Is(UINT ID) const
         {
            return !CmdComment && Syntax.Is(ID);
         }

         /// <summary>Query command syntax type</summary>
         bool  CommandTree::Is(CommandType t) const
         {
            return Syntax.Is(t);
         }

         /// <summary>Identifies branch logic</summary>
         BranchLogic  CommandTree::GetBranchLogic() const
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
         GuiString   CommandTree::GetLineCode() const
         {
            return LineText.TrimLeft(L" ");
         }

         /// <summary>Debug print</summary>
         /// <param name="depth">The depth.</param>
         void  CommandTree::Print(int depth) const
         {
            if (IsRoot())
            {
               Console << ENDL << "Ln  Index  Logic            Text        " << Cons::Purple << Cons::Bold << GetString(State);
               Console << ENDL << "-------------------------------------------------------" << ENDL; 
            }
            else
            {
               // Line#/Logic/Text
               VString   line(!Is(CMD_HIDDEN_JUMP) ? L"%03d: " : L"---: ", LineNumber), 
                         logic(::GetString(Logic));
               GuiString txt(LineText);
               Cons      colour(Cons::White);
            
               // Index
               line += VString(Is(CommandType::Standard) && Index != EMPTY_JUMP ? L"%03d: " : L"---: ", Index);
            
               // Logic
               switch (Logic)
               {
               // Conditional:
               default: 
                  colour = Cons::Cyan;
                  if (JumpTarget)
                     txt = (JumpTarget->Index ? VString(L"Jump-if-false: %d", JumpTarget->Index) 
                                              : VString(L"<Invalid JumpTarget> : ") + JumpTarget->LineCode);
                  break;

               // NOP:
               case BranchLogic::NOP:
                  logic = (!CmdComment ? L"NOP" : L"Cmd");
                  colour = Cons::Yellow;
                  break;

               // Command:
               case BranchLogic::None:
                  if (Is(CMD_HIDDEN_JUMP) || Is(CMD_GOTO_LABEL) || Is(CMD_GOTO_SUB))
                  {
                     colour = Cons::Green; 
                     logic = Is(CMD_HIDDEN_JUMP) ? L"Jmp" : L"Goto";
                     txt = L"Unconditional: ";

                     // Post-Compile: Display label number 
                     if (JumpTarget)
                        txt += VString(L"%d", JumpTarget->Index);
                     // Pre-Compile: Label name
                     else 
                        txt += (!Parameters.empty() ? Parameters[0].Token.Text : L"<missing>");
                  }
                  else if (Is(CMD_DEFINE_LABEL))
                  {
                     colour = Cons::Purple;
                     logic = L"Proc";
                  }
                  else if (Is(CMD_RETURN))
                  {
                     colour = Cons::Cyan;
                     logic = L"Ret";
                  }
                  else if (Syntax == CommandSyntax::Unrecognised)
                  {  // Print entire line in red
                     Console << (colour = Cons::Red);
                     logic = L"???";
                  }
                  else
                     logic = L"Cmd";
                  break;
               }

               // Print
               Console << line+Indent(depth) << colour << logic << Cons::White << L" : " << colour << txt.TrimLeft(L" ") << ENDL;
            }

            // Print Children
            for (auto c : Children)
               c->Print(depth+1);

            // Spacing
            if (IsRoot())
               Console << ENDL;
         }

         /// <summary>Flattens the tree into a list.</summary>
         /// <param name="l">output.</param>
         void  CommandTree::ToList(CommandNodeList& l) const
         {
            // Flatten children (if any) into list
            for (auto& c : Children)
            {
               l.push_back(c);
               c->ToList(l);
            }
         }

         /// <summary>Verifies the entire tree</summary>
         /// <param name="script">script</param>
         /// <param name="errors">errors collection</param>
         void  CommandTree::Verify(ScriptFile& script, ErrorArray& errors) 
         {
            // Identify labels/variables
            IdentifyVariables(script, errors);

            // Identify constants
            IdentifyConstants(script, errors);

            // parameters
            VerifyCommand(script, errors);

            // branching logic
            VerifyLogic(errors);

            // Ensure script has std commands  [don't count break/continue]
            if (!any_of(Children.begin(), Children.end(), isStandardCommand))
               errors += MakeError(L"No executable commands found");

            // [VALID] Verify all control paths lead to RETURN
            else if (errors.empty()) 
               VerifyTermination(errors);
            
            // Update state
            State = InputState::Verified;
         }

         // ------------------------------ PROTECTED METHODS -----------------------------
         
         // ------------------------------- PRIVATE METHODS ------------------------------

         /// <summary>Finds an ancestor with a given branch logic</summary>
         /// <returns>Parent if found, otherwise nullptr</returns>
         CommandTree*  CommandTree::FindAncestor(BranchLogic l) const
         {
            // Check for a parent 'while' command
            for (CommandTree* n = Parent; n != nullptr; n = n->Parent)
               if (n->Logic == l)
                  return n;

            // Not found
            return nullptr;
         }

         /// <summary>Find a child node by value</summary>
         /// <param name="child">desired child</param>
         /// <returns></returns>
         CommandTree::NodeIterator CommandTree::FindChild(const CommandTree* child) const
         {
            return find_if(Children.begin(), Children.end(), [child](const CommandNodePtr& n) {return child == n.get();} );
         }
         
         /// <summary>Finds the conditional or standard command following an if/else-if statement</summary>
         /// <returns></returns>
         CommandTree* CommandTree::FindConditionalAlternate() const
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
         CommandTree* CommandTree::FindConditionalEnd() const
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
         CommandTree*  CommandTree::FindLabel(const wstring& name) const
         {
            // Check node
            if (Is(CMD_DEFINE_LABEL) && Parameters[0].Value.String == name)
               return const_cast<CommandTree*>(this);
            
            // Check children
            for (const auto& c : Children)
               if (CommandTree* label = c->FindLabel(name))
                  return label;

            // Not in current branch
            return nullptr;
         }
         
         /// <summary>Finds the next executable sibling</summary>
         /// <returns></returns>
         CommandTree* CommandTree::FindNextCommand() const
         {
            return FindSibling(isExecutableCommand, L"next executable command");
         }

         /// <summary>Finds the next sibling of this node</summary>
         /// <returns></returns>
         /// <exception cref="Logic::AlgorithmException">Executed on root node</exception>
         CommandTree* CommandTree::FindNextSibling() const
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
         CommandTree* CommandTree::FindPrevSibling() const
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
         CommandTree*  CommandTree::FindRoot() const
         {
            CommandTree* n;
            for (n = const_cast<CommandTree*>(this); n->Parent != nullptr; n = n->Parent)
            {}
            return n;
         }
         
         /// <summary>Searches for a sibling or ancestral sibling with matching properties</summary>
         /// <param name="d">delegate for matching properties</param>
         /// <param name="help">help string on failure</param>
         /// <returns>First matching node</returns>
         /// <remarks>Does not examine the children of any nodes, searches 'up' and 'right' along the tree</remarks>
         /// <exception cref="Logic::AlgorithmException">Unable to find node</exception>
         CommandTree*  CommandTree::FindSibling(NodeDelegate d, const wchar* help) const
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
         CommandNodePtr  CommandTree::ExpandCommand(const wstring& txt, GameVersion v)
         {
            // Generate new node
            return new CommandTree(*this, *ScriptParser::Generate(txt, v));
         }
         
         /// <summary>Generates the actual commands necessary to form the 'dim' macro</summary>
         /// <param name="script">script file.</param>
         /// <returns>List of expanded replacement commands</returns>
         /// <exception cref="Logic::AlgorithmException">macro parameters improperly verified</exception>
         /// <exception cref="Logic::InvalidOperationException">Not a 'dim' macro</exception>
         CommandNodeList  CommandTree::ExpandDimArray(ScriptFile& script)
         {
            CommandNodeList nodes;

            // Require 'DIM' command
            if (!Is(MACRO_DIM_ARRAY))
               throw InvalidOperationException(HERE, L"Command must be 'dim' macro");
            
            // Generate components
            auto size   = Parameters.size()-1;
            auto retVar = Parameters[0].DisplayText.c_str();

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
               auto value = Parameters[i+1].DisplayText.c_str();
               
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
         CommandNodeList  CommandTree::ExpandForLoop(ScriptFile& script)
         {
            CommandNodeList  nodes;
            ParameterArray   params,
                             postfix;
            CommandSyntaxRef syntax = SyntaxLib.Find(CMD_EXPRESSION, GameVersion::Threat);

            // Require 'for $0 = $1 to $2 step $3'
            if (!Is(MACRO_FOR_LOOP))
               throw InvalidOperationException(HERE, L"Command must be 'for loop' macro");
            
            // Lookup components
            const wchar *iterator = Parameters[0].DisplayText.c_str(),
                        *init_val = Parameters[1].DisplayText.c_str(),
                        *last_val = Parameters[2].DisplayText.c_str(),
                        *step_val = Parameters[3].DisplayText.c_str();

            // Validate parameters
            if (Parameters[3].Type != DataType::INTEGER)
               throw AlgorithmException(HERE, L"Loop step must be an integer");

            // Determine direction
            bool ascending = (Parameters[3].Value.Int > 0);

            // (iterator) = (inital_value) ± (step_value)
            VString init(L"%s = %s %s %s", iterator, init_val, (ascending ? L"-" : L"+"), step_val);
            nodes += ExpandCommand(init, script.Game);

            // while (iterator) less/greater (final_value)
            VString compare(L"while %s %s %s", iterator, (ascending ? L"<" : L">"), last_val);
            nodes += ExpandCommand(compare, script.Game);

            // (iterator) = (iterator) ± (step_value)
            VString advance(L"%s = %s %s %s", iterator, iterator, (ascending ? L"+" : L"-"), step_val);
            nodes += ExpandCommand(advance, script.Game);

            return nodes;
         }

         /// <summary>Expands macro commands</summary>
         /// <param name="script">script file.</param>
         /// <param name="errors">Errors collection.</param>
         void  CommandTree::ExpandMacros(ScriptFile& script, ErrorArray& errors)
         {
            try
            {
               // Don't expand commented macros
               if (!IsRoot() && !CmdComment && Is(CommandType::Macro))
               {
                  CommandNodeList nodes;

                  // Generate replacement nodes
                  switch (Syntax.ID)
                  {
                  case MACRO_DIM_ARRAY:  nodes = ExpandDimArray(script);
                  case MACRO_FOR_LOOP:   nodes = ExpandForLoop(script);
                  }

                  // Insert following self
                  Children.insert(++Parent->FindChild(this), nodes.begin(), nodes.end());
               }
            }
            catch (ExceptionBase& e) {
               errors += MakeError(GuiString(L"Macro expansion failed: ") + e.Message); 
            }
            
            // Recurse into children  [Allowing for in-place modification of child list]
            for (auto c = Children.begin(); c != Children.end(); )
            {
               auto n = c++;

               // Expand macros
               (*n)->ExpandMacros(script, errors);

               // Delete original
               if ((*n)->Is(CommandType::Macro))
                  Children.erase(n);
            }
         }

         /// <summary>Perform linkage steps that require the entire tree to be linked</summary>
         /// <param name="errors">Errors collection.</param>
         void  CommandTree::FinalizeLinkage(ErrorArray& errors)
         {
            if (!IsRoot())
            {
               // JMP: Set address
               if (Is(CMD_HIDDEN_JUMP))
                  Parameters[0].Value = JumpTarget->Index;

               // Linked to break/continue: Link to associated JMP (1st child)
               if (JumpTarget && (JumpTarget->Is(CMD_BREAK) || JumpTarget->Is(CMD_CONTINUE)))
                  JumpTarget = JumpTarget->Children.begin()->get();

               // Verify linkage
               if (JumpTarget && JumpTarget->Index == EMPTY_JUMP)
                  errors += MakeError( VString(L"Linking failed: Illegal linkage to line %d : '%s'", JumpTarget->LineNumber, JumpTarget->LineCode.c_str()) ); 
            }

            // Recurse into children
            for (auto& c : Children)
               c->FinalizeLinkage(errors);
         }
         
         /// <summary>Compiles the parameters/commands into the script</summary>
         /// <param name="script">script.</param>
         /// <param name="errors">Errors collection.</param>
         void  CommandTree::GenerateCommands(ScriptFile& script, ErrorArray& errors)
         {
            try
            {
               if (!IsRoot())
               {
                  ParameterArray params;

                  // Compile parameters
                  for (auto& p : Parameters)
                  {
                     // Goto/Gosub: Change label number dataType from DT_STRING (ie. label name) into DT_INTEGER. 
                     if (p.Syntax.Type == ParameterType::LABEL_NUMBER && !CmdComment)
                        p.Type = DataType::INTEGER;   // parameter ctor resolves 'label' token type to DT_STRING

                     // Compile
                     p.Generate(script, JumpTarget ? JumpTarget->Index : EMPTY_JUMP, CmdComment);
                  }

                  // Command: Provide compiled parameters in display order
                  if (!Syntax.Is(CMD_EXPRESSION))
                     script.Commands.AddOutput(ScriptCommand(LineText, Syntax, Parameters, CmdComment));
                  else
                  {
                     // Compile postfix parameters
                     for (auto& p : Postfix)
                        p.Generate(script, JumpTarget ? JumpTarget->Index : EMPTY_JUMP, CmdComment);

                     // Expression: Provide compiled parameters in infix & postfix order
                     script.Commands.AddOutput(ScriptCommand(LineText, Syntax, Parameters, Postfix, CmdComment));
                  }
               }
            }
            catch (ExceptionBase& e) {
               errors += MakeError(GuiString(L"Compile failed: ") + e.Message); 
            }
            
            // Recurse into children
            for (auto& c : Children)
               c->GenerateCommands(script, errors);
         }
         
         /// <summary>Gets the last executable child.</summary>
         /// <returns></returns>
         /// <exception cref="Logic::AlgorithmException">No executable children</exception>
         CommandTree* CommandTree::GetLastExecutableChild() const
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
         wstring CommandTree::GetScriptCallName() const
         {
            // Find scriptName parameter
            auto callName = find_if(Parameters.begin(), Parameters.end(), [](const ScriptParameter& s) {return s.Syntax.Usage == ParameterUsage::ScriptName;} );
                     
            // Return name is present, empty string if missing (or a variable)
            return callName != Parameters.end() && callName->Type == DataType::STRING ? callName->Value.String : L"";
         }

         /// <summary>Determines whether has executable child</summary>
         /// <returns></returns>
         bool  CommandTree::HasExecutableChild() const
         {
            return any_of(Children.begin(), Children.end(), isExecutableCommand);
         }

         /// <summary>Maps each variable name to a unique ID, and locates all label definitions</summary>
         /// <param name="script">script.</param>
         /// <param name="errors">Errors collection</param>
         void  CommandTree::IdentifyConstants(ScriptFile& script, ErrorArray& errors) 
         {
            // Skip command comments
            if (!CmdComment)
            {
               // Enumerate assignments (RetVars)
               for (auto& p : Parameters)
               {
                  if (p.Type == DataType::VARIABLE && p.Syntax.IsRetVar() && p.Value.Type == ValueType::String)
                     script.Variables.Add(p.Value.String).Assignment++;
               }
            }

            // Examine children
            for (const auto& cmd : Children)
               cmd->IdentifyConstants(script, errors);
         }

         /// <summary>Maps each variable name to a unique ID, and locates all label definitions</summary>
         /// <param name="script">script.</param>
         /// <param name="errors">Errors collection</param>
         void  CommandTree::IdentifyVariables(ScriptFile& script, ErrorArray& errors) 
         {
            typedef reference_wrapper<ScriptParameter>  ParameterRef;

            // Do not enumerate the labels/variables of command comments  [But do include script-calls]
            if (!CmdComment)
            {
               // Add label definitions to script
               if (Is(CMD_DEFINE_LABEL) && !Parameters.empty()) 
               {
                  auto name = Parameters[0].Value.String;
                  // Ensure unique
                  if (!script.Labels.Add(name, LineNumber))
                     errors += MakeError(VString(L"Label '%s' already defined on line %d", name.c_str(), script.Labels[name].LineNumber), Parameters[0].Token);
               }

               list<ParameterRef> params;

   #ifdef VALIDATION
               // For the sake of producing code that exactly duplicates egosoft code, build the variable names map
               // by enumerating variables in physical syntax order. (Requires all parameters be present)
               if (Parameters.size() == Syntax.ParameterCount)
                  for (const auto& ps : Syntax.Parameters)
                     params.push_back( ref(Parameters[ps.DisplayIndex]) );
               else 
   #endif
                  // Missing/Discarded/vArgs: Enumerate in display order
                  for (auto& p : Parameters)
                     params.push_back(ref(p));

               // Enumerate variables
               for (auto& ref : params)
               {
                  auto& p = ref.get();
                  
                  // Variable: Lookup/Store variable and increment usage
                  if (p.Type == DataType::VARIABLE && p.Value.Type == ValueType::String)
                     script.Variables.Add(p.Value.String).Usage++;
               }
            }

            // Load script-calls for argument type-checking
            if (Syntax.IsScriptCall())
            {
               // Find name of target script (empty if a variable)
               wstring call = GetScriptCallName();

               try {  
                  // Skip reading if previously read
                  if (!call.empty() && !script.ScriptCalls.Contains(call))
                     script.ScriptCalls.Add(call, ScriptFileReader::ReadExternalScript(script.FullPath.Folder, call));
               }
               catch (ExceptionBase&) {
                  //if (e.ErrorID != ERROR_FILE_NOT_FOUND)
                     //Console.Log(HERE, e, VString(L"Unable to resolve call to external script '%s'", call.c_str()));
               }
            }

            // Examine children
            for (const auto& cmd : Children)
               cmd->IdentifyVariables(script, errors);
         }
         
         /// <summary>Calculates the standard command index</summary>
         /// <param name="next">Next index to use</param>
         void CommandTree::IndexCommands(UINT& next)
         {
            // Standard command  
            if (!IsRoot() && !CmdComment && Is(CommandType::Standard))
               Index = next++;

            // Recurse into children
            for (auto c : Children)
               c->IndexCommands(next);
         }

         /// <summary>Inserts an unconditional jump command as the last child</summary>
         /// <param name="target">Command to target</param>
         /// <returns></returns>
         void  CommandTree::InsertJump(NodeIterator pos, const CommandTree* target)
         {
            Children.insert(pos, new CommandTree(*this, target));
         }
         
         /// <summary>Query whether node is root</summary>
         bool  CommandTree::IsRoot() const
         {
            return Parent == nullptr;
         }
         
         /// <summary>Perform command linking</summary>
         /// <param name="errors">errors collection</param>
         /// <exception cref="Logic::AlgorithmException">Error in linking algorithm</exception>
         void  CommandTree::LinkCommands(ErrorArray& errors) 
         {
            try
            {
               CommandTree* n;

               switch (Logic)
               {
               // JIF: ELSE-IF/ELSE/END
               case BranchLogic::If: 
               case BranchLogic::ElseIf: 
                  // JIF: else-if/else/next-std-sibling
                  JumpTarget = FindConditionalAlternate();
               
                  // preceeds ELSE-IF/ELSE: Append child JMP-> next-std-sibling
                  if ((n=FindNextSibling()) && (n->Logic == BranchLogic::Else || n->Logic == BranchLogic::ElseIf))
#ifndef VALIDATION
                     // Don't add JMP if last child is return
                     if (!HasExecutableChild() || !GetLastExecutableChild()->Is(CMD_RETURN))   
#endif
                        // JMP -> end of conditional branch
                        InsertJump(Children.end(), FindConditionalEnd());
                  break;

               // <nothing>
               case BranchLogic::Else:
                  break;    

               // JIF: next-std-sibling
               case BranchLogic::SkipIf:
                  JumpTarget = FindNextCommand();
                  break;

               // JIF: next-std-sibling
               case BranchLogic::While:
                  JumpTarget = FindNextCommand();
                  InsertJump(Children.end(), this); // JMP: WHILE (to create loop)
                  break;

               // JMP: WHILE->next-std-sibling
               case BranchLogic::Break:
                  JumpTarget = FindAncestor(BranchLogic::While)->FindNextCommand();
                  InsertJump(Children.begin(), JumpTarget);
                  break;

               // JMP: WHILE
               case BranchLogic::Continue:
                  JumpTarget = FindAncestor(BranchLogic::While);
                  InsertJump(Children.begin(), JumpTarget);
                  break;

               // JMP: LABEL
               case BranchLogic::None:
                  if (Is(CMD_GOTO_LABEL) || Is(CMD_GOTO_SUB))
                  {
                     JumpTarget = FindRoot()->FindLabel(Parameters[0].Value.String);  

                     if (!JumpTarget)     // Previously identified, should always be found
                        throw AlgorithmException(HERE, VString(L"Cannot find label %s", Parameters[0].Value.String.c_str()));
                  }
                  break;
               }
            }
            catch (ExceptionBase& e) {
               errors += MakeError(GuiString(L"Linking failed: ") + e.Message); 
            }

            // Recurse into children
            for (auto& c : Children)
               c->LinkCommands(errors);
         }
         
         /// <summary>Create error for this entire line</summary>
         /// <param name="msg">error message</param>
         /// <returns></returns>
         ErrorToken  CommandTree::MakeError(const GuiString& msg) const
         {
            return ErrorToken(msg, LineNumber, LineText.substr(Extent.cpMin, Extent.cpMax-Extent.cpMin), Extent);
         }

         /// <summary>Create error for a token on this line</summary>
         /// <param name="msg">error message</param>
         /// <param name="tok">token</param>
         /// <returns></returns>
         ErrorToken  CommandTree::MakeError(const GuiString& msg, const ScriptToken& tok) const
         {
            return ErrorToken(msg, LineNumber, tok);
         }

         /// <summary>Reverts a command comment with verification errors into an ordinary comment</summary>
         /// <param name="child">Child command to revert</param>
         /// <exception cref="Logic::ArgumentNullException">child is null</exception>
         /// <exception cref="Logic::InvalidOperationException">Not a child of current node</exception>
         void CommandTree::RevertCommandComment(CommandTree* child)
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
            ReplaceChild(child, new CommandTree(Conditional::NONE, newSyntax, params, lex, child->LineNumber, false));
         }

         /// <summary>Replaces one child node with another.</summary>
         /// <param name="oldChild">existing child.</param>
         /// <param name="newChild">new replacement child.</param>
         /// <exception cref="Logic::InvalidOperationException">Not a child of current node</exception>
         void CommandTree::ReplaceChild(CommandTree* oldChild, CommandTree* newChild)
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

         /// <summary>Verifies the execution type and parameters</summary>
         /// <param name="script">script</param>
         /// <param name="errors">errors collection</param>
         void  CommandTree::VerifyCommand(const ScriptFile& script, ErrorArray& errors) 
         {
            try
            {
               // Skip for unrecognised commands
               if (Syntax != CommandSyntax::Unrecognised)
               {
                  // CmdComment: Redirect verification errors into dummy queue
                  ErrorArray  commentErrors;
                  ErrorArray& errQueue = (CmdComment ? commentErrors : errors);

                  // Check for invalid 'start' 
                  if (Condition == Conditional::START && Syntax.Execution == ExecutionType::Serial)
                     errQueue += MakeError(L"Command cannot be executed asynchronously");

                  // Check for missing 'start'
                  else if (Condition != Conditional::START && Syntax.Execution == ExecutionType::Concurrent)
                     errQueue += MakeError(L"Command must be executed asynchronously");

                  // Check for macro within 'skip-if'
                  if (Syntax.Is(CommandType::Macro) && Parent->Logic == BranchLogic::SkipIf)
                     errQueue += MakeError(L"Macros cannot be used within skip-if conditional");

                  // Verify parameter values/types
                  UINT index = 0;
                  for (const ScriptParameter& p : Parameters)
                     VerifyParameter(p, index++, script, errQueue);

                  // VARG: Verify argument count     [Genuine CallScript commands can have unlimited arguments]
                  if (Syntax.IsVArgument() && !Syntax.Is(CMD_CALL_SCRIPT) && !CmdComment)
                     if (Parameters.size() > Syntax.MaxParameters)
                        errQueue += MakeError(VString(L"Command may only have up to %d variable arguments", Syntax.VArgCount));

                  // Error in CmdComment: Silently revert to ordinary comment
                  if (CmdComment && !errQueue.empty())
                  {
                     // Replace self with commented node + delete self.
                     Parent->RevertCommandComment(this);    // NB: CmdComments can never have children, their branch logic is always
                     return;                                //     NOP even if they have a conditional, so no need to check children.
                  }
               }
            }
            catch (ExceptionBase& e) {
               errors += MakeError(GuiString(L"Verification failed: ") + e.Message); 
            }

            // Recurse into children
            for (auto& c : Children)
               c->VerifyCommand(script, errors);
         }

         /// <summary>Verifies the branching logic</summary>
         void  CommandTree::VerifyLogic(ErrorArray& errors) const
         {
            CommandTree* n;
            switch (Logic)
            {
            // IF: Must preceed ELSE-IF/ELSE/END
            case BranchLogic::If:
               // EOF?
               if ((n=FindNextSibling()) == nullptr)
                  errors += MakeError(L"missing 'end' command");
               // preceeds End/Else/Else-if?
               else if (n->Logic != BranchLogic::End && n->Logic != BranchLogic::Else && n->Logic != BranchLogic::ElseIf)
                  errors += n->MakeError(L"expected 'else', 'else if' or 'end'");
               break;

            // WHILE: Must preceed END
            case BranchLogic::While:
               // EOF?
               if ((n=FindNextSibling()) == nullptr)
                  errors += MakeError(L"missing 'end' command");
               // preceed END?
               else if (n->Logic != BranchLogic::End)
                  errors += n->MakeError(L"expected 'end'");
               break;

            // ELSE: Must follow IF/ELSE-IF.  Must preceed END
            case BranchLogic::Else:
               // follow IF/ELSE-IF?
               if ((n=FindPrevSibling()) == nullptr || (n->Logic != BranchLogic::If && n->Logic != BranchLogic::ElseIf))
                  errors += MakeError(L"unexpected 'else'");

               // EOF?
               else if ((n=FindNextSibling()) == nullptr)
                  errors += MakeError(L"missing 'end' command");
               // preceed END?
               else if (n->Logic != BranchLogic::End)
                  errors += n->MakeError(L"expected 'end'");
               break;

            // ELSE-IF: Must follow IF/ELSE-IF. Must preceed ELSE-IF/ELSE/END
            case BranchLogic::ElseIf:
               // follow IF/ELSE-IF?
               if ((n=FindPrevSibling()) == nullptr || (n->Logic != BranchLogic::If && n->Logic != BranchLogic::ElseIf))
                  errors += MakeError(L"unexpected 'else-if'");
               
               // EOF?
               else if ((n=FindNextSibling()) == nullptr)
                  errors += MakeError(L"missing 'end' command");
               // preceed ELSE-IF/ELSE/END?
               else if (n->Logic != BranchLogic::Else && n->Logic != BranchLogic::ElseIf && n->Logic != BranchLogic::End)
                  errors += n->MakeError(L"expected 'else', 'else if' or 'end'");
               break;

            // END: Must follow IF/WHILE/ELSE-IF/ELSE
            case BranchLogic::End:
               // follow IF/WHILE/ELSE-IF/ELSE?
               if ((n=FindPrevSibling()) == nullptr || (n->Logic != BranchLogic::If && n->Logic != BranchLogic::While && n->Logic != BranchLogic::ElseIf && n->Logic != BranchLogic::Else))
                  errors += MakeError(L"unexpected 'end' command");
               break;

            // SKIP-IF: Must not be child of SKIP-IF. Must contain 1 standard command
            case BranchLogic::SkipIf:
               // not parent SKIP-IF?
               if (Parent->Logic == BranchLogic::SkipIf)
                  errors += MakeError(L"'skip-if' cannot be nested");

               // Ensure command present
               //if (!cmd->Is(CommandType::Standard) && !cmd->Is(CMD_CONTINUE) && !cmd->Is(CMD_BREAK))
               if (count_if(Children.begin(), Children.end(), isSkipIfCompatible) != 1)
                  errors += MakeError(L"must contain single command without conditional");
               break;

            // BREAK/CONTINUE: Must be decendant of WHILE
            case BranchLogic::Break:
            case BranchLogic::Continue:
               // Check for a parent 'while' command
               if (!FindAncestor(BranchLogic::While))
                  errors += MakeError(L"break/continue cannot appear outside 'while'");
               break;
            }

            // Recurse into children
            for (auto& c : Children)
               c->VerifyLogic(errors);
         }
         
         /// <summary>Verifies object/label names, and performs static type checking on standard and variable arguments</summary>
         /// <param name="p">parameter</param>
         /// <param name="index">display index</param>
         /// <param name="script">script</param>
         /// <param name="errors">errors collection</param>
         void  CommandTree::VerifyParameter(const ScriptParameter& p, UINT index, const ScriptFile& script, ErrorArray& errors) const
         {
            GameObjectLibrary::ObjectID obj;

            // Recognise game/script objects
            switch (p.Token.Type)
            {
            // GameObject: Ensure exists  (allow {SSTYPE_LASER@12} placeholders)
            case TokenType::GameObject:
               if (!GameObjectLib.Contains(p.Value.String) && !GameObjectLib.ParsePlaceholder(p.Value.String, obj))
                  errors += MakeError(L"Unrecognised game object", p.Token);
               break;

            // ScriptObject: Ensure exists 
            case TokenType::ScriptObject:
               if (!ScriptObjectLib.Contains(p.Value.String))
                  errors += MakeError(L"Unrecognised script object", p.Token);
               break;

            // Label: Ensure exists  [Don't check if commented]
            case TokenType::Label:
               if (!CmdComment)
                  if (!script.Labels.Contains(p.Value.String))
                     errors += MakeError(L"Unrecognised label", p.Token);
               break;
            }

            // Varg: Check param vs. target script arguments
            if (p.Syntax == ParameterSyntax::VArgParameter)
            {
               // ScriptCall: 
               if (Syntax.IsScriptCall())
               {  
                  // Find scriptName parameter
                  auto callName = GetScriptCallName();
                     
                  // Skip check if script-name present + script loaded
                  if (!callName.empty() && script.ScriptCalls.Contains(callName))
                  {
                     auto call = script.ScriptCalls.Find(callName);
                     auto argIndex = index-Syntax.ParameterCount;

                     // Verify argument exists
                     if (argIndex >= call.Variables.Arguments.Count)
                     {
                        // Create token representing argument name/value pair
                        ScriptToken tok(TokenType::Text, p.ArgName.Start, p.Token.End, p.ArgName.Text + L"=" + p.Token.Text);
                        errors += MakeError(VString(L"'%s' only has %d arguments", callName.c_str(), call.Variables.Arguments.Count), tok);
                     }
                     // Verify argument name
                     else if (PrefsLib.CheckArgumentNames && !call.Variables.Contains(p.ArgName.Text))
                        errors += MakeError(VString(L"'%s' does not have a '%s' argument", callName.c_str(), p.ArgName.Text.c_str()), p.ArgName);

#ifndef STRICT_VALIDATION   // Skip these checks when validating, some Egosoft scripts have missing script-call arguments
                     // Verify argument type
                     else if (PrefsLib.CheckArgumentTypes)
                     {
                        auto arg = call.Variables[p.ArgName.Text];
                     
                        // Verify argument order
                        if (arg.ID != argIndex)
                           errors += MakeError(VString(L"argument out of order: '%s' must be at index %d", arg.Name.c_str(), arg.ID+1), p.ArgName);

                        // Verify argument type 
                        if (!ParameterSyntax::Verify(arg.ParamType, p.Type))
                           errors += MakeError(VString(L"type mismatch - '%s' is not a valid %s", p.Text.c_str(), ::GetString(arg.ParamType).c_str()), p.Token);
                     }
#endif
                  }
               }
            }
            // Std parameter: Check value vs. type
            else if (!p.Syntax.Verify(p.Type))
               errors += MakeError(VString(L"type mismatch - '%s' is not a valid %s", p.Text.c_str(), ::GetString(p.Syntax.Type).c_str()), p.Token);
         }

         /// <summary>Verifies that all control paths lead to termination.</summary>
         /// <param name="errors">error collection</param>
         /// <remarks>This method is only executed when no syntax errors are present</remarks>
         void  CommandTree::VerifyTermination(ErrorArray& errors) const
         {
            // Check for presence of possible conditional or return cmd
            if (!HasExecutableChild())
               errors += MakeError(L"Not all control paths return a value");
            else
            {
               bool allPaths = false;

               // Find last child (exclude NOP/END)
               auto last = *find_if(Children.rbegin(), Children.rend(), isConditionalAlternate);
               switch (last->Logic)
               {
               // If: Verify branch and any alternates all lead to RETURN
               case BranchLogic::If:
                  last->VerifyTermination(errors);

                  // Verify following Else/ElseIf
                  for (auto n = last->FindNextSibling(); n != nullptr && (n->Logic == BranchLogic::Else || n->Logic == BranchLogic::ElseIf); n = n->FindPrevSibling())
                  {
                     // Cannot guarantee termination without 'Else'
                     if (n->Logic == BranchLogic::Else)
                        allPaths = true;

                     // Verify branch
                     n->VerifyTermination(errors);
                  }

                  // Ensure 'Else' was present
                  if (!allPaths)
                     errors += last->MakeError(L"Not all control paths return a value");
                  break;

               // ElseIf: Find + Verify preceeding 'IF'
               case BranchLogic::Else:
               case BranchLogic::ElseIf:
                  // Simpler if we just verify the preceeding 'If'
                  for (auto n = last->FindPrevSibling(); ;n = n->FindPrevSibling())
                     if (n->Logic == BranchLogic::If)
                        return n->VerifyTermination(errors);
                  break;
                  
               // Command: Verify RETURN
               case BranchLogic::While:      // While is unacceptable because we can't evaluate conditions under which loop will end
               case BranchLogic::SkipIf:     // SkipIf is unacceptable by default because JIF leads to no alternate branch
               case BranchLogic::Break:      
               case BranchLogic::Continue:   // Break/Continue are unacceptable because they're not RETURN
               case BranchLogic::None:
                  if (!last->Is(CMD_RETURN))
                     errors += last->MakeError(L"Last command in script must be 'return'");
                  break;

               default:
                  throw AlgorithmException(HERE, VString(L"Unexpected branching logic '%s' for an executable child", ::GetString(last->Logic)));
               }
            }
         }

      }
   }
}

