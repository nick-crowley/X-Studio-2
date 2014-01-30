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

         ScriptParser::CommandNode::CommandNode(const CommandTree& parent, const ScriptCommand& cmd, UINT line) 
                  : Parent(parent.get()), LineNumber(line), Index(0), JumpTarget(nullptr), Command(cmd), Logic(cmd.Logic)
         {
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
            /*switch (Logic)
            {
            }*/

            // Verify children
            for (const auto& cmd : Children)
               cmd->Verify(err);
         }

         // ------------------------------ PROTECTED METHODS -----------------------------

         // ------------------------------- PRIVATE METHODS ------------------------------
      }
   }
}

