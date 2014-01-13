#include "stdafx.h"
#include "ScriptLexer.h"

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         // -------------------------------- CONSTRUCTION --------------------------------

         ScriptLexer::ScriptLexer(const wstring& line) : Tokens(Parse(line))
         {
         }


         ScriptLexer::~ScriptLexer()
         {
         }

         // ------------------------------- STATIC METHODS -------------------------------

         list<ScriptToken> ScriptLexer::Parse(const wstring& line)
         {
            return list<ScriptToken>();
         }

         // ------------------------------- PUBLIC METHODS -------------------------------

         // ------------------------------ PROTECTED METHODS -----------------------------

         // ------------------------------- PRIVATE METHODS ------------------------------

      }
   }
}