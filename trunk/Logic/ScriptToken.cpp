#include "stdafx.h"
#include "ScriptToken.h"

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         // -------------------------------- CONSTRUCTION --------------------------------

         /*ScriptToken::ScriptToken()
         {
         }*/


         /*ScriptToken::~ScriptToken()
         {
         }*/

         // ------------------------------- STATIC METHODS -------------------------------

         /// <summary>Get token type string</summary>
         LogicExport GuiString  GetString(TokenType t)
         {
            static const wchar* str[] = { L"Text", L"Number", L"String", L"GameObject", L"ScriptObject", L"Keyword", L"Variable", L"Null", L"Label", L"BinaryOp", L"UnaryOp", L"Comment", L"Whitespace" };
            return str[(UINT)t];
         }

         /// <summary>Write script token to the console</summary>
         LogicExport ConsoleWnd& operator<<(ConsoleWnd& c, const ScriptToken& tok)
         {
            return c << "{ScriptToken: Type=" << GetString(tok.Type) << " Text=" << tok.Text << " Start=" << tok.Start << "}";
         }

         // ------------------------------- PUBLIC METHODS -------------------------------

         // ------------------------------ PROTECTED METHODS -----------------------------

         // ------------------------------- PRIVATE METHODS ------------------------------
      }
   }
}


