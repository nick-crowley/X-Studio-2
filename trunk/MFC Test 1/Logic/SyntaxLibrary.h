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

         typedef multimap<wstring,CommandSyntax>  HashCollection;

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
         CommandSyntax  Find(UINT id, GameVersion ver) const;

         /// <summary>Finds syntax by Hash</summary>
         /// <param name="h">command hash</param>
         /// <param name="ver">Game version</param>
         /// <returns>Syntax if found, otherwise 'Unknown'</returns>
         CommandSyntax  Identify(const CommandHash& h, GameVersion v) const;

         /// <summary>Merges a syntax file into the library</summary>
         /// <param name="f">The file</param>
         void  Merge(SyntaxFile&& f);

		   // ----------------------- MUTATORS ------------------------

		   // -------------------- REPRESENTATION ---------------------

      public:
         const CommandSyntax  Unknown;

      private:
         SyntaxCollection  Commands;
         HashCollection    Hashes;
      };

   }
}

// Syntax library singleton
#define SyntaxLib SyntaxLibrary::Instance

using namespace Logic::Scripts;



