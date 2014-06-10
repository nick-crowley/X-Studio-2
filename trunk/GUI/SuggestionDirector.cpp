#include "stdafx.h"
#include "ScriptEdit.h"

namespace GUI
{
   namespace Controls
   {
   
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Creates a suggestion director for a script edit.</summary>
      /// <param name="e">Script edit</param>
      /// <exception cref="Logic::ArgumentNullException">script edit is null</exception>
      ScriptEdit::SuggestionDirector::SuggestionDirector(ScriptEdit* e) 
         : Edit(e),
           State(InputState::Normal),
           SuggestionType(Suggestion::None),
           SuggestionRect(0,0,0,0)
      {
         REQUIRED(e);
      }


      ScriptEdit::SuggestionDirector::~SuggestionDirector()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------
      
      /// <summary>Freezes or unfreezes the window.</summary>
      /// <param name="freeze">True to freeze, false to restore</param>
      /// <param name="invalidate">True to invalidate after unfreezing</param>
      void ScriptEdit::SuggestionDirector::FreezeWindow(bool freeze, bool invalidate)
      {
         if (State != InputState::Suggestions)
            return;

         // Freeze/Restore suggestion list
         if (freeze)
            SuggestionRect = CtrlRect(Edit, &SuggestionsList);
         else
            SuggestionsList.SetWindowPos(nullptr, SuggestionRect.left, SuggestionRect.top, -1, -1, SWP_NOZORDER|SWP_NOSIZE);
      }
   
      
      /// <summary>Shows/updates the suggestion list in response to character input</summary>
      /// <param name="nChar">The character.</param>
      /// <param name="nRepCnt">The repeat count.</param>
      /// <param name="nFlags">The flags.</param>
      void ScriptEdit::SuggestionDirector::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
      {
         try
         {
            // DEBUG: Console << "SuggestionDirector::OnChar" << " nChar=" << (wchar)nChar << ENDL;

            // Initiate suggestions if appliciable
            if (State != InputState::Suggestions)
            {
               // Identify/Set suggestions to display (if any)
               SuggestionType = IdentifySuggestion(nChar);
               //Console << L"Identified " << GetString(SuggestionType) << L" from " << (wchar)nChar << ENDL;

               // Display new suggestions 
               if (SuggestionType != Suggestion::None)
                  ShowSuggestions(); 
            }

            // Update existing suggestions
            if (State == InputState::Suggestions)
               UpdateSuggestions();
         }
         catch (ExceptionBase& e) {
            Console.Log(HERE, e, VString(L"Unable to process '%c' character", (wchar)nChar)); 
         }
      }

      /// <summary>Updates the suggestion list in response to caret movement</summary>
      /// <param name="nChar">The character.</param>
      /// <param name="nRepCnt">The repeat count.</param>
      /// <param name="nFlags">The flags.</param>
      void ScriptEdit::SuggestionDirector::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
      {
         try 
         {  
            // DEBUG: Console << "SuggestionDirector::OnKeyDown" << " nChar=" << (wchar)nChar << ENDL;

            // Ignore when not showing suggestions
            if (State != InputState::Suggestions)
               return;

            // Update in response to caret movement
            switch (nChar)
            {
            // TAB/LEFT/RIGHT/HOME/END/DELETE/BACKSPACE: 
            case VK_TAB:
            case VK_LEFT:
            case VK_RIGHT:
            case VK_HOME:
            case VK_END:
            case VK_DELETE:
            case VK_BACK:
               UpdateSuggestions();
               break;
            }
         } 
         catch (ExceptionBase& e) {
            Console.Log(HERE, e, VString(L"Unable to process '%d' key (char '%c')", nChar, (wchar)nChar)); 
         }
      }
   
      
      /// <summary>Closes the suggestion when edit or list loses input focus</summary>
      /// <param name="pNewWnd">New window.</param>
      void ScriptEdit::SuggestionDirector::OnKillFocus(CWnd* pNewWnd)
      {
         try
         {
            // DEBUG: Console << "SuggestionDirector::OnKillFocus" << " pNewWnd=" << pNewWnd << ENDL;

            // Ensure exist
            if (State != InputState::Suggestions)
               return;

            // Close suggestions when focus lost to anything but edit/list
            if (!pNewWnd || (*pNewWnd != SuggestionsList && *pNewWnd != *Edit))
               CloseSuggestions();
         }
         catch (ExceptionBase& e) {
            Console.Log(HERE, e); 
         }
      }
      
