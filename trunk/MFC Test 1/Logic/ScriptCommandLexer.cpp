#include "stdafx.h"
#include "ScriptCommandLexer.h"

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         // -------------------------------- CONSTRUCTION --------------------------------

         ScriptCommandLexer::ScriptCommandLexer(const wstring& line) : Tokens(Parse(line))
         {
         }


         ScriptCommandLexer::~ScriptCommandLexer()
         {
         }

         // ------------------------------- STATIC METHODS -------------------------------

         TokenList ScriptCommandLexer::Parse(const wstring& line)
         {
            return TokenList();
         }

         // ------------------------------- PUBLIC METHODS -------------------------------

         // ------------------------------ PROTECTED METHODS -----------------------------

         // ------------------------------- PRIVATE METHODS ------------------------------

      }
   }
}