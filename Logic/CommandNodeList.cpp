#include "stdafx.h"
#include "CommandNodeList.h"
#include "CommandNode.h"

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
         {
         // -------------------------------- CONSTRUCTION --------------------------------

         // ------------------------------- STATIC METHODS -------------------------------

         // ------------------------------- PUBLIC METHODS -------------------------------

         /// <summary>Determines if 'define label' command is a label definition or subroutine</summary>
         /// <param name="cmd">The command.</param>
         /// <returns></returns>
         bool  CommandNodeList::IsSubRoutine(iterator cmd) const
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

         /// <summary>Appends a node to the list</summary>
         /// <param name="p">node.</param>
         /// <returns></returns>
         CommandNodeList&  CommandNodeList::operator+=(const CommandNodePtr& p)
         {
            push_back(p);
            return *this;
         }

         // ------------------------------ PROTECTED METHODS -----------------------------

         // ------------------------------- PRIVATE METHODS ------------------------------
      }
   }
}

