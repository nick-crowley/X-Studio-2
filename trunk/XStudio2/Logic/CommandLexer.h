#pragma once

#include "Common.h"
#include "ScriptToken.h"
#include <algorithm>

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
            /// <summary>Get start token iterator</summary>
            /// <returns></returns>
            TokenIterator begin() const 
            { 
               return Tokens.begin(); 
            }

            /// <summary>Get end token iterator</summary>
            /// <returns></returns>
            TokenIterator end() const 
            { 
               return Tokens.end(); 
            }

            /// <summary>Get number of non-whitespace tokens</summary>
            /// <returns></returns>
            TokenArray::size_type count() const
            {
               return 0; // return Tokens.count();
            }

            /// <summary>Finds a token by character index.</summary>
            /// <param name="index">character index</param>
            /// <returns></returns>
            TokenIterator Find(UINT index) const
            {
               //Console << Colour::Yellow << L"Searching for token @ " << index << ENDL;
               return find_if(Tokens.begin(), Tokens.end(), [index](const ScriptToken& t) { 
                  //Console << Colour::Yellow << t.Text << ENDL;
                  return index >= t.Start && index <= t.End;   // Also match true for character position immediately following token
               });
            }

            /// <summary>Validates the specified position.</summary>
            /// <param name="pos">The position.</param>
            /// <returns></returns>
            bool  Valid(const TokenIterator& pos) const
            {
               return pos < Tokens.end();
            }

            /// <summary>Matches a token at the specified position</summary>
            /// <param name="pos">The position.</param>
            /// <param name="t">The token type</param>
            /// <returns></returns>
            bool  Match(TokenIterator& pos, TokenType t) const
            {
               return pos < Tokens.end() && pos->Type == t ? (++pos, true) : false;
            }

            /// <summary>Matches a token at the specified position (CASE INSENSITIVE)</summary>
            /// <param name="pos">The position.</param>
            /// <param name="t">The token type</param>
            /// <param name="txt">The token text (Case insensitive)</param>
            /// <returns></returns>
            bool  Match(TokenIterator& pos, TokenType t, const WCHAR* txt) const
            {
               return pos < Tokens.end() && pos->Type == t && StrCmpI(pos->Text.c_str(), txt)==0 ? (++pos, true) : false;
            }

            /// <summary>Determines if a character index immediately follows a token, ie. immediately follows</summary>
            /// <param name="pos">The position.</param>
            /// <param name="index">The index.</param>
            /// <returns>true if index represents EOF, next whitespace or first char of next token.</returns>
            /*bool Trails(const TokenIterator pos, UINT index) const
            {
               return Valid(pos) && pos->End <= index   
                   && (!Valid(pos+1) || index <= (pos+1)->Start);
            }*/

         private:
            bool  IsValidPosition() const  { return Position < LineEnd; }
            
            bool  MatchChar(WCHAR ch) const;
            bool  MatchChars(const WCHAR* str) const;
            bool  MatchChars(const CharIterator pos, const WCHAR* str) const;

            bool  MatchConstant() const;
            bool  MatchDigit() const;
            bool  MatchNumber() const;
            bool  MatchOperator() const;
            bool  MatchText() const;
            bool  MatchVariable() const;
            bool  MatchWhitespace() const;

            ScriptToken  MakeToken(CharIterator start, TokenType t) const;

            // ----------------------- MUTATORS ------------------------

         private:
            bool         ReadChar();

            ScriptToken  ReadAmbiguous(CharIterator start, const TokenArray& prev);
            ScriptToken  ReadConstant(CharIterator start);
            ScriptToken  ReadComment(CharIterator start);
            ScriptToken  ReadGameObject(CharIterator start);
            ScriptToken  ReadNumber(CharIterator start);
            ScriptToken  ReadOperator(CharIterator start);
            ScriptToken  ReadString(CharIterator start);
            ScriptToken  ReadText(CharIterator start, const TokenArray& prev);
            ScriptToken  ReadVariable(CharIterator start);
            ScriptToken  ReadWhitespace(CharIterator start);

            // -------------------- REPRESENTATION ---------------------
         
         private:
            const CharIterator  LineStart,    // Do not re-order declarations, order is relied upon by ctor
                                LineEnd;
            CharIterator        Position;
            const bool          SkipWhitespace;

         public:
            const wstring       Input;
            const TokenArray    Tokens;
         };
      }
   }
}

using namespace Logic::Scripts::Compiler;
