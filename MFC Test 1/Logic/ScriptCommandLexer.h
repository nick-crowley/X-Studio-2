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
         class ScriptCommandLexer
         {
            // ------------------------ TYPES --------------------------
         private:

            // --------------------- CONSTRUCTION ----------------------

         public:
            ScriptCommandLexer(const wstring& line);
            virtual ~ScriptCommandLexer();

            DEFAULT_COPY(ScriptCommandLexer);	// Default copy semantics
            DEFAULT_MOVE(ScriptCommandLexer);	// Default move semantics

            // ------------------------ STATIC -------------------------
         private:
            static TokenList Parse(const wstring& line);

            // --------------------- PROPERTIES ------------------------

            // ---------------------- ACCESSORS ------------------------			

            // ----------------------- MUTATORS ------------------------

            // -------------------- REPRESENTATION ---------------------

         public:
            const TokenList  Tokens;

         private:
         };
      }
   }
}

using namespace Logic::Scripts::Compiler;
