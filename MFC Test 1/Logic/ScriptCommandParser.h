#pragma once

#include "Common.h"
#include "ScriptCommandLexer.h"
#include "ScriptCommand.h"

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         typedef TokenArray::const_iterator  TokenIterator;

         enum class Operator : UINT { Equals, NotEquals, Multiply, Divide, Add, Subtract };

         /// <summary></summary>
         class ScriptCommandParser
         {
            class CommandHash 
            {
               // --------------------- CONSTRUCTION ----------------------
            public:
               CommandHash(TokenIterator b, TokenIterator e) : Hash(GenerateHash(b,e)), Parameters(SeparateParams(b,e))
               {}

               // ------------------------ STATIC -------------------------
            private:
               static wstring  GenerateHash(TokenIterator begin, TokenIterator end) 
               {
                  wstring h;
                  // Assemble text into hash
                  for (TokenIterator it = begin; it != end; ++it)
                     if (it->Type == TokenType::Text)
                        h += it->Text;
                  return h;
               }

               static TokenArray  SeparateParams(TokenIterator begin, TokenIterator end) 
               {
                  TokenArray arr;
                  // Separate tokens
                  for (TokenIterator it = begin; it != end; ++it)
                     if (it->Type != TokenType::Text)
                        arr.push_back(*it);
                  return arr;
               }

               // ---------------------- ACCESSORS ------------------------	

               bool  operator==(const CommandHash& r) { return Hash == r.Hash; }
               bool  operator!=(const CommandHash& r) { return Hash != r.Hash; }

               // ----------------------- MUTATORS ------------------------

               // -------------------- REPRESENTATION ---------------------
            public:
               const wstring     Hash;
               const TokenArray  Parameters;
            };

            // ------------------------ TYPES --------------------------
         private:

            // --------------------- CONSTRUCTION ----------------------

         public:
            ScriptCommandParser(const wstring& line, GameVersion  v);
            virtual ~ScriptCommandParser();

            DEFAULT_COPY(ScriptCommandParser);	// Default copy semantics
            DEFAULT_MOVE(ScriptCommandParser);	// Default move semantics

            // ------------------------ STATIC -------------------------

            // --------------------- PROPERTIES ------------------------

            // ---------------------- ACCESSORS ------------------------			

            // ----------------------- MUTATORS ------------------------
         public:
            ScriptCommand  Parse();

         private:
            ScriptCommand  MatchLine();

            bool  MatchAssignment(TokenIterator& pos, wstring& var);
            bool  MatchConditional(TokenIterator& pos, Conditional& c);
            bool  MatchCommand(TokenIterator& pos, UINT& id, TokenArray& params);
            bool  MatchExpression(TokenIterator& pos, TokenArray& params);
            bool  MatchNOP(TokenIterator& pos);

            bool  Match(const TokenIterator& pos, TokenType  type);
            bool  Match(const TokenIterator& pos, TokenType  type, const TCHAR* txt);

            ScriptCommand GenerateCommand(UINT id, TokenArray& params);
            ScriptCommand GenerateCommand(UINT id, const wstring& retVar, TokenArray& params);
            ScriptCommand GenerateCommand(UINT id, Conditional c, TokenArray& params);

            // -------------------- REPRESENTATION ---------------------

         private:
            ScriptCommandLexer  Lexer;
            GameVersion         Version;
         };
      }
   }
}

using namespace Logic::Scripts::Compiler;
