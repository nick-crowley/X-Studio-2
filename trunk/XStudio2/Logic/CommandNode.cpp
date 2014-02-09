#include "stdafx.h"
#include "CommandNode.h"
#include "GameObjectLibrary.h"
#include "ScriptObjectLibrary.h"
#include "ExpressionParser.h"
#include "SyntaxLibrary.h"
#include "ScriptFile.h"


namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         
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
              State(InputState::Raw)
         {}

         /// <summary>Create node for a hidden jump command</summary>
         /// <param name="parent">parent node</param>
         /// <param name="target">target node</param>
         CommandNode::CommandNode(CommandNode* parent, const CommandNode* target)
            : Syntax(SyntaxLib.Find(CMD_HIDDEN_JUMP, GameVersion::Threat)),
              Condition(Conditional::NONE),
              JumpTarget(target),
              Parent(parent),
              LineNumber(parent->LineNumber),
              Extent({0,0}),
              Index(EMPTY_JUMP),
              State(InputState::Raw)
         {
            REQUIRED(parent);
            REQUIRED(target);

            // Add empty jump parameter
            Parameters += ScriptParameter(ParameterSyntax::LabelNumberParameter, DataType::INTEGER, EMPTY_JUMP);
         }

         /// <summary>Create node for a script command</summary>
         /// <param name="cnd">conditional.</param>
         /// <param name="syntax">command syntax.</param>
         /// <param name="params">parameters.</param>
         /// <param name="lex">lexer.</param>
         /// <param name="line">1-based line number</param>
         CommandNode::CommandNode(Conditional cnd, CommandSyntaxRef syntax, ParameterArray& params, const CommandLexer& lex, UINT line)
            : Syntax(syntax),
              Condition(cnd),
              Parameters(move(params)),
              LineNumber(line), 
              Extent(lex.Extent), 
              LineText(lex.Input),
              Parent(nullptr), 
              JumpTarget(nullptr), 
              Index(EMPTY_JUMP),
              State(InputState::Raw)
         {}

         /// <summary>Create node for an expression</summary>
         /// <param name="cnd">conditional.</param>
         /// <param name="syntax">command syntax.</param>
         /// <param name="params">infix parameters and retVar</param>
         /// <param name="params">postfix parameters.</param>
         /// <param name="lex">lexer.</param>
         /// <param name="line">1-based line number</param>
         CommandNode::CommandNode(Conditional cnd, CommandSyntaxRef syntax, ParameterArray& infix, ParameterArray& postfix, const CommandLexer& lex, UINT line)
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
              State(InputState::Raw)
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
            return n->Is(CommandType::Standard) || n->Is(CMD_BREAK) || n->Is(CMD_CONTINUE); 
         };

         /// <summary>Checks whether commands are standard</summary>
         CommandNode::NodeDelegate  CommandNode::isStandardCommand = [](const CommandNodePtr& n) 
         { 
            return n->Is(CommandType::Standard); 
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
         
         /// <summary>Finds first standard command after any else/else-if conditionals</summary>
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

         /// <summary>Finds next conditional or command following failed if/else-if</summary>
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

         /// <summary>Compiles the script.</summary>
         /// <param name="script">The script.</param>
         /// <exception cref="Logic::AlgorithmException">Error in linking algorithm</exception>
         void  CommandNode::Compile(ScriptFile& script, ErrorArray& errors)
         {
            // Perform linking
            LinkCommands(errors);

            // Index commands
            UINT index = 0;
            IndexCommands(index);

            // Finalise linkage
            FinalizeLinkage(errors);
            
            // Compile commands
            GenerateCommands(script, errors);

            // Update state
            State = InputState::Compiled;
         }
         
         /// <summary>Query command syntax ID</summary>
         bool  CommandNode::Is(UINT ID) const
         {
            return Syntax.Is(ID);
         }

         /// <summary>Query command syntax type</summary>
         bool  CommandNode::Is(CommandType t) const
         {
            return Syntax.Is(t);
         }

         /// <summary>Identifies branch logic</summary>
         BranchLogic  CommandNode::GetBranchLogic() const
         {
            // Command
            switch (Syntax.ID)
            {
            case CMD_END:      return BranchLogic::End;
            case CMD_ELSE:     return BranchLogic::Else;
            case CMD_BREAK:    return BranchLogic::Break;
            case CMD_CONTINUE: return BranchLogic::Continue;

            case CMD_COMMAND_COMMENT:
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
         
         /// <summary>Debug print</summary>
         /// <param name="depth">The depth.</param>
         void  CommandNode::Print(int depth) const
         {
            if (IsRoot())
            {
               Console << ENDL << "Ln  Index  Logic            Text        " << Colour::Purple << Cons::Bold << GetString(State);
               Console << ENDL << "-------------------------------------------------------" << ENDL; 
            }
            else
            {
               // Line#/Logic/Text
               GuiString line(!Is(CMD_HIDDEN_JUMP) ? L"%03d: " : L"---: ", LineNumber), 
                         logic(::GetString(Logic)),
                         txt(LineText);
               Colour    colour(Colour::White);
            
               // Index
               line += GuiString(Is(CommandType::Standard) && Index != EMPTY_JUMP ? L"%03d: " : L"---: ", Index);
            
               // Logic
               switch (Logic)
               {
               // Conditional:
               default: 
                  colour = Colour::Cyan;
                  if (JumpTarget)
                     txt = (JumpTarget->Index ? GuiString(L"Jump-if-false: %d", JumpTarget->Index) 
                                              : GuiString(L"<Invalid JumpTarget> : ") + JumpTarget->DebugText);
                  break;

               // NOP:
               case BranchLogic::NOP:
                  colour = Colour::Yellow;
                  break;

               // Command:
               case BranchLogic::None:
                  if (Is(CMD_HIDDEN_JUMP) || Is(CMD_GOTO_LABEL) || Is(CMD_GOTO_SUB))
                  {
                     colour = Colour::Green; 
                     logic = Is(CMD_HIDDEN_JUMP) ? L"Jmp" : L"Goto";
                     txt = L"Unconditional: ";

                     // Post-Compile: Display label number 
                     if (JumpTarget)
                        txt += GuiString(L"%d", JumpTarget->Index);
                     // Pre-Compile: Label name
                     else 
                        txt += (!Parameters.empty() ? Parameters[0].Token.Text : L"<missing>");
                  }
                  else if (Is(CMD_DEFINE_LABEL))
                  {
                     colour = Colour::Purple;
                     logic = L"Proc";
                  }
                  else if (Syntax == CommandSyntax::Unrecognised)
                  {  // Print entire line in red
                     Console << (colour = Colour::Red);
                     logic = L"???";
                  }
                  else
                     logic = L"Cmd";
                  break;
               }

               // Print
               Console << line+wstring(depth, L' ') << colour << logic << Colour::White << L" : " << colour << txt.TrimLeft(L" ") << ENDL;
            }

            // Print Children
            for (auto c : Children)
               c->Print(depth+1);

            // Spacing
            if (IsRoot())
               Console << ENDL;
         }

         /// <summary>Verifies the entire tree</summary>
         /// <param name="script">script</param>
         /// <param name="errors">errors collection</param>
         void  CommandNode::Verify(ScriptFile& script, ErrorArray& errors) 
         {
            // Identify labels/variables
            IdentifyVariables(script);

            // parameters
            VerifyCommand(script, errors);

            // branching logic
            VerifyLogic(errors);

            // Ensure script has commands
            if (count_if(Children.begin(), Children.end(), isStandardCommand) == 0)
               errors += MakeError(L"No executable commands found");
            
            // Ensure last std command is RETURN
            else if (find_if(Children.rbegin(), Children.rend(), isStandardCommand) == Children.rend())
            {
               auto last = Children.end()[-1];
               if (!last->Is(CMD_RETURN))
                  errors += last->MakeError(L"Last command in script must be 'return'");
            }

            // Update state
            State = InputState::Verified;
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
               else if (!isExecutableCommand(node->Children[0])) 
                  node = node->Children[0]->FindNextCommand();
               else
                  // Default: Use first child
                  node = node->Children[0].get();
            }

            return node;
         }

         /// <summary>Finds the command to execute following a failed if/else-if statement</summary>
         /// <returns></returns>
         CommandNode* CommandNode::FindConditionalEnd() const
         {
            // Find next Std command that isn't ELSE-IF
            return FindSibling(isConditionalEnd, L"conditional end-point");
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
         CommandNode* CommandNode::FindNextSibling() const
         {
            auto node = Parent->FindChild(this);
            return node+1 != Parent->Children.end() ? node[1].get() : nullptr;
         }

         /// <summary>Finds the prev sibling of this node</summary>
         /// <returns></returns>
         CommandNode* CommandNode::FindPrevSibling() const
         {
            auto node = Parent->FindChild(this);
            return node != Parent->Children.begin() ? node[-1].get() : nullptr;
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
               throw AlgorithmException(HERE, GuiString(L"Cannot find %s", help));

            // Find next sibling node containing a standard command
            auto node = find_if(Parent->FindChild(this)+1, Parent->Children.cend(), d);

            // Return if found, else recurse into parent
            return node != Parent->Children.cend() ? node->get() : Parent->FindSibling(d, help);
         }

         /// <summary>Perform linkage steps that require the entire tree to be linked</summary>
         /// <param name="errors">Errors collection.</param>
         void  CommandNode::FinalizeLinkage(ErrorArray& errors)
         {
            if (!IsRoot())
            {
               // JMP: Set address
               if (Is(CMD_HIDDEN_JUMP))
                  Parameters[0].Value = JumpTarget->Index;

               // Linked to break/continue: Link to associated JMP (1st child)
               if (JumpTarget && (JumpTarget->Is(CMD_BREAK) || JumpTarget->Is(CMD_CONTINUE)))
                  JumpTarget = JumpTarget->Children[0].get();

               // Verify linkage
               if (JumpTarget && JumpTarget->Index == EMPTY_JUMP)
                  errors += MakeError( GuiString(L"Linking failed: Illegal linkage to line %d : '%s'", JumpTarget->LineNumber, JumpTarget->DebugText.c_str()) ); 
            }

            // Recurse into children
            for (auto& c : Children)
               c->FinalizeLinkage(errors);
         }

         /// <summary>Compiles the parameters/commands into the script</summary>
         /// <param name="script">script.</param>
         /// <param name="errors">Errors collection.</param>
         void  CommandNode::GenerateCommands(ScriptFile& script, ErrorArray& errors)
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
                     if (p.Syntax.Type == ParameterType::LABEL_NUMBER)
                        p.Type = DataType::INTEGER;   // parameter ctor resolves 'label' token type to DT_STRING

                     // Compile
                     p.Generate(script, JumpTarget ? JumpTarget->Index : EMPTY_JUMP);
                  }

                  // Generate & insert command
                  if (!Syntax.Is(CMD_EXPRESSION))
                     script.Commands.AddOutput(ScriptCommand(LineText, Syntax, Parameters));
                  else
                  {
                     // Compile postfix parameters
                     for (auto& p : Postfix)
                        p.Generate(script, JumpTarget ? JumpTarget->Index : EMPTY_JUMP);

                     // Generate & insert command
                     script.Commands.AddOutput(ScriptCommand(LineText, Syntax, Parameters, Postfix));
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

         /// <summary>Maps each variable name to a unique ID, and locates all label definitions</summary>
         /// <param name="script">script.</param>
         void  CommandNode::IdentifyVariables(ScriptFile& script) 
         {
            // Add label definitions to script
            if (Is(CMD_DEFINE_LABEL) && !Parameters.empty()) 
               script.Labels.Add(Parameters[0].Value.String, LineNumber);

            // For the sake of producing code that exactly duplicates egosoft code, build the variable names map
            // by enumerating variables in physical syntax order. (if all parameters are present)
            if (Parameters.size() == Syntax.Parameters.size())
               for (const auto& ps : Syntax.Parameters)
               {
                  ScriptParameter& p = Parameters[ps.DisplayIndex];

                  if (p.Type == DataType::VARIABLE && p.Value.Type == ValueType::String)
                     script.Variables.Add(p.Value.String);
               }
            // Missing/Discarded/vArgs: Enumerate in display order
            else 
               for (const auto& p : Parameters)
                  if (p.Type == DataType::VARIABLE && p.Value.Type == ValueType::String)
                     script.Variables.Add(p.Value.String);

            // Examine children
            for (const auto& cmd : Children)
               cmd->IdentifyVariables(script);
         }
         
         /// <summary>Calculates the standard command index</summary>
         /// <param name="next">Next index to use</param>
         void CommandNode::IndexCommands(UINT& next)
         {
            // Standard
            if (!IsRoot() && Is(CommandType::Standard))
               Index = next++;

            // Recurse into children
            for (auto c : Children)
               c->IndexCommands(next);
         }

         /// <summary>Inserts an unconditional jump command as the last child</summary>
         /// <param name="target">Command to target</param>
         /// <returns></returns>
         void  CommandNode::InsertJump(NodeIterator pos, const CommandNode* target)
         {
            Children.insert(pos, new CommandNode(this, target));
         }
         
         /// <summary>Get line text without indentation</summary>
         GuiString   CommandNode::GetDebugText() const
         {
            return LineText.TrimLeft(L" ");
         }

         /// <summary>Query whether node is root</summary>
         bool  CommandNode::IsRoot() const
         {
            return Parent == nullptr;
         }
         
         /// <summary>Perform command linking</summary>
         /// <param name="errors">errors collection</param>
         /// <exception cref="Logic::AlgorithmException">Error in linking algorithm</exception>
         void  CommandNode::LinkCommands(ErrorArray& errors) 
         {
            try
            {
               CommandNode* n;

               switch (Logic)
               {
               // JIF: ELSE-IF/ELSE/END
               case BranchLogic::If: 
               case BranchLogic::ElseIf: 
                  // JIF: else-if/else/next-std-sibling
                  JumpTarget = FindConditionalAlternate();
               
                  // preceeds ELSE-IF/ELSE: Append child JMP-> next-std-sibling
                  if ((n=FindNextSibling()) && (n->Logic == BranchLogic::Else || n->Logic == BranchLogic::ElseIf))
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
                        throw AlgorithmException(HERE, GuiString(L"Cannot find label %s", Parameters[0].Value.String.c_str()));
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
         
         ErrorToken  CommandNode::MakeError(const GuiString& msg) const
         {
            return ErrorToken(msg, LineNumber, LineText.substr(Extent.cpMin, Extent.cpMax-Extent.cpMin), Extent);
         }

         ErrorToken  CommandNode::MakeError(const GuiString& msg, const ScriptToken& tok) const
         {
            return ErrorToken(msg, LineNumber, tok);
         }

         /// <summary>Converts parameter tokens into ordered list of script parameters</summary>
         /// <param name="script">script</param>
         /// <param name="errors">errors collection</param>
         void  CommandNode::VerifyCommand(const ScriptFile& script, ErrorArray& errors) const
         {
            // Skip for unrecognised commands
            if (Syntax != CommandSyntax::Unrecognised)
            {
               // Check for invalid 'start' 
               if (Condition == Conditional::START && Syntax.Execution == ExecutionType::Serial)
                  errors += MakeError(L"Command cannot be executed asynchronously");

               // Check for missing 'start'
               else if (Condition != Conditional::START && Syntax.Execution == ExecutionType::Concurrent)
                  errors += MakeError(L"Command must be executed asynchronously");

               // Parameter static type check
               for (const ScriptParameter& p : Parameters)
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

                  // Label: Ensure exists
                  case TokenType::Label:
                     if (!script.Labels.Contains(p.Value.String))
                        errors += MakeError(L"Unrecognised label", p.Token);
                     break;
                  }
               
                  // Static type check
                  if (!p.Syntax.Verify(p.Type))
                     errors += MakeError(GuiString(L"'%s' is not a valid %s", p.Text.c_str(), ::GetString(p.Syntax.Type).c_str()), p.Token);
               }
            }

            // Recurse into children
            for (auto& c : Children)
               c->VerifyCommand(script, errors);
         }

         /// <summary>Verifies the branching logic</summary>
         void  CommandNode::VerifyLogic(ErrorArray& errors) const
         {
            CommandNode* n;
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
         
      }
   }
}

