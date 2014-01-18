#pragma once

#include "Common.h"

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         /// <summary>Types of tokens produced by the script lexer</summary>
         enum class TokenType { Text, Number, String, GameObject, ScriptObject, Keyword, Variable, Null, Label, Operator, Comment, Whitespace };

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

            /*bool  Match(TokenType t, const WCHAR* txt, bool matchCase = false) const
            {
               return Type == t && (!matchCase ? Text == txt : StrCmpI(Text.c_str(), txt) == 0);
            }*/

            // ----------------------- MUTATORS ------------------------

            // -------------------- REPRESENTATION ---------------------

         public:
            const TokenType   Type;
            const UINT        Start, End;
            const wstring     Text;

         private:
         };

         /// <summary>Vector of script tokens</summary>
         typedef vector<ScriptToken> TokenArray;

         /// <summary>Script token array iterator</summary>
         typedef TokenArray::const_iterator  TokenIterator;
      }
   }
}

using namespace Logic::Scripts::Compiler;
