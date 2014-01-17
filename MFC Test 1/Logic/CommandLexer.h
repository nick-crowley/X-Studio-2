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
         class CommandLexer
         {
            // ------------------------ TYPES --------------------------
         private:
            typedef wstring::const_iterator StringIterator;

            // --------------------- CONSTRUCTION ----------------------

         public:
            CommandLexer(const wstring& line);
            virtual ~CommandLexer();

            DEFAULT_COPY(CommandLexer);	// Default copy semantics
            DEFAULT_MOVE(CommandLexer);	// Default move semantics

            // ------------------------ STATIC -------------------------
         private:
            TokenArray Parse();

            // --------------------- PROPERTIES ------------------------

            PROPERTY_GET(bool,ValidPosition,IsValidPosition);

            // ---------------------- ACCESSORS ------------------------			

         public:
            /// <summary>Matches a token at the specified position</summary>
            /// <param name="pos">The position.</param>
            /// <param name="t">The token type</param>
            /// <returns></returns>
            bool  Match(const TokenIterator& pos, TokenType t) const
            {
               return pos < Tokens.end() && pos->Type == t;
            }

            /// <summary>Matches a token at the specified position (CASE INSENSITIVE)</summary>
            /// <param name="pos">The position.</param>
            /// <param name="t">The token type</param>
            /// <param name="txt">The token text (Case insensitive)</param>
            /// <returns></returns>
            bool  Match(const TokenIterator& pos, TokenType t, const WCHAR* txt) const
            {
               return Match(pos, t) && StrCmpI(pos->Text.c_str(), txt)==0;
            }

         private:
            bool  IsValidPosition() const  { return Position < LineEnd; }
            
            bool  MatchChar(WCHAR ch) const;
            bool  MatchChars(const WCHAR* str) const;

            bool  MatchConstant() const;
            bool  MatchNumber() const;
            bool  MatchOperator() const;
            bool  MatchText() const;
            bool  MatchVariable() const;
            bool  MatchWhitespace() const;

            ScriptToken  MakeToken(StringIterator start, TokenType t) const;

            // ----------------------- MUTATORS ------------------------

         private:
            bool         ReadChar();
            void         ReadWhitespace();

            ScriptToken  ReadAmbiguous(StringIterator start);
            ScriptToken  ReadConstant(StringIterator start);
            ScriptToken  ReadComment(StringIterator start);
            ScriptToken  ReadGameObject(StringIterator start);
            ScriptToken  ReadNumber(StringIterator start);
            ScriptToken  ReadOperator(StringIterator start);
            ScriptToken  ReadString(StringIterator start);
            ScriptToken  ReadText(StringIterator start);
            ScriptToken  ReadVariable(StringIterator start);

            // -------------------- REPRESENTATION ---------------------
         
         private:
            const StringIterator  LineStart,    // Do not re-order declarations, order is relied upon by ctor
                                  LineEnd;
            StringIterator        Position;

         public:
            const TokenArray      Tokens;
         };
      }
   }
}

using namespace Logic::Scripts::Compiler;
