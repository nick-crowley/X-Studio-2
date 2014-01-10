#pragma once
#include "Common.h"
#include "CommandSyntax.h"

namespace Library
{
   namespace Scripts
   {
         
      /// <summary></summary>
      class SyntaxLibrary
      {
         // --------------------- CONSTRUCTION ----------------------

      public:
         SyntaxLibrary();
         virtual ~SyntaxLibrary();

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
			
		   // ---------------------- ACCESSORS ------------------------			

         static CommandSyntax  Find(UINT id, GameVersion ver) { return CommandSyntax(CommandSyntax::Declaration()); }

		   // ----------------------- MUTATORS ------------------------

		   // -------------------- REPRESENTATION ---------------------

      private:
      };

   }
}

using namespace Library::Scripts;
