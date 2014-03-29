#pragma once

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         class CommandNodePtr;

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
            bool IsSubRoutine(iterator cmd) const;

            // ----------------------- MUTATORS ------------------------
         public:
            /// <summary>Appends a node to the list</summary>
            /// <param name="p">node.</param>
            /// <returns></returns>
            CommandNodeList& operator+=(const CommandNodePtr& p);

            // -------------------- REPRESENTATION ---------------------
         };
      }
   }
}

using namespace Logic::Scripts::Compiler;

