#pragma once
#include "CommandTree.h"

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         /// <summary>List of script commands nodes</summary>
         class LogicExport CommandNodeList : public list<CommandNodePtr>
         {
            // --------------------- CONSTRUCTION ----------------------

            // ------------------------ STATIC -------------------------

            // --------------------- PROPERTIES ------------------------
			
		      // ---------------------- ACCESSORS ------------------------	
         public:
            /// <summary>Determines if 'define label' command is a label definition or subroutine</summary>
            /// <param name="cmd">The command.</param>
            /// <returns></returns>
            bool IsSubRoutine(iterator cmd) const
            {
               if (!(*cmd)->Is(CMD_DEFINE_LABEL))
                  return false;

               // Linear search following commands
               for (auto p = ++cmd; p != end(); ++p)
                  // Label: Not a subroutine
                  if ((*p)->Is(CMD_DEFINE_LABEL))
                     return false;

                  // EndSub: Subroutine
                  else if ((*p)->Is(CMD_END_SUB))
                     return true;

               // EOF: Not a subroutine
               return false;
            }

            // ----------------------- MUTATORS ------------------------

            // -------------------- REPRESENTATION ---------------------
         };
      }
   }
}

using namespace Logic::Scripts::Compiler;