      /// <summary>Query whether mouse/keyboard message should be handled by SuggestionList</summary>
      /// <param name="msg">Mouse/Keyboard message</param>
      /// <param name="wParam">parameter</param>
      /// <param name="lParam">parameter</param>
      /// <returns>True to block input message, False to allow</returns>
      bool ScriptEdit::SuggestionDirector::WantMessage(UINT msg, WPARAM wParam, LPARAM lParam)
      {
         wchar chr = static_cast<wchar>(wParam);

         try
         {
            // Ignore all messages when not displaying suggestions
            if (State != InputState::Suggestions)
               return false;

            switch (msg)
            {
            // CHARACTERS:
            case WM_CHAR:  // DEBUG: Console << "SuggestionDirector::WantMessage" << " msg=WM_CHAR chr=" << chr << ENDL;
               switch (chr)
               {
               // TAB/ESCAPE: Insert/close suggestions
               case VK_TAB:    InsertSuggestion();  return true;
               case VK_ESCAPE: CloseSuggestions();  return true;
               }
               break;

            // KEYSTROKES: 
            case WM_KEYDOWN:  // DEBUG: Console << "SuggestionDirector::WantMessage" << " msg=WM_KEYDOWN chr=" << chr << ENDL;
               switch (chr)
               {
               // TAB/ESCAPE: Block keystroke
               case VK_TAB:
               case VK_ESCAPE:
                  return true;

               // UP/DOWN/PAGEUP/PAGEDOWN: Navigate suggestions list
               case VK_UP:    
               case VK_DOWN:
               case VK_PRIOR: 
               case VK_NEXT:  
                  SuggestionsList.SendMessage(msg, wParam, lParam);
                  return true;
               }
               break;

            // CLICK: Close Suggestions
            case WM_LBUTTONDOWN:
            case WM_RBUTTONDOWN:
            case WM_MBUTTONDOWN:
               CloseSuggestions();
               return false;

            // WHEEL: Forward to list
            case WM_MOUSEWHEEL:
               SuggestionsList.SendMessage(msg, wParam, lParam);
               return true;
            }

            return false;
         }
         catch (ExceptionBase& e) { 
            Console.Log(HERE, e, VString(L"Unable to process input filter: message=%d wparam=%d lparam=%d char='%c'", msg, wParam, lParam, chr) ); 
            return false;
         }
      }
      
      // ------------------------------ PROTECTED METHODS -----------------------------
      
      /// <summary>Closes the suggestion list</summary>
      /// <exception cref="Logic::InvalidOperationException">Suggestions don't exists</exception>
      /// <exception cref="Logic::Win32Exception">Unable to destroy suggestion list</exception>
      void ScriptEdit::SuggestionDirector::CloseSuggestions()
      {
         // Ensure exists
         if (SuggestionsList.GetSafeHwnd() == nullptr)
            throw InvalidOperationException(HERE, L"suggestion list does not exist");

         // Revert state
         State = InputState::Normal;

         // Destroy
         if (!SuggestionsList.DestroyWindow())
            throw Win32Exception(HERE, L"Unable to destroy suggestion list");
      }
   
