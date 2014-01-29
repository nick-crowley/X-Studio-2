#include "stdafx.h"
#include "ScriptParser.h"

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         // -------------------------------- CONSTRUCTION --------------------------------

         ScriptParser::ScriptTree::ScriptTree(ErrorArray& err) : Errors(err)
         {
         }


         ScriptParser::ScriptTree::~ScriptTree()
         {
         }

         // ------------------------------- STATIC METHODS -------------------------------

         // ------------------------------- PUBLIC METHODS -------------------------------

         /// <summary>Add a command to the tree</summary>
         /// <param name="t">command</param>
         void  ScriptParser::ScriptTree::Add(CommandTree t)
         { 
            Commands.push_back(t); 
         }

         /// <summary>Compiles commands</summary>
         void  ScriptParser::ScriptTree::Compile()
         {
            for (auto& cmd : Commands)
               cmd->Verify(Errors);
         }

         // ------------------------------ PROTECTED METHODS -----------------------------

         // ------------------------------- PRIVATE METHODS ------------------------------
      }
   }
}

