#include "stdafx.h"
#include "CommandNode.h"
#include "GameObjectLibrary.h"
#include "ScriptObjectLibrary.h"
#include "ScriptFileReader.h"
#include "ExpressionParser.h"
#include "SyntaxLibrary.h"
#include "ScriptFile.h"


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
         CommandNode::CommandNode(CommandNode* parent, const CommandNode* target)
            : Syntax(SyntaxLib.Find(CMD_HIDDEN_JUMP, GameVersion::Threat)),
              Condition(Conditional::NONE),
              JumpTarget(target),
              Parent(parent),
              LineNumber(parent->LineNumber),
              Extent({0,0}),
              Index(EMPTY_JUMP),
              State(InputState::Raw),
              CmdComment(false)
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
               Console << ENDL << "Ln  Index  Logic            Text        " << Cons::Purple << Cons::Bold << GetString(State);
               Console << ENDL << "-------------------------------------------------------" << ENDL; 
            }
            else
            {
               // Line#/Logic/Text
               GuiString line(!Is(CMD_HIDDEN_JUMP) ? L"%03d: " : L"---: ", LineNumber), 
                         logic(::GetString(Logic)),
                         txt(LineText);
               Cons      colour(Cons::White);
            
               // Index
               line += GuiString(Is(CommandType::Standard) && Index != EMPTY_JUMP ? L"%03d: " : L"---: ", Index);
            
               // Logic
               switch (Logic)
               {
               // Conditional:
               default: 
                  colour = Cons::Cyan;
                  if (JumpTarget)
                     txt = (JumpTarget->Index ? GuiString(L"Jump-if-false: %d", JumpTarget->Index) 
                                              : GuiString(L"<Invalid JumpTarget> : ") + JumpTarget->DebugText);
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
                        txt += GuiString(L"%d", JumpTarget->Index);
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

         /// <summary>Verifies the entire tree</summary>
         /// <param name="script">script</param>
         /// <param name="errors">errors collection</param>
         void  CommandNode::Verify(ScriptFile& script, ErrorArray& errors) 
         {
            // Identify labels/variables
            IdentifyVariables(script, errors);

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
#ifndef VALIDATION
            // Find next Std command that isn't ELSE-IF
            return FindSibling(isConditionalEnd, L"conditional end-point");
#else
            // EOF: Return sentinel
            if (IsRoot())
               return &EndOfScript;

            // Find next sibling node containing a standard command
            auto node = find_if(Parent->FindChild(this)+1, Parent->Children.cend(), isConditionalEnd);

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
            return node+1 != Parent->Children.end() ? node[1].get() : nullptr;
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

         /// <summary>Maps each variable name to a unique ID, and locates all label definitions</summary>
         /// <param name="script">script.</param>
         /// <param name="errors">Errors collection</param>
         void  CommandNode::IdentifyVariables(ScriptFile& script, ErrorArray& errors) 
         {
            // Do not enumerate the labels/variables of command comments  [But do include script-calls]
            if (!CmdComment)
            {
               // Add label definitions to script
               if (Is(CMD_DEFINE_LABEL) && !Parameters.empty()) 
               {
                  auto name = Parameters[0].Value.String;
                  // Ensure unique
                  if (!script.Labels.Add(name, LineNumber))
                     errors += MakeError(GuiString(L"Label '%s' already defined on line %d", name.c_str(), script.Labels[name].LineNumber), Parameters[0].Token);
               }

   #ifdef VALIDATION
               // For the sake of producing code that exactly duplicates egosoft code, build the variable names map
               // by enumerating variables in physical syntax order. (Required all parameters be present)
               if (Parameters.size() == Syntax.Parameters.size())
                  for (const auto& ps : Syntax.Parameters)
                  {
                     ScriptParameter& p = Parameters[ps.DisplayIndex];

                     if (p.Type == DataType::VARIABLE && p.Value.Type == ValueType::String)
                        script.Variables.Add(p.Value.String);
                  }
               else 
   #endif
                  // Missing/Discarded/vArgs: Enumerate in display order
                  for (const auto& p : Parameters)
                     if (p.Type == DataType::VARIABLE && p.Value.Type == ValueType::String)
                        script.Variables.Add(p.Value.String);
            }

            // Load script-calls for argument type-checking
            if (Syntax.IsScriptCall())
            {
               // Find name of target script (empty if a variable)
               wstring call = GetScriptCallName();

               try {  
                  // Skip reading if previously read
                  if (!call.empty() && !script.ScriptCalls.Contains(call))
                     script.ScriptCalls.Add(call, ScriptFileReader::ReadExternalScript(L"D:\\X3 Albion Prelude\\scripts", call));
               }
               catch (ExceptionBase&) {
                  //if (e.ErrorID != ERROR_FILE_NOT_FOUND)
                     //Console.Log(HERE, e, GuiString(L"Unable to resolve call to external script '%s'", call.c_str()));
               }
            }

            // Examine children
            for (const auto& cmd : Children)
               cmd->IdentifyVariables(script, errors);
         }
         
         /// <summary>Calculates the standard command index</summary>
         /// <param name="next">Next index to use</param>
         void CommandNode::IndexCommands(UINT& next)
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

         /// <summary>Reverts a command comment with verification errors into an ordinary comment</summary>
         void CommandNode::RevertCommandComment()
         {
            throw NotImplementedException(HERE, L"Cannot revert command comments");
         }

         /// <summary>Verifies the execution type and parameters</summary>
         /// <param name="script">script</param>
         /// <param name="errors">errors collection</param>
         void  CommandNode::VerifyCommand(const ScriptFile& script, ErrorArray& errors) 
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

                  // Verify parameter values/types
                  UINT index = 0;
                  for (const ScriptParameter& p : Parameters)
                     VerifyParameter(p, index++, script, errQueue);

                  // Verify vArg argument count     [Genuine CallScript commands can have unlimited arguments]
                  if (!Syntax.Is(CMD_CALL_SCRIPT) && !CmdComment)
                     if (Syntax.IsVariableArgument() && Parameters.size() > Syntax.Parameters.size()+Syntax.VarArgCount)
                        errQueue += MakeError(GuiString(L"Command may only have up to %d variable arguments", Syntax.VarArgCount));

                  // Error in CmdComment: Silently revert to ordinary comment
                  if (CmdComment && !errQueue.empty())
                  {
                     // DEBUG:
                     for (auto& err : errQueue)
                        Console << err.Line << " " << err.Message << " " << err.Text << ENDL;
                     RevertCommandComment();
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
         
         /// <summary>Verifies object/label names, and performs static type checking on standard and variable arguments</summary>
         /// <param name="p">parameter</param>
         /// <param name="index">display index</param>
         /// <param name="script">script</param>
         /// <param name="errors">errors collection</param>
         void  CommandNode::VerifyParameter(const ScriptParameter& p, UINT index, const ScriptFile& script, ErrorArray& errors) const
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

            // varg Argument: Lookup script and verify argument name+type
            if (p.Syntax == ParameterSyntax::ScriptCallArgument)
            {
#ifndef VALIDATION
               // Find scriptName parameter
               auto callName = GetScriptCallName();
                     
               // Skip check if name or script is missing 
               if (!callName.empty() && script.ScriptCalls.Contains(callName))
               {
                  auto call = script.ScriptCalls.Find(callName);

                  // Verify argument name
                  if (!call.Variables.Contains(p.ArgName.Text))
                     errors += MakeError(GuiString(L"'%s' does not have a '%s' argument", callName.c_str(), p.ArgName.Text.c_str()), p.ArgName);
                  else 
                  {
                     auto arg = call.Variables[p.ArgName.Text];
                     
                     // Verify argument order
                     if (arg.ID != index-Syntax.Parameters.size())
                        errors += MakeError(GuiString(L"argument out of order: '%s' must be at index %d", arg.Name.c_str(), arg.ID+1), p.ArgName);

                     // Verify argument type 
                     if (!ParameterSyntax::Verify(arg.ValueType, p.Type))
                        errors += MakeError(GuiString(L"type mismatch - '%s' is not a valid %s", p.Text.c_str(), ::GetString(arg.ValueType).c_str()), p.Token);

                  }
               }
#endif
            }
            // Std parameter: Check value vs. type
            else if (!p.Syntax.Verify(p.Type))
               errors += MakeError(GuiString(L"type mismatch - '%s' is not a valid %s", p.Text.c_str(), ::GetString(p.Syntax.Type).c_str()), p.Token);
         }

         /// <summary>Verifies that all control paths lead to termination.</summary>
         /// <param name="errors">error collection</param>
         /// <remarks>This method is only executed when no syntax errors are present</remarks>
         void  CommandNode::VerifyTermination(ErrorArray& errors) const
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
                  throw AlgorithmException(HERE, GuiString(L"Unexpected branching logic '%s' for an executable child", ::GetString(last->Logic)));
               }
            }
         }

      }
   }
}

