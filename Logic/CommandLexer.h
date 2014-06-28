#pragma once


#include "ScriptToken.h"
#include <algorithm>

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         /// <summary>Lexes MSCI Script command text into a stream of tokens</summary>
         class LogicExport CommandLexer
         {
            // ------------------------ TYPES --------------------------
         protected:
            typedef wstring::const_iterator CharIterator;

            // --------------------- CONSTRUCTION ----------------------
         public:
            CommandLexer(const wstring& line, bool skipWhitespace = true);
            virtual ~CommandLexer();

            NO_COPY_ASSIGN(CommandLexer);	// Immutable
            NO_MOVE(CommandLexer);	      // Immutable

            // ------------------------ STATIC -------------------------
         protected:
            void  Parse();

            // --------------------- PROPERTIES ------------------------
         public:
            PROPERTY_GET(CHARRANGE,Extent,GetExtent);
         protected:
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
               return Tokens.count();
            }

            /// <summary>Finds a token by character index.</summary>
            /// <param name="index">Zero-based character index</param>
            /// <returns></returns>
            TokenIterator Find(UINT index) const
            {
               //Console << Cons::Yellow << L"Searching for token @ " << index << ENDL;
               return find_if(Tokens.begin(), Tokens.end(), [index](const ScriptToken& t) { 
                  //Console << Cons::Yellow << t.Text << ENDL;
                  return index >= t.Start && index <= t.End;   // Also match true for character position immediately following token
               });
            }

            /// <summary>Gets character indicies of the first and last token</summary>
            /// <returns></returns>
            CHARRANGE GetExtent() const
            {
               CHARRANGE range = {0, LineEnd-LineStart};

               if (Tokens.size() > 0)
               {
                  range.cpMin = Tokens[0].Start;
                  range.cpMax = Tokens[Tokens.size()-1].End;
               }
               return range;
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
            
            /// <summary>Matches a token type at the caret</summary>
            /// <param name="caret">Zero-based character index</param>
            /// <param name="t">The token type</param>
            /// <returns></returns>
            bool  MatchAtCaret(UINT caret, TokenType t) const
            {
               auto p = Find(caret);
               return Valid(p) && p->Type == t;
            }

            /// <summary>Validates the specified position.</summary>
            /// <param name="pos">The position.</param>
            /// <returns></returns>
            bool  Valid(const TokenIterator& pos) const
            {
               return pos < Tokens.end();
            }

         protected:
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

         protected:
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
         public:
            const wstring       Input;             // Input text
            const TokenArray&   Tokens;            // Output tokens
            const bool          SkipWhitespace;    // Whether skip whitespace option enabled

         protected:
            const CharIterator  LineStart,         // Input start position
                                LineEnd;           // Input end position
            CharIterator        Position;          // Position
            TokenArray          Output;            // Tokens parsed so far
         };
      }
   }
}

using namespace Logic::Scripts::Compiler;
