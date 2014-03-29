#pragma once

namespace Logic
{
   namespace Scripts
   {
      class ScriptCommand;

      /// <summary>List of script commands</summary>
      class LogicExport CommandList : public list<ScriptCommand>
      {
         // --------------------- CONSTRUCTION ----------------------

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
			
		   // ---------------------- ACCESSORS ------------------------	
      public:
         /// <summary>Determines if 'define label' command is a label definition or subroutine</summary>
         /// <param name="cmd">The command.</param>
         /// <returns></returns>
         bool IsSubRoutine(iterator cmd) const;

         /// <summary>Find command by index.</summary>
         /// <param name="index">The index.</param>
         /// <returns></returns>
         /// <exception cref="Logic::IndexOutOfRangeException">Invalid index</exception>
         ScriptCommand& operator[](UINT index);

         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------
      };

      
      /// <summary>Script Command list iterator</summary>
      typedef CommandList::iterator  CommandIterator;

   }
}

using namespace Logic::Scripts;
