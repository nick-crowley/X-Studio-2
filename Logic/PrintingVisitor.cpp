#include "stdafx.h"
#include "CommandTree.h"

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         // -------------------------------- CONSTRUCTION --------------------------------

         // ------------------------------- STATIC METHODS -------------------------------

         // ------------------------------- PUBLIC METHODS -------------------------------

         /// <summary>Print tree to the console</summary>
         /// <param name="n">Node</param>
         void  CommandTree::PrintingVisitor::VisitNode(CommandTree* n) 
         {
            // Line#/Logic/Text
            VString   line(!n->Is(CMD_HIDDEN_JUMP) ? L"%03d: " : L"---: ", n->LineNumber), 
                      logic(::GetString(n->Logic));
            GuiString txt(n->LineText);
            Cons      colour(Cons::White);
            
            // Index
            line += VString(n->Is(CommandType::Standard) && n->Index != EMPTY_JUMP ? L"%03d: " : L"---: ", n->Index);
            
            // Logic
            switch (n->Logic)
            {
            // Conditional:
            default: 
               colour = Cons::Cyan;
               if (n->JumpTarget)
                  txt = (n->JumpTarget->Index ? VString(L"Jump-if-false: %d", n->JumpTarget->Index) 
                                              : VString(L"<Invalid JumpTarget> : ") + n->JumpTarget->LineCode);
               break;

            // NOP:
            case BranchLogic::NOP:
               logic = (!n->CmdComment ? L"NOP" : L"Cmd");
               colour = Cons::Yellow;
               break;

            // Command:
            case BranchLogic::None:
               if (n->Is(CMD_HIDDEN_JUMP) || n->Is(CMD_GOTO_LABEL) || n->Is(CMD_GOTO_SUB))
               {
                  colour = Cons::Green; 
                  logic = n->Is(CMD_HIDDEN_JUMP) ? L"Jmp" : L"Goto";
                  txt = L"Unconditional: ";

                  // Post-Compile: Display label number 
                  if (n->JumpTarget)
                     txt += VString(L"%d", n->JumpTarget->Index);
                  // Pre-Compile: Label name
                  else 
                     txt += (!n->Parameters.empty() ? n->Parameters[0].Token.Text : L"<missing>");
               }
               else if (n->Is(CMD_DEFINE_LABEL))
               {
                  colour = Cons::Purple;
                  logic = L"Proc";
               }
               else if (n->Is(CMD_RETURN))
               {
                  colour = Cons::Cyan;
                  logic = L"Ret";
               }
               else if (n->Syntax == CommandSyntax::Unrecognised)
               {  // Print entire line in red
                  Console << (colour = Cons::Red);
                  logic = L"???";
               }
               else
                  logic = L"Cmd";
               break;
            }

            // Print
            Console << line+Indent(depth) << colour << logic << Cons::White << L" : " << colour << txt.TrimLeft(L" ") << ENDL;
         }

         /// <summary>Perform command linking</summary>
         /// <param name="n">Node</param>
         /// <exception cref="Logic::AlgorithmException">Error in linking algorithm</exception>
         void  CommandTree::PrintingVisitor::VisitRoot(CommandTree* n) 
         {
            Console << ENDL << "Ln  Index  Logic            Text        " << Cons::Purple << Cons::Bold << GetString(n->State);
            Console << ENDL << "-------------------------------------------------------" << ENDL; 

            // Spacing
            Console << ENDL;
         }

         // ------------------------------ PROTECTED METHODS -----------------------------

         // ------------------------------- PRIVATE METHODS ------------------------------
      }
   }
}
