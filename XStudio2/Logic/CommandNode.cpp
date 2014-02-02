#include "stdafx.h"
#include "ScriptParser.h"
#include "GameObjectLibrary.h"
#include "ScriptObjectLibrary.h"
#include "ExpressionParser.h"

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         // -------------------------------- CONSTRUCTION --------------------------------

         /// <summary>Create root node</summary>
         ScriptParser::CommandNode::CommandNode()
            : Syntax(CommandSyntax::Unknown), 
              Condition(Conditional::NONE),
              Parent(nullptr), 
              JumpTarget(nullptr), 
              Index(0), 
              LineNumber(0),
              Extent({0,0})
         {}

         /// <summary>Create node for a script command</summary>
         /// <param name="cnd">conditional.</param>
         /// <param name="syntax">command syntax.</param>
         /// <param name="params">parameters.</param>
         /// <param name="lex">lexer.</param>
         /// <param name="line">1-based line number</param>
         ScriptParser::CommandNode::CommandNode(Conditional cnd, const CommandSyntax& syntax, ParameterArray& params, const CommandLexer& lex, UINT line)
            : Syntax(syntax),
              Condition(cnd),
              Parameters(move(params)),
              LineNumber(line), 
              Extent(lex.Extent), 
              LineText(lex.Input),
              Parent(nullptr), 
              JumpTarget(nullptr), 
              Index(0)
         {}

         /// <summary>Create node for an expression</summary>
         /// <param name="cnd">conditional.</param>
         /// <param name="syntax">command syntax.</param>
         /// <param name="params">infix parameters and retVar</param>
         /// <param name="params">postfix parameters.</param>
         /// <param name="lex">lexer.</param>
         /// <param name="line">1-based line number</param>
         ScriptParser::CommandNode::CommandNode(Conditional cnd, const CommandSyntax& syntax, ParameterArray& infix, ParameterArray& postfix, const CommandLexer& lex, UINT line)
            : CommandNode(cnd, syntax, infix, lex, line)
         {
            Postfix = postfix;
         }

         ScriptParser::CommandNode::~CommandNode()
         {
         }

         // ------------------------------- STATIC METHODS -------------------------------

         // ------------------------------- PUBLIC METHODS -------------------------------
         
         /// <summary>Add child node</summary>
         /// <param name="cmd">The command node</param>
         /// <returns>Command node</returns>
         ScriptParser::CommandNodePtr  ScriptParser::CommandNode::Add(CommandNodePtr node)
         {
            // Set parent and append
            node->Parent = this;
            Children.push_back(node);
            return node;
         }

         /// <summary>Debug print</summary>
         /// <param name="depth">The depth.</param>
         void  ScriptParser::CommandNode::Print(int depth) const
         {
            wstring tab(depth, (WCHAR)L' ');

            // Line/Indent
            Console << GuiString(L"%03d: %s", LineNumber, tab.c_str());
            
            // Logic
            switch (Logic)
            {
            case BranchLogic::If:
            case BranchLogic::While:
            case BranchLogic::Else:
            case BranchLogic::ElseIf:
            case BranchLogic::SkipIf:
            case BranchLogic::End:  
               Console << Colour::Yellow << GetString(Logic) << Colour::White << L" : " << LineText << ENDL;
               break;
            case BranchLogic::Break:
            case BranchLogic::Continue:
               Console << Colour::Green << GetString(Logic) << Colour::White << L" : " << LineText << ENDL;
               break;
            }
            
            // Print Children
            for (auto c : Children)
               c->Print(depth+1);
         }
         
         /// <summary>Verifies the entire tree</summary>
         void  ScriptParser::CommandNode::Verify(ErrorArray& errors) const 
         {
            // isNode
            if (Parent != nullptr)
            {
               // Verify parameters
               VerifyParameters(errors);

               // verify branching logic
               VerifyLogic(errors);
            }

            // Verify children
            for (const auto& cmd : Children)
               cmd->Verify(errors);

            // isRoot
            if (Parent == nullptr)
            {
               // Ensure script has commands
               if (Children.size() == 0)
                  errors += ErrorToken(L"No commands found", LineNumber, Extent);
            
               // Ensure last std command is RETURN
               else for (auto node = Children.rbegin(); node != Children.rend(); ++node)
               {
                  if (node[0]->Syntax.Is(CommandType::Auxiliary))
                     continue;
                  else if (!node[0]->Syntax.Is(CMD_RETURN))
                     errors += ErrorToken(L"Last command in script must be 'return'", node[0]->LineNumber, node[0]->Extent);
                  break;
               }
            }
         }

         // ------------------------------ PROTECTED METHODS -----------------------------
         
         // ------------------------------- PRIVATE METHODS ------------------------------

         /// <summary>Check children for presence of certain branch logic</summary>
         /// <param name="l">logic</param>
         /// <returns></returns>
         bool  ScriptParser::CommandNode::Contains(BranchLogic l) const
         {
            return find_if(Children.begin(), Children.end(), [=](const CommandNodePtr& t){ return t->Logic == l; }) != Children.end();
         }
         
         /// <summary>Finds the first child with certain branch logic</summary>
         /// <param name="l">desired logic</param>
         /// <returns>position if found, otherwise end</returns>
         ScriptParser::CommandNode::NodeIterator  ScriptParser::CommandNode::Find(BranchLogic l) const
         {
            return find_if(Children.begin(), Children.end(), [l](const CommandNodePtr& n) {return n->Logic == l;} );
         }
         
         /// <summary>Identifies branch logic</summary>
         BranchLogic  ScriptParser::CommandNode::GetBranchLogic() const
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

         /// <summary>Verifies the branching logic</summary>
         void  ScriptParser::CommandNode::VerifyLogic(ErrorArray& errors) const
         {
            // Check for END
            switch (Logic)
            {
            case BranchLogic::If:
            case BranchLogic::While:
               if (Logic == BranchLogic::If)
               {
                  auto Else = Find(BranchLogic::Else);
                  auto ElseIf = Find(BranchLogic::ElseIf);

                  // Ensure 'else-if' does not follow 'else'
                  if (Else != Children.end() && ElseIf != Children.end() && Else < ElseIf)
                     errors += ErrorToken(L"'else-if' must come before 'else' command", (*ElseIf)->LineNumber, (*ElseIf)->Extent);
               }

               // Ensure 'end' is present
               if (Find(BranchLogic::End) == Children.end())
                  errors += ErrorToken(L"missing 'end' command", LineNumber, Extent);
               break;

            case BranchLogic::End:
               // Check for parent 'if'/'while' command
               if (Parent->Logic != BranchLogic::While && Parent->Logic != BranchLogic::If)
                  errors += ErrorToken(L"unexpected 'end' command", LineNumber, Extent);
               break;

            case BranchLogic::Break:
            case BranchLogic::Continue:
               // Check for a parent 'while' command
               for (const CommandNode* n = Parent; n != nullptr; n = n->Parent)
                  if (n->Logic == BranchLogic::While)
                     return;
               
               // Missing 
               errors += ErrorToken(L"break/continue outside 'while' conditional", LineNumber, Extent);
               break;

            case BranchLogic::Else:
            case BranchLogic::ElseIf:
               // Ensure within 'if' command
               if (Parent->Logic != BranchLogic::If)
                  errors += ErrorToken(L"else/else-if outside 'if' conditional", LineNumber, Extent);
               break;

            case BranchLogic::SkipIf:
               // Ensure standard command present
               for (auto node = Children.begin(); node != Children.end(); ++node)
                  if (node[0]->Syntax.Is(CommandType::Auxiliary))
                     continue;
                  else if (node[0]->Logic != BranchLogic::None)
                     errors += ErrorToken(L"not supported within 'skip if' conditional", node[0]->LineNumber, node[0]->Extent);
                  else
                     return;
               break;
            }
         }
         
         
         /// <summary>Converts parameter tokens into ordered list of script parameters</summary>
         void  ScriptParser::CommandNode::VerifyParameters(ErrorArray& errors) const
         {
            // Skip for unrecognised commands
            if (Syntax == CommandSyntax::Unknown)
               return;

            //
            for (const ScriptParameter& param : Parameters)
            {
               // Skip for expressions/script-args
               if (param.Syntax.Type == ParameterType::EXPRESSION || param.Syntax.Type == ParameterType::PARAMETER)
                  continue;
               
               // Static type check
               if (!param.Syntax.Verify(param.Type))
                  errors += ErrorToken(GuiString(L"'%s' is not a valid %s", param.Text.c_str(), GetString(param.Syntax.Type).c_str())
                                                                          , LineNumber, param.Token);
            }
         }

      }
   }
}

