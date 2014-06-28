#include "stdafx.h"
#include "CommandNode.h"

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         // -------------------------------- CONSTRUCTION --------------------------------

         /// <summary>Create linking visitor</summary>
         /// <param name="e">errors collection</param>
         CommandTree::NodeLinker::NodeLinker(ErrorArray& e) : Errors(e)
         {
         }
         
         /// <summary>Nothing</summary>
         CommandTree::NodeLinker::~NodeLinker()
         {
         }

         // ------------------------------- STATIC METHODS -------------------------------

         // ------------------------------- PUBLIC METHODS -------------------------------
         
         /// <summary>Perform command linking</summary>
         /// <param name="n">Node</param>
         /// <exception cref="Logic::AlgorithmException">Error in linking algorithm</exception>
         void  CommandTree::NodeLinker::VisitNode(CommandNode* n) 
         {
            try
            {
               CommandNode* p;

               switch (n->Logic)
               {
               // JIF: ELSE-IF/ELSE/END
               case BranchLogic::If: 
               case BranchLogic::ElseIf: 
                  // JIF: else-if/else/next-std-sibling
                  n->JumpTarget = n->FindConditionalAlternate();
               
                  // preceeds ELSE-IF/ELSE: Append child JMP-> next-std-sibling
                  if ((p=n->FindNextSibling()) && (p->Logic == BranchLogic::Else || p->Logic == BranchLogic::ElseIf))
#ifndef VALIDATION
                     // Don't add JMP if last child is return
                     if (!HasExecutableChild() || !GetLastExecutableChild()->Is(CMD_RETURN))   
#endif
                        // JMP -> end of conditional branch
                        n->InsertJump(n->Children.end(), n->FindConditionalEnd());
                  break;

               // <nothing>
               case BranchLogic::Else:
                  break;    

               // JIF: next-std-sibling
               case BranchLogic::SkipIf:
                  n->JumpTarget = n->FindNextCommand();
                  break;

               // JIF: next-std-sibling
               case BranchLogic::While:
                  n->JumpTarget = n->FindNextCommand();
                  n->InsertJump(n->Children.end(), n); // JMP: WHILE (to create loop)
                  break;

               // JMP: WHILE->next-std-sibling
               case BranchLogic::Break:
                  n->JumpTarget = n->FindAncestor(BranchLogic::While)->FindNextCommand();
                  n->InsertJump(n->Children.begin(), n->JumpTarget);
                  break;

               // JMP: WHILE
               case BranchLogic::Continue:
                  n->JumpTarget = n->FindAncestor(BranchLogic::While);
                  n->InsertJump(n->Children.begin(), n->JumpTarget);
                  break;

               // JMP: LABEL
               case BranchLogic::None:
                  if (n->Is(CMD_GOTO_LABEL) || n->Is(CMD_GOTO_SUB))
                  {
                     n->JumpTarget = n->FindRoot()->FindLabel(n->Parameters[0].Value.String);  

                     if (!n->JumpTarget)     // Previously identified, should always be found
                        throw AlgorithmException(HERE, VString(L"Cannot find label %s", n->Parameters[0].Value.String.c_str()));
                  }
                  break;
               }
            }
            catch (ExceptionBase& e) {
               Errors += n->MakeError(GuiString(L"Linking failed: ") + e.Message); 
            }
         }
         
         
         // ------------------------------ PROTECTED METHODS -----------------------------

         // ------------------------------- PRIVATE METHODS ------------------------------
      }
   }
}
