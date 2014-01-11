#pragma once
#include "Common.h"
#include "SyntaxFile.h"

namespace Logic
{
   namespace Scripts
   {
         
      /// <summary></summary>
      class SyntaxLibrary
      {
         // ------------------------ TYPES --------------------------
      private:
         

         // --------------------- CONSTRUCTION ----------------------

      public:
         SyntaxLibrary();
         virtual ~SyntaxLibrary();

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
			
		   // ---------------------- ACCESSORS ------------------------			

         static CommandSyntax  Find(UINT id, GameVersion ver) 
         { 
            // Search all syntax with matching ID for a compatible version
            for (auto it = Commands.find(id); it != Commands.end() && it->first == id; ++it)
               if (it->second.Versions & (UINT)ver)
                  return it->second;

            // Not found
            throw Win32Exception(HERE, L"Syntax not found");
         }

         static void           Merge(SyntaxFile&& f)          { Commands.Merge(std::move(f)); }

		   // ----------------------- MUTATORS ------------------------

		   // -------------------- REPRESENTATION ---------------------

      private:
         static SyntaxCollection  Commands;
      };

   }
}

using namespace Logic::Scripts;
