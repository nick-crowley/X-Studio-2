#include "stdafx.h"
#include "SuggestionVisitor.h"
#include "ScriptEdit.h"
#include "SuggestionList.h"

namespace GUI
{
   namespace Controls
   {
   
      // -------------------------------- CONSTRUCTION --------------------------------

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------
      
      /// <summary>Identifies when to display variable/game-object/script-object suggestions</summary>
      /// <param name="d">suggestion director.</param>
      /// <param name="ch">last character typed.</param>
      /// <returns>GameObject/ScriptObject/Variable if successful, otherwise None</returns>
      Suggestion ScriptEdit::SuggestionDirector::LabelVisitor::Identify(SuggestionDirector* d, wchar ch) const
      {
         // Ensure character is alpha-numeric
         if (!iswalpha(ch))
            return Suggestion::None;

         // Lex current line
         CommandLexer  lex(d->Edit->GetLineText(-1));
         TokenIterator pos = lex.begin();

         // RULE: GoSub|Goto <whitespace> <token> <caret>  
         if (lex.Match(pos, TokenType::Keyword, L"gosub") 
          || lex.Match(pos, TokenType::Keyword, L"goto"))
            // <continued...>
            if (!lex.Valid(pos))
               return Suggestion::Label;

         return Suggestion::None;
      }

      /// <summary>Identifies when to display variable/game-object/script-object suggestions</summary>
      /// <param name="d">suggestion director.</param>
      /// <param name="ch">last character typed.</param>
      /// <returns>GameObject/ScriptObject/Variable if successful, otherwise None</returns>
      Suggestion ScriptEdit::SuggestionDirector::LiteralVisitor::Identify(SuggestionDirector* d, wchar ch) const
      {
         switch (ch)
         {
         // VARIABLE/GAME-OBJ/SCRIPT-OBJ
         case '$': return Suggestion::Variable;
         case '{': return Suggestion::GameObject;
         case '[': return Suggestion::ScriptObject;
         }

         // Unrecognised
         return Suggestion::None;
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}

