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
            typedef wstring::const_iterator CharIterator;

            // --------------------- CONSTRUCTION ----------------------

         public:
            CommandLexer(const wstring& line, bool skipWhitespace = true);
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
            bool  MatchChars(const CharIterator pos, const WCHAR* str) const;

            bool  MatchConstant() const;
            bool  MatchNumber() const;
            bool  MatchOperator() const;
            bool  MatchText() const;
            bool  MatchVariable() const;
            bool  MatchWhitespace() const;

            ScriptToken  MakeToken(CharIterator start, TokenType t) const;

            // ----------------------- MUTATORS ------------------------

         private:
            bool         ReadChar();

            ScriptToken  ReadAmbiguous(CharIterator start);
            ScriptToken  ReadConstant(CharIterator start);
            ScriptToken  ReadComment(CharIterator start);
            ScriptToken  ReadGameObject(CharIterator start);
            ScriptToken  ReadNumber(CharIterator start);
            ScriptToken  ReadOperator(CharIterator start);
            ScriptToken  ReadString(CharIterator start);
            ScriptToken  ReadText(CharIterator start);
            ScriptToken  ReadVariable(CharIterator start);
            ScriptToken  ReadWhitespace(CharIterator start);

            // -------------------- REPRESENTATION ---------------------
         
         private:
            const CharIterator  LineStart,    // Do not re-order declarations, order is relied upon by ctor
                                  LineEnd;
            CharIterator        Position;
            const bool            SkipWhitespace;

         public:
            const TokenArray      Tokens;
         };
      }
   }
}

using namespace Logic::Scripts::Compiler;
