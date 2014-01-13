#pragma once

#include "Common.h"
#include "ScriptToken.h"

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         /// <summary></summary>
         class ScriptLexer
         {
            // ------------------------ TYPES --------------------------
         private:

            // --------------------- CONSTRUCTION ----------------------

         public:
            ScriptLexer(const wstring& line);
            virtual ~ScriptLexer();

            DEFAULT_COPY(ScriptLexer);	// Default copy semantics
            DEFAULT_MOVE(ScriptLexer);	// Default move semantics

            // ------------------------ STATIC -------------------------
         private:
            static list<ScriptToken> Parse(const wstring& line);

            // --------------------- PROPERTIES ------------------------

            // ---------------------- ACCESSORS ------------------------			

            // ----------------------- MUTATORS ------------------------

            // -------------------- REPRESENTATION ---------------------

         public:
            const list<ScriptToken>  Tokens;

         private:
         };
      }
   }
}

using namespace Logic::Scripts::Compiler;
