#include "stdafx.h"
#include "SyntaxLibrary.h"
#include "ScriptCommandLexer.h"

namespace Logic
{
   namespace Scripts
   {
      SyntaxLibrary  SyntaxLibrary::Instance;

      // -------------------------------- CONSTRUCTION --------------------------------

      SyntaxLibrary::SyntaxLibrary()
      {
      }


      SyntaxLibrary::~SyntaxLibrary()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Finds syntax by ID</summary>
      /// <param name="id">command ID</param>
      /// <param name="ver">Game version</param>
      /// <returns></returns>
      /// <exception cref="Logic::SyntaxNotFoundException">Not found</exception>
      CommandSyntax  SyntaxLibrary::Find(UINT id, GameVersion ver) const
      { 
         // Search all syntax with matching ID for a compatible version
         for (auto it = Commands.find(id); it != Commands.end() && it->first == id; ++it)
            if (it->second.Versions & (UINT)ver)
               return it->second;

         // Not found
         throw SyntaxNotFoundException(HERE, id, ver);
      }

      /// <summary>Finds syntax by Hash</summary>
      /// <param name="h">command hash</param>
      /// <param name="v">Game version</param>
      /// <returns>Syntax if found, otherwise 'Unknown'</returns>
      CommandSyntax  SyntaxLibrary::Identify(const CommandHash& h, GameVersion v) const
      {
         // Search all syntax with matching hash for a compatible version
         for (auto it = Hashes.find(h.Hash); it != Hashes.end() && it->first == h.Hash; ++it)
            if (it->second.Versions & (UINT)v)
               return it->second;

         // Not found
         return Unknown;
      }

      /// <summary>Merges a syntax file into the library</summary>
      /// <param name="f">The file</param>
      void  SyntaxLibrary::Merge(SyntaxFile&& f)
      { 
         // Merge commands
         Commands.Merge(std::move(f)); 

         // add hashes
         for (auto& pair : Commands)
         {
            // Hash command syntax
            ScriptCommandLexer lex(pair.second.Text);
            CommandHash        h(lex.Tokens.begin(), lex.Tokens.end());
            
            // Insert {Hash,Syntax}
            Hashes.insert(HashCollection::value_type(h.Hash, pair.second));
         }
      }
		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

   }
}

