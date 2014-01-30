#include "stdafx.h"
#include "ScriptParser.h"

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         // -------------------------------- CONSTRUCTION --------------------------------

         ScriptParser::ScriptTree::ScriptTree()
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

         /// <summary>Verifies the parsed commands</summary>
         void  ScriptParser::ScriptTree::Verify(ErrorArray& err) const
         {
            for (auto& cmd : Commands)
               cmd->Verify(err);
         }

         // ------------------------------ PROTECTED METHODS -----------------------------

         // ------------------------------- PRIVATE METHODS ------------------------------
      }
   }
}

