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

         /// <summary>Get token type string</summary>
         GuiString  GetString(TokenType t);

         /// <summary>Common base class for all token types</summary>
         class TokenBase
         {
            // --------------------- CONSTRUCTION ----------------------
         public:
            TokenBase() : Start(0), End(0)
            {}
            TokenBase(UINT s, UINT e) : Start(s), End(e)
            {}
            virtual ~TokenBase()
            {}

            DEFAULT_COPY(TokenBase);	// Default copy semantics
            DEFAULT_MOVE(TokenBase);	// Default move semantics
            
            // ---------------------- ACCESSORS ------------------------			
         public:
            bool  Contains(UINT pos) const  { return pos >= Start && pos < End; }
            UINT  Length() const            { return End - Start; }

            // -------------------- REPRESENTATION ---------------------
         public:
            const UINT  Start, 
                        End;
         };

         /// <summary></summary>
         class ScriptToken : public TokenBase
         {
            // ------------------------ TYPES --------------------------
         private:

            // --------------------- CONSTRUCTION ----------------------

         public:
            ScriptToken() : Type(TokenType::Whitespace)
            {}
            ScriptToken(TokenType t, UINT s, UINT e, const wstring& txt) : TokenBase(s, e), Type(t), Text(txt)
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
            const wstring     Text;
         };

         /// <summary>List of script tokens</summary>
         typedef list<ScriptToken> TokenList;

         /// <summary>Vector of script tokens</summary>
         typedef vector<ScriptToken> TokenArray;

         /// <summary>Script token array iterator</summary>
         typedef TokenArray::const_iterator  TokenIterator;
      }
   }
}

using namespace Logic::Scripts::Compiler;
