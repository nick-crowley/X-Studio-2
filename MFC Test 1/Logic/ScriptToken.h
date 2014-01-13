#pragma once

#include "Common.h"

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         /// <summary>Types of tokens produced by the script lexer</summary>
         enum class TokenType { Text, Number, String, Operator, Keyword, Variable, Constant, GameObject, ScriptObject };

         /// <summary></summary>
         class ScriptToken
         {
            // ------------------------ TYPES --------------------------
         private:

            // --------------------- CONSTRUCTION ----------------------

         public:
            ScriptToken(TokenType t, UINT s, UINT e, const wstring& txt) : Type(t), Start(s), End(e), Text(txt)
            {}
            virtual ~ScriptToken()
            {}

            DEFAULT_COPY(ScriptToken);	// Default copy semantics
            DEFAULT_MOVE(ScriptToken);	// Default move semantics

            // ------------------------ STATIC -------------------------

            // --------------------- PROPERTIES ------------------------

            // ---------------------- ACCESSORS ------------------------			

            // ----------------------- MUTATORS ------------------------

            // -------------------- REPRESENTATION ---------------------

         public:
            const TokenType   Type;
            const UINT        Start, End;
            const wstring     Text;

         private:
         };
      }
   }
}

using namespace Logic::Scripts::Compiler;
