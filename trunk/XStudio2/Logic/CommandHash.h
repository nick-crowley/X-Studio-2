#pragma once

#include "Common.h"
#include "ScriptToken.h"

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         /// <summary>List of script tokens</summary>
         typedef list<ScriptToken>  TokenList;

         /// <summary></summary>
         class CommandHash 
         {
            // --------------------- CONSTRUCTION ----------------------
         public:
            CommandHash(TokenIterator b, TokenIterator e) : Hash(GenerateHash(b,e)), Parameters(SeparateParams(b,e))
            {}

            // ------------------------ STATIC -------------------------
         public:
            static wstring  GenerateHash(const TokenIterator begin, const TokenIterator end) 
            {
               wstring h;
               // Assemble text into hash
               for (TokenIterator it = begin; it != end; ++it)
                  switch (it->Type)
                  {
                  // Include text/operators/keywords
                  case TokenType::Text:
                  case TokenType::Operator:
                  case TokenType::Keyword:
                     h += it->Text;

                  // Exclude literals (during script parsing), and variables (during syntax parsing)
                  } 
               return h;
            }

            static TokenList  SeparateParams(const TokenIterator begin, const TokenIterator end) 
            {
               TokenList list;

               // Separate tokens
               for (TokenIterator it = begin; it != end; ++it)
                  switch (it->Type)
                  {
                  // Exclude text/operators/keywords
                  case TokenType::Text:
                  case TokenType::Operator:
                  case TokenType::Keyword:
                     break;

                  // Include literals
                  default: 
                     list.push_back(*it);
                  }  
                     
               return list;
            }

            // ---------------------- ACCESSORS ------------------------	

            bool  operator==(const CommandHash& r) { return Hash == r.Hash; }
            bool  operator!=(const CommandHash& r) { return Hash != r.Hash; }

            // ----------------------- MUTATORS ------------------------

            // -------------------- REPRESENTATION ---------------------
         public:
            const wstring    Hash;
            const TokenList  Parameters;
         };
      }
   }
}

using namespace Logic::Scripts::Compiler;
