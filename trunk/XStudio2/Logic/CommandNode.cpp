#include "stdafx.h"
#include "ScriptParser.h"
#include "GameObjectLibrary.h"
#include "ScriptObjectLibrary.h"

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         // -------------------------------- CONSTRUCTION --------------------------------

         /// <summary>Create root node</summary>
         ScriptParser::CommandNode::CommandNode()
            : Parent(nullptr), JumpTarget(nullptr), Command(ScriptCommand::Unknown), Logic(BranchLogic::None),
              Index(0), LineNumber(0), LineText({0,0})
         {
         }

         /// <summary>Create node from a script command</summary>
         /// <param name="parent">parent node</param>
         /// <param name="cmd">script command.</param>
         /// <param name="line">1-based line number</param>
         ScriptParser::CommandNode::CommandNode(const CommandTree& parent, const ScriptCommand& cmd, const CommandLexer& lex, UINT line)
            : Parent(parent.get()), JumpTarget(nullptr), Command(cmd), Logic(cmd.Logic),
              Index(0), LineNumber(line), LineText(lex.Extent)
         {
            REQUIRED(parent);
         }

         ScriptParser::CommandNode::~CommandNode()
         {
         }

         // ------------------------------- STATIC METHODS -------------------------------

         // ------------------------------- PUBLIC METHODS -------------------------------
         
         /// <summary>Add child node</summary>
         /// <param name="cmd">The command node</param>
         void  ScriptParser::CommandNode::Add(const CommandTree& cmd)
         {
            Children.push_back(cmd);
         }

         /// <summary>Check children for presence of certain branch logic</summary>
         /// <param name="l">logic</param>
         /// <returns></returns>
         bool  ScriptParser::CommandNode::Contains(BranchLogic l) const
         {
            return find_if(Children.begin(), Children.end(), [=](const CommandTree& t){ return t->Logic == l; }) != Children.end();
         }

         /// <summary>Finds the first child with certain branch logic</summary>
         /// <param name="l">desired logic</param>
         /// <returns>position if found, otherwise end</returns>
         ScriptParser::CommandNode::NodeIterator  ScriptParser::CommandNode::Find(BranchLogic l) const
         {
            return find_if(Children.begin(), Children.end(), [l](CommandTree& n) {return n->Logic == l;} );
         }

         /// <summary>Query whether this node is the root</summary>
         bool  ScriptParser::CommandNode::IsRoot() const
         {
            return Parent == nullptr;
         }

         /// <summary>Debug print</summary>
         /// <param name="depth">The depth.</param>
         void  ScriptParser::CommandNode::Print(int depth) const
         {
            wstring tab(depth, (WCHAR)L' ');

            Console.Writef(L"%03d: %s%s : ", LineNumber, tab.c_str(), GetString(Logic));
            Console.WriteLn(Command.Syntax == CommandSyntax::Unknown ? Command.Text : Command.Syntax.Text);
                  
            for (auto c : Children)
               c->Print(depth+1);
         }

         /// <summary>Verifies the node</summary>
         /// <param name="err">The error collection</param>
         void  ScriptParser::CommandNode::Verify(ErrorArray& err) const
         {
            if (!IsRoot())
            {
               // Recognise objects
               for (const auto& p : Command.Parameters)
               {
                  // Verify game object
                  if (p.Token.Type == TokenType::GameObject && !GameObjectLib.Contains(p.Token.ValueText))
                     err += ErrorToken(L"Unrecognised game object", LineNumber, p.Token);

                  // Verify script object
                  else if (p.Token.Type == TokenType::ScriptObject && !ScriptObjectLib.Contains(p.Token.ValueText))
                     err += ErrorToken(L"Unrecognised script object", LineNumber, p.Token);
               }

               // Check for END
               switch (Logic)
               {
               case BranchLogic::If:
               case BranchLogic::While:
                  break;
               }
            }

            // Verify children
            for (const auto& cmd : Children)
               cmd->Verify(err);
         }

         // ------------------------------ PROTECTED METHODS -----------------------------

         /// <summary>Verifies the node</summary>
         /// <param name="err">The error collection</param>
         void  ScriptParser::CommandNode::VerifyLogic(ErrorArray& err) const
         {
            // Check for END
            switch (Logic)
            {
            case BranchLogic::If:
            case BranchLogic::While:
               // Ensure 'end' is last command
               if (Find(BranchLogic::End) == Children.end())
                  err += ErrorToken(L"missing 'end' command", LineNumber, LineText);
               break;

            case BranchLogic::End:
               // Check for parent 'if'/'while' command
               if (Parent->Logic == BranchLogic::While || Parent->Logic == BranchLogic::If)
                  return;
               
               // Missing 
               err += ErrorToken(L"unexpected 'end' command", LineNumber, LineText);
               break;

            case BranchLogic::Break:
            case BranchLogic::Continue:
               // Check for a parent 'while' command
               for (const CommandNode* n = Parent; n != nullptr; n = n->Parent)
                  if (n->Logic == BranchLogic::While)
                     return;
               
               // Missing 
               err += ErrorToken(L"break/continue outside 'while' conditional", LineNumber, LineText);
               break;

            case BranchLogic::Else:
            case BranchLogic::ElseIf:
               // Check for a parent 'if' command
               for (const CommandNode* n = Parent; n != nullptr; n = n->Parent)
                  if (n->Logic == BranchLogic::If)
                     return;
               
               // Missing 
               err += ErrorToken(L"else/else-if outside 'if' conditional", LineNumber, LineText);
               break;

            default:
               return;
            }
         }

         // ------------------------------- PRIVATE METHODS ------------------------------
      }
   }
}

