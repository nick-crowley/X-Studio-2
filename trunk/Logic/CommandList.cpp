#include "stdafx.h"
#include "CommandList.h"
#include "ScriptCommand.h"

namespace Logic
{
   namespace Scripts
   {
   
      // -------------------------------- CONSTRUCTION --------------------------------

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Determines if 'define label' command is a label definition or subroutine</summary>
      /// <param name="cmd">The command.</param>
      /// <returns></returns>
      bool  CommandList::IsSubRoutine(iterator cmd) const
      {
         if (!cmd->Is(CMD_DEFINE_LABEL))
            return false;

         // Linear search following commands
         for (auto p = ++cmd; p != end(); ++p)
            // Label: Not a subroutine
            if (p->Is(CMD_DEFINE_LABEL))
               return false;
            // EndSub: Subroutine
            else if (p->Is(CMD_END_SUB))
               return true;

         // EOF: Not a subroutine
         return false;
      }

      /// <summary>Find command by index.</summary>
      /// <param name="index">The index.</param>
      /// <returns></returns>
      /// <exception cref="Logic::IndexOutOfRangeException">Invalid index</exception>
      ScriptCommand&  CommandList::operator[](UINT index)
      {
         UINT i = 0;

         // Linear search
         for (auto pos = begin(); pos != end(); ++pos)
            if (index == i++)
               return *pos;

         // Invalid index
         throw IndexOutOfRangeException(HERE, index, size());
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}

