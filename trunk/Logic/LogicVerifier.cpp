#include "stdafx.h"
#include "CommandTree.h"
#include "ScriptFile.h"
#include "GameObjectLibrary.h"
#include "ScriptObjectLibrary.h"
#include "PreferencesLibrary.h"

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         // -------------------------------- CONSTRUCTION --------------------------------
         
         /// <summary>Create logic verifying visitor</summary>
         /// <param name="e">errors collection</param>
         LogicVerifier::LogicVerifier(ErrorArray& e) : Errors(e)
         {
         }

         /// <summary>Nothing</summary>
         LogicVerifier::~LogicVerifier()
         {
         }

         // ------------------------------- STATIC METHODS -------------------------------

         // ------------------------------- PUBLIC METHODS -------------------------------

         /// <summary>Verifies the branching logic</summary>
         /// <param name="n">Node</param>
         void  CommandVerifier::VisitNode(CommandNode* n) 
         {
            CommandNode* p;
            switch (n->Logic)
            {
            // IF: Must preceed ELSE-IF/ELSE/END
            case BranchLogic::If:
               // EOF?
               if ((p=n->FindNextSibling()) == nullptr)
                  Errors += n->MakeError(L"missing 'end' command");
               // preceeds End/Else/Else-if?
               else if (p->Logic != BranchLogic::End && p->Logic != BranchLogic::Else && p->Logic != BranchLogic::ElseIf)
                  Errors += p->MakeError(L"expected 'else', 'else if' or 'end'");
               break;

            // WHILE: Must preceed END
            case BranchLogic::While:
               // EOF?
               if ((p=n->FindNextSibling()) == nullptr)
                  Errors += n->MakeError(L"missing 'end' command");
               // preceed END?
               else if (p->Logic != BranchLogic::End)
                  Errors += p->MakeError(L"expected 'end'");
               break;

            // ELSE: Must follow IF/ELSE-IF.  Must preceed END
            case BranchLogic::Else:
               // follow IF/ELSE-IF?
               if ((p=n->FindPrevSibling()) == nullptr || (p->Logic != BranchLogic::If && p->Logic != BranchLogic::ElseIf))
                  Errors += n->MakeError(L"unexpected 'else'");

               // EOF?
               else if ((p=n->FindNextSibling()) == nullptr)
                  Errors += n->MakeError(L"missing 'end' command");
               // preceed END?
               else if (p->Logic != BranchLogic::End)
                  Errors += p->MakeError(L"expected 'end'");
               break;

            // ELSE-IF: Must follow IF/ELSE-IF. Must preceed ELSE-IF/ELSE/END
            case BranchLogic::ElseIf:
               // follow IF/ELSE-IF?
               if ((p=n->FindPrevSibling()) == nullptr || (p->Logic != BranchLogic::If && p->Logic != BranchLogic::ElseIf))
                  Errors += n->MakeError(L"unexpected 'else-if'");
               
               // EOF?
               else if ((p=n->FindNextSibling()) == nullptr)
                  Errors += n->MakeError(L"missing 'end' command");
               // preceed ELSE-IF/ELSE/END?
               else if (p->Logic != BranchLogic::Else && p->Logic != BranchLogic::ElseIf && p->Logic != BranchLogic::End)
                  Errors += p->MakeError(L"expected 'else', 'else if' or 'end'");
               break;

            // END: Must follow IF/WHILE/ELSE-IF/ELSE
            case BranchLogic::End:
               // follow IF/WHILE/ELSE-IF/ELSE?
               if ((p=n->FindPrevSibling()) == nullptr || (p->Logic != BranchLogic::If && p->Logic != BranchLogic::While && p->Logic != BranchLogic::ElseIf && p->Logic != BranchLogic::Else))
                  Errors += n->MakeError(L"unexpected 'end' command");
               break;

            // SKIP-IF: Must not be child of SKIP-IF. Must contain 1 standard command
            case BranchLogic::SkipIf:
               // not parent SKIP-IF?
               if (n->Parent->Logic == BranchLogic::SkipIf)
                  Errors += n->MakeError(L"'skip-if' cannot be nested");

               // Ensure command present
               //if (!cmd->Is(CommandType::Standard) && !cmd->Is(CMD_CONTINUE) && !cmd->Is(CMD_BREAK))
               if (count_if(n->Children.begin(), n->Children.end(), CommandNode::isSkipIfCompatible) != 1)
                  Errors += n->MakeError(L"must contain single command without conditional");
               break;

            // BREAK/CONTINUE: Must be decendant of WHILE
            case BranchLogic::Break:
            case BranchLogic::Continue:
               // Check for a parent 'while' command
               if (!n->FindAncestor(BranchLogic::While))
                  Errors += n->MakeError(L"break/continue cannot appear outside 'while'");
               break;
            }
         }

         // ------------------------------ PROTECTED METHODS -----------------------------


         // ------------------------------- PRIVATE METHODS ------------------------------
      }
   }
}
