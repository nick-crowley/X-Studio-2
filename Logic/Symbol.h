#pragma once

#include "ScriptToken.h"
#include <algorithm>

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         
         /// <summary>Types of symbol available</summary>
         enum class SymbolType { Variable, Label };

         /// <summary>Represents a symbol used during refactoring process</summary>
         class LogicExport Symbol
         {
            // --------------------- CONSTRUCTION ----------------------
         public:
            /// <summary>Create symbol match.</summary>
            /// <param name="name">name without operators.</param>
            /// <param name="t">type.</param>
            /// <param name="line">1-based line number</param>
            /// <param name="txt">line text</param>
            /// <param name="comment">Whether line is commented</param>
            Symbol(const ScriptToken& tok, SymbolType t, UINT line, const wstring& txt, bool comment) 
               : Token(tok), Type(t), LineNumber(line), LineText(txt), Commented(comment)
            {}

            // ---------------------- ACCESSORS ------------------------	

            // ----------------------- MUTATORS ------------------------

            // -------------------- REPRESENTATION ---------------------
         public:
            const SymbolType  Type;          // Type
            const ScriptToken Token;         // Name
            const UINT        LineNumber;    // 1-based line number
            const GuiString   LineText;      // Line text
            const bool        Commented;     // Whether commented
         };

         /// <summary></summary>
         typedef list<Symbol>  SymbolList;
            
      }
   }
}

using namespace Logic::Scripts::Compiler;
