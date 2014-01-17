#pragma once
#include "Common.h"
#include "SyntaxFile.h"
#include "CommandHash.h"

namespace Logic
{
   namespace Scripts
   {
         
      /// <summary></summary>
      class SyntaxLibrary
      {
         // ------------------------ TYPES --------------------------

         // --------------------- CONSTRUCTION ----------------------

      private:
         SyntaxLibrary();
      public:
         virtual ~SyntaxLibrary();

         // ------------------------ STATIC -------------------------

      public:
         static SyntaxLibrary  Instance;

         // --------------------- PROPERTIES ------------------------
			
		   // ---------------------- ACCESSORS ------------------------			

      public:
         /// <summary>Finds syntax by ID</summary>
         /// <param name="id">command ID</param>
         /// <param name="ver">Game version</param>
         /// <returns></returns>
         /// <exception cref="Logic::SyntaxNotFoundException">Not found</exception>
         CommandSyntax  Find(UINT id, GameVersion ver)
         { 
            // Search all syntax with matching ID for a compatible version
            for (auto it = Commands.find(id); it != Commands.end() && it->first == id; ++it)
               if (it->second.Versions & (UINT)ver)
                  return it->second;

            // Not found
            throw SyntaxNotFoundException(HERE, id, ver);
         }

         /// <summary>Merges a syntax file into the library</summary>
         /// <param name="f">The file</param>
         void  Merge(SyntaxFile&& f)
         { 
            Commands.Merge(std::move(f)); 
         }

		   // ----------------------- MUTATORS ------------------------

		   // -------------------- REPRESENTATION ---------------------

      private:
         SyntaxCollection  Commands;
      };

   }
}

// Syntax library singleton
#define SyntaxLib SyntaxLibrary::Instance

using namespace Logic::Scripts;



