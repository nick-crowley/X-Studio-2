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
              CmdComment(commented)
         {}

         CommandNode::~CommandNode()
         {}

         // ------------------------------- STATIC METHODS -------------------------------
         
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
            const CommandNode* n;
            // Search ancestors
            for (n = this; !n->IsRoot(); n = n->Parent)
               ;
            return const_cast<CommandNode*>(n);
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

         /// <summary>Gets the jump destination, if any, for this node</summary>
         /// <returns>Jump address if any, otherwise </returns>
         UINT CommandNode::GetJumpAddress() const
         {
            return JumpTarget ? JumpTarget->Index : EMPTY_JUMP;
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

         /// <summary>Get line text without indentation</summary>
         GuiString   CommandNode::GetLineCode() const
         {
            return LineText.TrimLeft(L" ");
         }
         
         /// <summary>Gets the name of the script (if any) called by this command</summary>
         /// <returns>Script name, or empty string if defined by variable</returns>
         wstring CommandNode::GetScriptCallName() const
         {
            // Find scriptName parameter
            auto callName = find_if(Parameters.begin(), Parameters.end(), [](const ScriptParameter& s) {
               return s.Syntax.Usage == ParameterUsage::ScriptName; });
                     
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
         
         /// <summary>Transfer all children to another node.</summary>
         /// <param name="n">Destination node.</param>
         /// <exception cref="Logic::AlgorithmException">Source and destination node are the same</exception>
         void CommandNode::MoveChildren(CommandNode& n)
         {
            if (this == &n)
               throw AlgorithmException(HERE, L"Cannot transfer children to itself");

            // Move children 
            for (auto& c = Children.begin(); c != Children.end(); )
            {
               n += *c;
               Children.erase(c++);
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
         
         /// <summary>Append node as a child</summary>
         /// <param name="n">node</param>
         /// <returns>Self</returns>
         CommandNode& CommandNode::operator+=(const CommandNodePtr& n)
         {
            // Set parent 
            n->Parent = this;

            // Append child
            Children.push_back(n);
            return *this;
         }
         
         /// <summary>Remove a child node</summary>
         /// <param name="n">node to remove</param>
         /// <returns>Self</returns>
         /// <exception cref="Logic::ArgumentNullException">Node is null</exception>
         /// <exception cref="Logic::InvalidOperationException">Node is not found</exception>
         CommandNode& CommandNode::operator-=(const CommandNodePtr& n)
         {
            REQUIRED(n);
            NodeIterator pos = FindChild(n.get());

            // Ensure exists
            if (pos == Children.end())
               throw InvalidOperationException(HERE, L"Child node not found");

            // Remove
            Children.erase(pos);
            return *this;
         }

         // ------------------------------ PROTECTED METHODS -----------------------------
         
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

         // ------------------------------- PRIVATE METHODS ------------------------------


      }
   }
}

