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
         
         /// <summary>Create termination verifying visitor</summary>
         /// <param name="e">errors collection</param>
         CommandTree::TerminationVerifier::TerminationVerifier(ErrorArray& e) : Errors(e)
         {
         }

         /// <summary>Nothing</summary>
         CommandTree::TerminationVerifier::~TerminationVerifier()
         {
         }

         // ------------------------------- STATIC METHODS -------------------------------

         // ------------------------------- PUBLIC METHODS -------------------------------

         /// <summary>Verifies that all control paths lead to termination</summary>
         /// <param name="n">Node</param>
         void  CommandTree::TerminationVerifier::VisitNode(CommandNode* n) 
         {
            // Check for presence of possible conditional or return cmd
            if (!n->HasExecutableChild())
               Errors += n->MakeError(L"Not all control paths return a value");
            else
            {
               bool allPaths = false;

               // Find last child (exclude NOP/END)
               auto last = *find_if(n->Children.rbegin(), n->Children.rend(), CommandNode::isConditionalAlternate);
               switch (last->Logic)
               {
               // If: Verify branch and any alternates all lead to RETURN
               case BranchLogic::If:
                  VisitNode(last.get());

                  // Verify following Else/ElseIf
                  for (auto m = last->FindNextSibling(); m != nullptr && (m->Logic == BranchLogic::Else || m->Logic == BranchLogic::ElseIf); m = m->FindPrevSibling())
                  {
                     // Cannot guarantee termination without 'Else'
                     if (m->Logic == BranchLogic::Else)
                        allPaths = true;

                     // Verify branch
                     VisitNode(m);
                  }

                  // Ensure 'Else' was present
                  if (!allPaths)
                     Errors += last->MakeError(L"Not all control paths return a value");
                  break;

               // ElseIf: Find + Verify preceeding 'IF'
               case BranchLogic::Else:
               case BranchLogic::ElseIf:
                  // Simpler if we just verify the preceeding 'If'
                  for (auto m = last->FindPrevSibling(); ; m = m->FindPrevSibling())
                     if (m->Logic == BranchLogic::If)
                        return VisitNode(m);
                  break;
                  
               // Command: Verify RETURN
               case BranchLogic::While:      // While is unacceptable because we can't evaluate conditions under which loop will end
               case BranchLogic::SkipIf:     // SkipIf is unacceptable by default because JIF leads to no alternate branch
               case BranchLogic::Break:      
               case BranchLogic::Continue:   // Break/Continue are unacceptable because they're not RETURN
               case BranchLogic::None:
                  if (!last->Is(CMD_RETURN))
                     Errors += last->MakeError(L"Last command in script must be 'return'");
                  break;

               default:
                  throw AlgorithmException(HERE, VString(L"Unexpected branching logic '%s' for an executable child", ::GetString(last->Logic)));
               }
            }
         }

         // ------------------------------ PROTECTED METHODS -----------------------------


         // ------------------------------- PRIVATE METHODS ------------------------------
      }
   }
}
