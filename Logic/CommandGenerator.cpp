#include "stdafx.h"
#include "CommandTree.h"
#include "ScriptFile.h"

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         // -------------------------------- CONSTRUCTION --------------------------------
         
         /// <summary>Create command generating visitor</summary>
         /// <param name="s">script</param>
         /// <param name="e">errors collection</param>
         CommandTree::CommandGenerator::CommandGenerator(ScriptFile& s, ErrorArray& e) : Errors(e), Script(s)
         {
         }

         /// <summary>Nothing</summary>
         CommandTree::CommandGenerator::~CommandGenerator()
         {
         }

         // ------------------------------- STATIC METHODS -------------------------------

         // ------------------------------- PUBLIC METHODS -------------------------------

         /// <summary>Generates a ScriptCommand from the node</summary>
         /// <param name="n">Node</param>
         void  CommandTree::CommandGenerator::VisitNode(CommandNode* n) 
         {
            try
            {
               ParameterArray params;

               // Compile parameters
               for (auto& p : n->Parameters)
               {
                  // Goto/Gosub: Change label number dataType from DT_STRING (ie. label name) into DT_INTEGER. 
                  if (p.Syntax.Type == ParameterType::LABEL_NUMBER && !n->CmdComment)
                     p.Type = DataType::INTEGER;   // parameter ctor resolves 'label' token type to DT_STRING

                  // Compile
                  p.Generate(Script, n->JumpTarget ? n->JumpTarget->Index : EMPTY_JUMP, n->CmdComment);
               }

               // Command: Provide compiled parameters in display order
               if (!n->Syntax.Is(CMD_EXPRESSION))
                  Script.Commands.AddOutput(ScriptCommand(n->LineText, n->Syntax, n->Parameters, n->CmdComment));
               else
               {
                  // Compile postfix parameters
                  for (auto& p : n->Postfix)
                     p.Generate(Script, n->JumpTarget ? n->JumpTarget->Index : EMPTY_JUMP, n->CmdComment);

                  // Expression: Provide compiled parameters in infix & postfix order
                  Script.Commands.AddOutput(ScriptCommand(n->LineText, n->Syntax, n->Parameters, n->Postfix, n->CmdComment));
               }
            }
            catch (ExceptionBase& e) {
               Errors += n->MakeError(GuiString(L"Compile failed: ") + e.Message); 
            }
         }

         // ------------------------------ PROTECTED METHODS -----------------------------

         // ------------------------------- PRIVATE METHODS ------------------------------
      }
   }
}
