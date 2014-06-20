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
         CommandTree::FinalizingVisitor::FinalizingVisitor(ErrorArray& e) : Errors(e)
         {
         }

         /// <summary>Create standard command indexer visitor</summary>
         /// <param name="init">Initial index</param>
         CommandTree::IndexingVisitor::IndexingVisitor(UINT& init) : NextIndex(init)
         {
         }
         
         /// <summary>Create linking visitor</summary>
         /// <param name="e">errors collection</param>
         CommandTree::LinkingVisitor::LinkingVisitor(ErrorArray& e) : Errors(e)
         {
         }
         
         /// <summary>Nothing</summary>
         CommandTree::FinalizingVisitor::~FinalizingVisitor()
         {
         }
         
         /// <summary>Nothing</summary>
         CommandTree::IndexingVisitor::~IndexingVisitor()
         {
         }

         /// <summary>Nothing</summary>
         CommandTree::LinkingVisitor::~LinkingVisitor()
         {
         }

         // ------------------------------- STATIC METHODS -------------------------------

         // ------------------------------- PUBLIC METHODS -------------------------------
         
         /// <summary>Finalizes linkage between nodes</summary>
         /// <param name="n">Node</param>
         void  CommandTree::FinalizingVisitor::VisitNode(CommandTree* n) 
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
         
         /// <summary>Assigns standard command indicies to nodes</summary>
         /// <param name="n">Node</param>
         void  CommandTree::IndexingVisitor::VisitNode(CommandTree* n) 
         {
            // Standard command  
            if (!n->CmdComment && n->Is(CommandType::Standard))
               n->Index = NextIndex++;
         }

         /// <summary>Perform command linking</summary>
         /// <param name="n">Node</param>
         /// <exception cref="Logic::AlgorithmException">Error in linking algorithm</exception>
         void  CommandTree::LinkingVisitor::VisitNode(CommandTree* n) 
         {
            try
            {
               CommandTree* p;

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
