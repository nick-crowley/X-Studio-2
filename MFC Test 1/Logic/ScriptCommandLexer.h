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
         class ScriptCommandLexer
         {
            // ------------------------ TYPES --------------------------
         private:
            typedef wstring::const_iterator StringIterator;

            // --------------------- CONSTRUCTION ----------------------

         public:
            ScriptCommandLexer(const wstring& line);
            virtual ~ScriptCommandLexer();

            DEFAULT_COPY(ScriptCommandLexer);	// Default copy semantics
            DEFAULT_MOVE(ScriptCommandLexer);	// Default move semantics

            // ------------------------ STATIC -------------------------
         private:
            TokenArray Parse();

            // --------------------- PROPERTIES ------------------------

            PROPERTY_GET(bool,ValidPosition,IsValidPosition);

            // ---------------------- ACCESSORS ------------------------			

         private:
            bool  IsValidPosition() const  { return Position < LineEnd; }
            
            bool  MatchChar(WCHAR ch) const;
            bool  MatchChars(const WCHAR* str) const;

            bool  MatchConstant() const;
            //bool  MatchGameObject() const;
            bool  MatchNumber() const;
            bool  MatchOperator() const;
            //bool  MatchString() const;
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
            const StringIterator  LineStart,    // Do not re-order, these declarations, relied upon by ctor
                                  LineEnd;
            StringIterator        Position;

         public:
            const TokenArray      Tokens;
         };
      }
   }
}

using namespace Logic::Scripts::Compiler;
