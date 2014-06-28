#include "stdafx.h"
#include "CommandNode.h"
#include "ScriptFile.h"

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         // -------------------------------- CONSTRUCTION --------------------------------
         
         /// <summary>Create visitor for distinguishing constants from variables</summary>
         /// <param name="s">script</param>
         /// <param name="e">errors collection</param>
         CommandNode::ConstantIdentifier::ConstantIdentifier(ScriptFile& s, ErrorArray& e) : Script(s), Errors(e)
         {
         }
         
         /// <summary>Nothing</summary>
         CommandNode::ConstantIdentifier::~ConstantIdentifier()
         {
         }
         
         // ------------------------------- STATIC METHODS -------------------------------

         // ------------------------------- PUBLIC METHODS -------------------------------
         
         /// <summary>Distinguishes variables and constants from their usage</summary>
         /// <param name="n">Node</param>
         void  CommandNode::ConstantIdentifier::VisitNode(CommandNode* n) 
         {
            // Skip command comments
            if (!n->CmdComment)
            {
               // Enumerate assignments (RetVars)
               for (auto& p : n->Parameters)
               {
                  if (p.Type == DataType::VARIABLE && p.Syntax.IsRetVar() && p.Value.Type == ValueType::String)
                     Script.Variables.Add(p.Value.String).Assignment++;
               }
            }
         }
         
         // ------------------------------ PROTECTED METHODS -----------------------------

         // ------------------------------- PRIVATE METHODS ------------------------------
      }
   }
}
