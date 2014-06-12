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
      
      /// <summary>Identifies when user types an assignment</summary>
      /// <param name="lex">Lexer for line containing caret.</param>
      /// <param name="caret">Zero-based char index of caret.</param>
      /// <param name="ch">last character typed.</param>
      /// <returns>GameObject/ScriptObject/Variable if successful, otherwise None</returns>
      Suggestion ScriptEdit::SuggestionDirector::AssignmentVisitor::Identify(CommandLexer& lex, UINT caret, wchar ch) const
      {
         // Ensure character is alpha-numeric
         if (!iswalpha(ch))
            return Suggestion::None;

         // Lex current line
         TokenIterator pos = lex.begin();

         // RULE: variable '=' char <caret>  
         if (lex.Match(pos=lex.begin(), TokenType::Variable) 
          && lex.Match(pos, TokenType::BinaryOp, L"=") 
          && lex.Match(pos, TokenType::Text)
          && (pos-1)->ContainsEx(caret) )
            return Suggestion::Command;

         return Suggestion::None;
      }

      /// <summary>Identifies when user types a command without assignment</summary>
      /// <param name="lex">Lexer for line containing caret.</param>
      /// <param name="caret">Zero-based char index of caret.</param>
      /// <param name="ch">last character typed.</param>
      /// <returns>GameObject/ScriptObject/Variable if successful, otherwise None</returns>
      Suggestion ScriptEdit::SuggestionDirector::DiscardVisitor::Identify(CommandLexer& lex, UINT caret, wchar ch) const
      {
         // Ensure character is alpha-numeric
         if (!iswalpha(ch))
            return Suggestion::None;

         // Lex current line
         TokenIterator pos = lex.begin();

         // RULE: char <caret> 
         if (lex.Match(pos=lex.begin(), TokenType::Text)
          && (pos-1)->ContainsEx(caret) )
            return Suggestion::Command;

         return Suggestion::None;
      }
      
      /// <summary>Identifies when user types a command preceeded by a conditional</summary>
      /// <param name="lex">Lexer for line containing caret.</param>
      /// <param name="caret">Zero-based char index of caret.</param>
      /// <param name="ch">last character typed.</param>
      /// <returns>GameObject/ScriptObject/Variable if successful, otherwise None</returns>
      Suggestion ScriptEdit::SuggestionDirector::ConditionalVisitor::Identify(CommandLexer& lex, UINT caret, wchar ch) const
      {
         // Ensure character is alpha-numeric
         if (!iswalpha(ch))
            return Suggestion::None;

         // Lex current line
         TokenIterator pos = lex.begin();

         // RULE: conditional char <caret>
         if ((lex.Match(pos, TokenType::Keyword, L"skip") && lex.Match(pos, TokenType::Keyword, L"if") && lex.Match(pos, TokenType::Keyword, L"not"))
          || (lex.Match(pos=lex.begin(), TokenType::Keyword, L"skip") && lex.Match(pos, TokenType::Keyword, L"if"))
          || (lex.Match(pos=lex.begin(), TokenType::Keyword, L"if") && lex.Match(pos, TokenType::Keyword, L"not"))
          || lex.Match(pos=lex.begin(), TokenType::Keyword, L"if") 
          || (lex.Match(pos=lex.begin(), TokenType::Keyword, L"while") && lex.Match(pos, TokenType::Keyword, L"not"))
          || lex.Match(pos=lex.begin(), TokenType::Keyword, L"while")
          || (lex.Match(pos=lex.begin(), TokenType::Keyword, L"do") && lex.Match(pos, TokenType::Keyword, L"if")) )
            // <continued...>
            if (lex.Match(pos, TokenType::Text)
             && (pos-1)->ContainsEx(caret) )
               return Suggestion::Command;

         return Suggestion::None;
      }

      /// <summary>Identifies when user types 'goto <label>'</summary>
      /// <param name="lex">Lexer for line containing caret.</param>
      /// <param name="caret">Zero-based char index of caret.</param>
      /// <param name="ch">last character typed.</param>
      /// <returns>GameObject/ScriptObject/Variable if successful, otherwise None</returns>
      Suggestion ScriptEdit::SuggestionDirector::LabelVisitor::Identify(CommandLexer& lex, UINT caret, wchar ch) const
      {
         // Ensure character is alpha-numeric
         if (!iswalpha(ch))
            return Suggestion::None;

         // Lex current line
         TokenIterator pos = lex.begin();

         // RULE: GoSub|Goto <whitespace> <token> <caret>  
         if (lex.Match(pos, TokenType::Keyword, L"gosub") 
          || lex.Match(pos, TokenType::Keyword, L"goto"))
            // <continued...>
            if (!lex.Valid(pos))
               return Suggestion::Label;

         return Suggestion::None;
      }
      
      /// <summary>Identifies when user types a variable/game-object/script-object</summary>
      /// <param name="lex">Lexer for line containing caret.</param>
      /// <param name="caret">Zero-based char index of caret.</param>
      /// <param name="ch">last character typed.</param>
      /// <returns>GameObject/ScriptObject/Variable if successful, otherwise None</returns>
      Suggestion ScriptEdit::SuggestionDirector::LiteralVisitor::Identify(CommandLexer& lex, UINT caret, wchar ch) const
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
      
      /// <summary>Identifies when user types a command upon a reference object</summary>
      /// <param name="lex">Lexer for line containing caret.</param>
      /// <param name="caret">Zero-based char index of caret.</param>
      /// <param name="ch">last character typed.</param>
      /// <returns>GameObject/ScriptObject/Variable if successful, otherwise None</returns>
      Suggestion ScriptEdit::SuggestionDirector::RefObjectAssignmentVisitor::Identify(CommandLexer& lex, UINT caret, wchar ch) const
      {
         // Ensure character is alpha-numeric
         if (!iswalpha(ch))
            return Suggestion::None;

         // Lex current line
         TokenIterator pos = lex.begin();

         // RULE: (variable '=')? constant/variable/null '->' char <caret>
         if (lex.Match(pos=lex.begin(), TokenType::Variable)
          && lex.Match(pos, TokenType::BinaryOp, L"="))
            // <continued...>
            if (lex.Match(pos, TokenType::ScriptObject) 
             || lex.Match(pos, TokenType::Variable) 
             || lex.Match(pos, TokenType::Null))
               // <continued...>
               if (lex.Match(pos, TokenType::BinaryOp, L"->") 
                && lex.Match(pos, TokenType::Text) 
                && (pos-1)->ContainsEx(caret) )
                  return Suggestion::Command;

         return Suggestion::None;
      }
      
      /// <summary>Identifies when user types a command upon a reference object</summary>
      /// <param name="lex">Lexer for line containing caret.</param>
      /// <param name="caret">Zero-based char index of caret.</param>
      /// <param name="ch">last character typed.</param>
      /// <returns>GameObject/ScriptObject/Variable if successful, otherwise None</returns>
      Suggestion ScriptEdit::SuggestionDirector::RefObjectDiscardVisitor::Identify(CommandLexer& lex, UINT caret, wchar ch) const
      {
         // Ensure character is alpha-numeric
         if (!iswalpha(ch))
            return Suggestion::None;

         // Lex current line
         TokenIterator pos = lex.begin();

         // RULE: constant/variable/null '->' char <caret>
         if (lex.Match(pos, TokenType::ScriptObject) 
          || lex.Match(pos, TokenType::Variable) 
          || lex.Match(pos, TokenType::Null))
            // <continued...>
            if (lex.Match(pos, TokenType::BinaryOp, L"->") 
             && lex.Match(pos, TokenType::Text) 
             && (pos-1)->ContainsEx(caret) )
               return Suggestion::Command;

         return Suggestion::None;
      }
      
      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}

