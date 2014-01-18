#include "stdafx.h"
#include "SyntaxLibrary.h"
#include "CommandLexer.h"

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
            // Tokenise syntax
            CommandSyntax& syntax = pair.second;
            CommandLexer lex(syntax.Text);
            
            // Add hash to hash collection 
            CommandHash  h(lex.Tokens.begin(), lex.Tokens.end());
            Hashes.insert(HashCollection::value_type(h.Hash, syntax));

            try
            {
               // Skip comments
               if (syntax.ID == CMD_COMMENT || syntax.ID == CMD_COMMAND_COMMENT)
                  continue;

               // NEW: Add to syntax tree
               list<ScriptToken> tokens;
               for (const ScriptToken& t : lex.Tokens)
               {
                  if (t.Type == TokenType::Variable)
                  {
                     auto p = syntax.Parameters[t.Text[1]-48];
                     if (p.IsRefObj() || p.IsRetVar())
                        continue;
                  }

                  tokens.push_back(t);
               }
               Tree.Add(syntax, tokens);
            }
            catch (ExceptionBase& e)
            {
               Console << Colour::Red << L"Unable to insert syntax: " << syntax.Text << L" : " << Colour::Yellow << e.Message << ENDL;
            }
         }

         // NEW:
         Tree.Print();
      }
		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

   }
}

