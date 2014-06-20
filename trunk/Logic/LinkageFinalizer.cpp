#include "stdafx.h"
#include "CommandTree.h"

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         // -------------------------------- CONSTRUCTION --------------------------------

         /// <summary>Create visitor for finalizing node linkage</summary>
         /// <param name="e">errors collection</param>
         CommandTree::LinkageFinalizer::LinkageFinalizer(ErrorArray& e) : Errors(e)
         {
         }

         /// <summary>Nothing</summary>
         CommandTree::LinkageFinalizer::~LinkageFinalizer()
         {
         }
         
         // ------------------------------- STATIC METHODS -------------------------------

         // ------------------------------- PUBLIC METHODS -------------------------------
         
         /// <summary>Finalizes linkage between nodes</summary>
         /// <param name="n">Node</param>
         void  CommandTree::LinkageFinalizer::VisitNode(CommandTree* n) 
         {
            // JMP: Set address
            if (n->Is(CMD_HIDDEN_JUMP))
            {
               if (!n->JumpTarget)
                  throw AlgorithmException(HERE, L"JMP command with unassigned address");

               n->Parameters[0].Value = n->JumpTarget->Index;
            }

            // Linked to break/continue: Link to associated JMP (1st child)
            if (n->JumpTarget && (n->JumpTarget->Is(CMD_BREAK) || n->JumpTarget->Is(CMD_CONTINUE)))
               n->JumpTarget = n->JumpTarget->Children.begin()->get();

            // Verify linkage
            if (n->JumpTarget && n->JumpTarget->Index == EMPTY_JUMP)
               Errors += n->MakeError( VString(L"Linking failed: Illegal linkage to line %d : '%s'", n->JumpTarget->LineNumber, n->JumpTarget->LineCode.c_str()) ); 
         }
         
         // ------------------------------ PROTECTED METHODS -----------------------------

         // ------------------------------- PRIVATE METHODS ------------------------------
      }
   }
}