      /// <summary>Gets the suggestion rect.</summary>
      /// <param name="type">The type.</param>
      /// <returns></returns>
      CRect  ScriptEdit::SuggestionDirector::GetSuggestionRect(Suggestion type) const
      {
         ClientRect wnd(Edit);

         // Position beneath current line
         CRect rc(Edit->GetCharPos(Edit->GetSelection().cpMin), SuggestionList::GetDefaultSize(type));
         rc.OffsetRect(0, Edit->GetLineHeight());

         // Intersects Bottom: Position above line
         if (rc.bottom >= wnd.bottom)
            rc.OffsetRect(0, -Edit->GetLineHeight() - SuggestionList::GetDefaultSize(type).cy);

         // Intersects Right: Move left until visible
         if (rc.right >= wnd.right)
            rc.OffsetRect(-(rc.right-wnd.right), 0);
      
         return rc;
      }
   
      /// <summary>Identifies the type of suggestion to display in response to a character press</summary>
      /// <param name="ch">The character just typed</param>
      /// <returns></returns>
      Suggestion  ScriptEdit::SuggestionDirector::IdentifySuggestion(wchar ch) const
      {
         switch (ch)
         {
         // VARIABLE/GAME-OBJ/SCRIPT-OBJ
         case '$': return Suggestion::Variable;
         case '{': return Suggestion::GameObject;
         case '[': return Suggestion::ScriptObject;
      
         // LABEL/COMMAND:
         default:  
          { 
            // Ensure character is alpha-numeric
            if (!iswalpha(ch))
               return Suggestion::None;

            // Lex current line
            CommandLexer lex(Edit->GetLineText(-1));
            TokenIterator pos = lex.begin();

            // (Label) Rule: GoSub|Goto <whitespace> label|anything <caret>  (Desc: caret following token after goto/gosub)
            if ((lex.Match(pos, TokenType::Keyword, L"gosub") || lex.Match(pos, TokenType::Keyword, L"goto"))
             && (lex.Match(pos, TokenType::Label) || !lex.Valid(pos)) )  //&& GetCaretIndex() == (pos-1)->End+1)
               return Suggestion::Label;

            // (Command) Rule: char <caret>  (NB: first token is text. caret on 2nd letter)
            if (lex.Match(pos, TokenType::Text) && Edit->GetCaretIndex() == (pos-1)->Start+1)
               return Suggestion::Command;

            // (Command) Rule: variable '=' char <caret>  (NB: 3 tokens: variable, equals, text. caret on 2nd letter)
            if (lex.Match(pos=lex.begin(), TokenType::Variable) && lex.Match(pos, TokenType::BinaryOp, L"=") && lex.Match(pos, TokenType::Text)
                && Edit->GetCaretIndex() == (pos-1)->Start+1)
                return Suggestion::Command;

            // (Command) Rule: (variable '=')? constant/variable/null '->' char <caret>
            // match (variable '=')? while accounting for  variable '->'
            if (lex.Match(pos=lex.begin(), TokenType::Variable))
            {
               // (variable '=')?   
               if (lex.Match(pos, TokenType::BinaryOp, L"="))
               {}
               // Reset position if (variable '->')
               else if (lex.Match(pos, TokenType::BinaryOp, L"->"))
                  pos=lex.begin();
               else
                  return Suggestion::None;
            }
         
            // (Command) constant/variable/null 
            if (lex.Match(pos, TokenType::ScriptObject) || lex.Match(pos, TokenType::Variable) || lex.Match(pos, TokenType::Null))
               // '->' char <caret>  { caret on 2nd letter }
               if (lex.Match(pos, TokenType::BinaryOp, L"->") && lex.Match(pos, TokenType::Text) && Edit->GetCaretIndex() == (pos-1)->Start+1)
                  return Suggestion::Command;
          }
         }

         return Suggestion::None;
      }

      /// <summary>Inserts the current suggestion and closes the list</summary>
      /// <exception cref="Logic::InvalidOperationException">Suggestion list doesn't exist</exception>
      /// <exception cref="Logic::Win32Exception">Unable to destroy suggestion list</exception>
      void ScriptEdit::SuggestionDirector::InsertSuggestion()
      {
         // Ensure exists
         if (SuggestionsList.GetSafeHwnd() == nullptr)
            throw InvalidOperationException(HERE, L"suggestion list does not exist");

         // DEBUG:
         Console << L"Inserting suggestion: " << Cons::Yellow << SuggestionsList.GetSelected() << ENDL;

         // Find token at caret
         CommandLexer lex(Edit->GetLineText(-1));
         TokenIterator pos = lex.Find(Edit->GetCaretIndex());

         // Replace token with suggestion
         if (pos != lex.end())
         {
            Edit->SetSel(Edit->LineIndex(-1)+pos->Start, Edit->LineIndex(-1)+pos->End);
            Edit->ReplaceSel(SuggestionsList.GetSelected().c_str(), TRUE);
         }
         else 
            // Error: Failed to identify token
            Console << Cons::Error << "Cannot find suggestion token: " 
                    << Cons::White << " caret at " << Edit->GetCaretIndex() << " : " 
                    << Cons::Yellow << Edit->GetLineText(-1) << ENDL;

         // Close
         CloseSuggestions();
      }
   
      /// <summary>Matches a token type against the current suggestion type</summary>
      /// <param name="t">The t.</param>
      /// <returns></returns>
      bool ScriptEdit::SuggestionDirector::MatchSuggestionType(Compiler::TokenType t) const
      {
         switch (SuggestionType)
         {
         case Suggestion::Variable:     return t == TokenType::Variable;
         case Suggestion::GameObject:   return t == TokenType::GameObject;
         case Suggestion::ScriptObject: return t == TokenType::ScriptObject || t == TokenType::UnaryOp;
         case Suggestion::Label:        return t == TokenType::Label;
         case Suggestion::Command:      return t == TokenType::Text;
         }

         return false;
      }
   
      /// <summary>Shows the suggestion list</summary>
      /// <exception cref="Logic::InvalidOperationException">Suggestions already exists</exception>
      /// <exception cref="Logic::Win32Exception">Unable to create suggestion list</exception>
      void ScriptEdit::SuggestionDirector::ShowSuggestions()
      {
         // Ensure does not exist
         if (SuggestionsList.GetSafeHwnd() != nullptr)
            throw InvalidOperationException(HERE, L"suggestion list already exists");

         // Show list
         if (!SuggestionsList.Create(Edit, GetSuggestionRect(SuggestionType), SuggestionType, &Edit->Document->Script)) 
            throw Win32Exception(HERE, L"Unable to create suggestion list");

         // Update state
         State = InputState::Suggestions;
      }

   
      /// <summary>Scrolls/closes the suggestion list in response to caret movement or character input</summary>
      /// <exception cref="Logic::InvalidOperationException">Suggestion list doesn't exists</exception>
      /// <exception cref="Logic::Win32Exception">Unable to destroy suggestion list</exception>
      void ScriptEdit::SuggestionDirector::UpdateSuggestions()
      {
         // Ensure exists
         if (SuggestionsList.GetSafeHwnd() == nullptr)
            throw InvalidOperationException(HERE, L"suggestion list does not exist");

         // Find token at caret
         CommandLexer lex(Edit->GetLineText(-1));
         TokenIterator pos = lex.Find(Edit->GetCaretIndex());

         //Console << L"Updating from token '" << (lex.Valid(pos)?pos->Text:L"<end>") << L"' at index " << GetCaretIndex() << ENDL;
      
         // Close suggestions if caret has left the token
         if (!lex.Valid(pos) || !MatchSuggestionType(pos->Type))
         {
            //Console << Cons::Error << L"Match failed" << ENDL;
            CloseSuggestions();
            return;
         }

         // Highlight best match
         //Console << Cons::Green << L"Matching suggestion" << ENDL;
         SuggestionsList.MatchSuggestion(*pos);
      }

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}
