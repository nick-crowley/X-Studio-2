#include "stdafx.h"
#include "ScriptEdit.h"
#include "Logic/CommandLexer.h"

   

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Controls)

   // --------------------------------- CONSTANTS ----------------------------------

   #define ALLOW_INPUT  FALSE
   #define BLOCK_INPUT  TRUE

   #define COMPILE_TIMER      42

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNAMIC(ScriptEdit, CRichEditCtrl)

   BEGIN_MESSAGE_MAP(ScriptEdit, CRichEditCtrl)
      ON_CONTROL_REFLECT(EN_CHANGE, &ScriptEdit::OnTextChange)
      ON_WM_CREATE()
      ON_WM_TIMER()
      ON_NOTIFY_REFLECT(EN_MSGFILTER, &ScriptEdit::OnInputMessage)
      ON_WM_KILLFOCUS()
      ON_WM_CHAR()
      ON_WM_KEYDOWN()
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   ScriptEdit::ScriptEdit() : Focus(Suggestion::None)
   {
   }

   ScriptEdit::~ScriptEdit()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   const wchar*  ScriptEdit::GetString(Suggestion& s) 
   {
      static const wchar* str[] = { L"None", L"Variable", L"Command", L"GameObject", L"ScriptObject", L"Label" };
      return str[(UINT)s];
   }

   // ------------------------------- PUBLIC METHODS -------------------------------

   #ifdef _DEBUG
   void ScriptEdit::AssertValid() const
   {
	   CRichEditCtrl::AssertValid();
   }
   
   void ScriptEdit::Dump(CDumpContext& dc) const
   {
	   CRichEditCtrl::Dump(dc);
   }
   #endif //_DEBUG

   
   /// <summary>Gets the character index of the caret with respect to the current line.</summary>
   /// <returns></returns>
   int ScriptEdit::GetCaretIndex() const
   {
      return GetSelection().cpMin - LineIndex(-1);
   }

   /// <summary>Gets the length of the line.</summary>
   /// <param name="line">The zero-based line index, or -1 for current line</param>
   /// <returns></returns>
   int ScriptEdit::GetLineLength(int line) const
   {
      return CRichEditCtrl::LineLength(LineIndex(line));
   }

   /// <summary>Get line text</summary>
   /// <param name="line">zero based line index, or -1 for current line</param>
   /// <returns></returns>
   wstring ScriptEdit::GetLineText(int line) const
   {
      // Resolve line #  (-1 not supported by CRichEditCtrl::GetLine())
      line = (line == -1 ? LineFromChar(-1) : line);

      // Allocate buffer long enough to hold text + buffer length stored in first DWORD
      int len = GetLineLength(line);
      len = max(4, len);
      
      // Get text. 
      CharArrayPtr txt(new WCHAR[len+1]);
      len = CRichEditCtrl::GetLine(line, txt.get(), len);

      // Strip trailing \v if present
      if (txt.get()[max(0,len-1)] == '\v')
         --len;

      // Null terminate
      txt.get()[len] = NULL;
      return txt.get();
   }
   
   /// <summary>Gets the co-ordinates of the current selection</summary>
   /// <returns></returns>
   CHARRANGE ScriptEdit::GetSelection() const
   {
      CHARRANGE sel;
      CRichEditCtrl::GetSel(sel);
      return sel;
   }

   /// <summary>Gets the length of the line by character index.</summary>
   /// <param name="nChar">The zero-based character index, or -1 for current line</param>
   /// <returns></returns>
   int ScriptEdit::LineLength(int nChar) const
   {
      return CRichEditCtrl::LineLength(nChar);
   }

   /// <summary>Replace entire contents with RTF.</summary>
   /// <param name="rtf">The RTF.</param>
   void ScriptEdit::SetRtf(const string& rtf)
   {
      SETTEXTEX opt = {ST_DEFAULT, CP_ACP};
      SendMessage(EM_SETTEXTEX, (WPARAM)&opt, (LPARAM)rtf.c_str());
   }
   
   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Closes the suggestion list</summary>
   void ScriptEdit::CloseSuggestions()
   {
      // Ensure exists
      if (Suggestions.GetSafeHwnd() == nullptr)
         throw InvalidOperationException(HERE, L"suggestion list does not exist");

      // Revert state
      State = InputState::Normal;

      // Destroy
      if (!Suggestions.DestroyWindow())
         throw Win32Exception(HERE, L"Unable to destroy suggestion list");
   }

   /// <summary>Selects and formats a token.</summary>
   /// <param name="offset">The character index of the line</param>
   /// <param name="t">The token</param>
   /// <param name="cf">The formatting characteristics</param>
   void ScriptEdit::FormatToken(UINT offset, const TokenBase& t, CharFormat& cf)
   {
      SetSel(offset+t.Start, offset+t.End);
      SetSelectionCharFormat(cf);
   }

   /// <summary>Freezes or unfreezes the window.</summary>
   /// <param name="freeze">True to freeze, false to restore</param>
   void ScriptEdit::FreezeWindow(bool freeze)
   {
      if (freeze)
      {
         // Freeze window
         SetRedraw(FALSE);

         // Preserve state
         GetSel(PrevState.Selection);
         PrevState.EventMask = SetEventMask(NULL);
         PrevState.ScrollPos = GetScrollCoordinates();
      }
      else
      {
         // Restore state
         SetSel(PrevState.Selection);
         SetScrollCoordinates(PrevState.ScrollPos);
         SetEventMask(PrevState.EventMask);

         // Redraw 
         SetRedraw(TRUE);
         Invalidate();
      }
   }
   
   /// <summary>Gets the coordinates of the first character</summary>
   /// <returns>Character co-orindates</returns>
   CPoint ScriptEdit::GetScrollCoordinates() const
   {
      // Preserve scroll position
      int pos = CharFromPos(CPoint(0,0));
      return CPoint(pos-LineIndex(pos), LineFromChar(pos));
   }

   
   /// <summary>Highlights errors indicated by the compiler</summary>
   /// <param name="t">Script tree</param>
   void ScriptEdit::HighlightErrors(ScriptParser::ScriptTree& t)
   {
      // Freeze window
      FreezeWindow(true);

      // Define underline
      CharFormat cf(CFM_UNDERLINE | CFM_UNDERLINETYPE, CFE_UNDERLINE);
      cf.bUnderlineType = CFU_UNDERLINEWAVE;

      // TEST: Undocumented underline colour
      //cf.bUnderlineColor = 0x02;

      //Console << L"Highlighting " << t.GetErrors().size() << L" errors" << ENDL;

      // Format errors
      for (const auto& err : t.GetErrors())
      {
         FormatToken(LineIndex(err.Line-1), err, cf);
         //Console << L"Syntax error on line " << err.Line << L": '" << (const WCHAR*)GetSelText() << ENDL;
      }

      // UnFreeze window
      FreezeWindow(false);
   }

   
   /// <summary>Identifies the type of suggestion to display in response to a character press</summary>
   /// <param name="ch">The character just typed</param>
   /// <returns></returns>
   ScriptEdit::Suggestion  ScriptEdit::IdentifySuggestion(wchar ch) const
   {
      switch (ch)
      {
      case '$': return Suggestion::Variable;
      case '{': return Suggestion::GameObject;
      case '[': return Suggestion::ScriptObject;
      case ' ': 
       {  
         CommandLexer lex(GetLineText(-1));
         TokenIterator pos = lex.begin();
         
         // Match: GoSub|Goto ' ' <caret>  { caret following single space after goto/gosub }
         if ((lex.Match(pos, TokenType::Keyword, L"gosub") || lex.Match(pos, TokenType::Keyword, L"goto"))
             && GetCaretIndex() == (pos-1)->End+1)
            return Suggestion::Label;
         break;
       }
      default:  
       { 
         // Ensure character is alpha-numeric
         if (!iswalpha(ch))
            return Suggestion::None;

         // Lex current line
         CommandLexer lex(GetLineText(-1));
         TokenIterator pos = lex.begin();

         // Rule: char <caret>  { first token is text. caret on 2nd letter }
         if (lex.Match(pos, TokenType::Text) && GetCaretIndex() == (pos-1)->Start+1)
            return Suggestion::Command;

         // Rule: variable '=' char <caret>  { 3 tokens: variable, equals, text. caret on 2nd letter }
         if (lex.Match(pos=lex.begin(), TokenType::Variable) && lex.Match(pos, TokenType::Operator, L"=") && lex.Match(pos, TokenType::Text)
             && GetCaretIndex() == (pos-1)->Start+1)
             return Suggestion::Command;

         
         // Rule: (variable '=')? constant/variable/null '->' char <caret>
         // match (variable '=')? while accounting for  variable '->'
         if (lex.Match(pos=lex.begin(), TokenType::Variable))
         {
            // (variable '=')?   
            if (lex.Match(pos, TokenType::Operator, L"="))
            {}
            // Reset position if (variable '->')
            else if (lex.Match(pos, TokenType::Operator, L"->"))
               pos=lex.begin();
            else
               return Suggestion::None;
         }
         
         // constant/variable/null 
         if (lex.Match(pos, TokenType::ScriptObject) || lex.Match(pos, TokenType::Variable) || lex.Match(pos, TokenType::Null))
            // '->' char <caret>  { caret on 2nd letter }
            if (lex.Match(pos, TokenType::Operator, L"->") && lex.Match(pos, TokenType::Text) && GetCaretIndex() == (pos-1)->Start+1)
               return Suggestion::Command;
       }
      }

      return Suggestion::None;
   }

   /// <summary>Inserts the current suggestion and closes the list</summary>
   void ScriptEdit::InsertSuggestion()
   {
      // Ensure exists
      if (Suggestions.GetSafeHwnd() == nullptr)
         throw InvalidOperationException(HERE, L"suggestion list does not exist");

      // TODO: insert
      Console << Colour::Green << L"Inserting suggestion: " << Suggestions.GetSuggestion() << ENDL;

      // Close
      CloseSuggestions();
   }

   /// <summary>Setup control</summary>
   /// <param name="lpCreateStruct">The create structure.</param>
   /// <returns></returns>
   int ScriptEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
   {
      if (CRichEditCtrl::OnCreate(lpCreateStruct) == -1)
         return -1;

      // Set background colour
      SetBackgroundColor(FALSE, RGB(0,0,0));

      // Notify on change/scroll/input
      SetEventMask(ENM_CHANGE | ENM_SCROLL | ENM_KEYEVENTS | ENM_MOUSEEVENTS);

      return 0;
   }
   

   /// <summary>Shows/updates the suggestion list in response to character input</summary>
   /// <param name="nChar">The character.</param>
   /// <param name="nRepCnt">The repeat count.</param>
   /// <param name="nFlags">The flags.</param>
   void ScriptEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
   {
      // Process character
      CRichEditCtrl::OnChar(nChar, nRepCnt, nFlags);

      try
      {
         // Update suggestions
         if (State == InputState::Suggestions)
            UpdateSuggestions();

         // display suggestions if appropriate
         else if ((Focus = IdentifySuggestion(nChar)) != Suggestion::None)
         {
            //Console << L"Identified " << GetString(Focus) << L" from " << (wchar)nChar << ENDL;
            ShowSuggestions();
         }
      }
      catch (ExceptionBase& e) {
         Console << e; 
      }
   }


   /// <summary>Updates the suggestion list in response to caret movement</summary>
   /// <param name="nChar">The character.</param>
   /// <param name="nRepCnt">The repeat count.</param>
   /// <param name="nFlags">The flags.</param>
   void ScriptEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
   {
      // Process caret movement
      CRichEditCtrl::OnKeyDown(nChar, nRepCnt, nFlags);

      try
      {
         switch (nChar)
         {
         // LEFT/RIGHT/HOME/END/DELETE/BACKSPACE: Update current match
         case VK_LEFT:
         case VK_RIGHT:
         case VK_HOME:
         case VK_END:
         case VK_DELETE:
         case VK_BACK:
            if (State == InputState::Suggestions)
               UpdateSuggestions();
            break;
         }
      }
      catch (ExceptionBase& e) {
         Console << e; 
      }
   }


   /// <summary>Blocks or forwards certain keys used in suggestion display</summary>
   /// <param name="pNMHDR">The notify header</param>
   /// <param name="pResult">message result</param>
   void ScriptEdit::OnInputMessage(NMHDR *pNMHDR, LRESULT *pResult)
   {
      MSGFILTER *pFilter = reinterpret_cast<MSGFILTER *>(pNMHDR);
      wchar chr = pFilter->wParam;
   
      // Allow input by default
      *pResult = ALLOW_INPUT;

      try
      {
         switch (pFilter->msg)
         {
         case WM_CHAR:
            // TAB: Insert suggestion.  Prevent focus switch
            if (State == InputState::Suggestions && chr == VK_TAB)
            {
               InsertSuggestion();
               *pResult = BLOCK_INPUT;
            }
            // ESCAPE: Close suggestions
            else if (State == InputState::Suggestions && chr == VK_ESCAPE)
            {
               CloseSuggestions();
               *pResult = BLOCK_INPUT;
            }
            break;

         // NAVIGATION: Update suggestions / Forward message
         case WM_KEYDOWN:
            if (State == InputState::Suggestions)
               switch (chr)
               {
               // TAB: Insert suggestion
               case VK_TAB:
                  InsertSuggestion();
                  *pResult = BLOCK_INPUT;
                  break;

               // ESCAPE: Close suggestions
               case VK_ESCAPE:
                  CloseSuggestions();
                  *pResult = BLOCK_INPUT;
                  break;

               // UP/DOWN/PAGEUP/PAGEDOWN: Navigate suggestions list
               case VK_UP:    
               case VK_DOWN:
               case VK_PRIOR: 
               case VK_NEXT:  
                  Suggestions.SendMessage(pFilter->msg, pFilter->wParam, pFilter->lParam);
                  *pResult = BLOCK_INPUT;
                  return;
               }
            break;

         // CLICK: Close Suggestions
         case WM_LBUTTONDOWN:
         case WM_RBUTTONDOWN:
         case WM_MBUTTONDOWN:
            if (State == InputState::Suggestions)
               CloseSuggestions();
            break;

         // WHEEL: Forward to list
         case WM_MOUSEWHEEL:
            Suggestions.SendMessage(pFilter->msg, pFilter->wParam, pFilter->lParam);
            *pResult = BLOCK_INPUT;
            break;
         }
      }
      catch (ExceptionBase& e) { 
         Console << e; 
      }
   }
   

   /// <summary>Closes the suggestion when focus lost</summary>
   /// <param name="pNewWnd">New window.</param>
   void ScriptEdit::OnKillFocus(CWnd* pNewWnd)
   {
      try
      {
         // Close suggestions, unless they're gaining focus
         if (State == InputState::Suggestions && (!pNewWnd || Suggestions != *pNewWnd))
            CloseSuggestions();
      }
      catch (ExceptionBase& e) {
         Console << e; 
      }

      CRichEditCtrl::OnKillFocus(pNewWnd);
   }


   /// <summary>Performs syntax colouring on the current line</summary>
   void ScriptEdit::OnTextChange()
   {
      // Freeze window
      FreezeWindow(true);
      
      // Set/Reset background compiler timer
      SetCompilerTimer(true);

      try 
      {
         CharFormat cf(CFM_COLOR | CFM_UNDERLINE | CFM_UNDERLINETYPE, NULL);
         UINT start = LineIndex(-1);

         // Lex current line
         CommandLexer lex(GetLineText(-1));
         
         // Format tokens
         for (const auto& tok : lex.Tokens)
         {
            // Set colour
            switch (tok.Type)
            {
            case TokenType::Comment:      cf.crTextColor = RGB(128,128,128);  break;
            case TokenType::Null:
            case TokenType::Variable:     cf.crTextColor = RGB(0,255,0);      break;
            case TokenType::Keyword:      cf.crTextColor = RGB(0,0,255);      break;
            case TokenType::Number:  
            case TokenType::String:       cf.crTextColor = RGB(255,0,0);      break;
            case TokenType::ScriptObject: cf.crTextColor = RGB(255,255,0);    break;
            case TokenType::GameObject:   cf.crTextColor = RGB(0,255,255);    break;
            default:                      cf.crTextColor = RGB(255,255,255);  break;
            }

            // Set format
            FormatToken(start, tok, cf);
         }
      }
      catch (ExceptionBase& e) { 
         Console << e << ENDL; 
      }

      // Restore selection
      FreezeWindow(false);
   }
   
   /// <summary>Compiles the current text</summary>
   /// <param name="nIDEvent">The timer identifier.</param>
   void ScriptEdit::OnTimer(UINT_PTR nIDEvent)
   {
      if (nIDEvent == COMPILE_TIMER)
      {
         CWaitCursor c;
         //Console << L"Background compiler activated" << ENDL;

         // End compiler timer
         SetCompilerTimer(false);
         
         // Get array of line text
         LineArray lines;
         for (int i = 0; i < GetLineCount(); i++)
            lines.push_back(GetLineText(i));
         
         // Parse script + highlight errors
         try 
         { 
            HighlightErrors(ScriptParser(lines, GameVersion::TerranConflict).ParseScript());
         }
         catch (ExceptionBase& e) 
         { 
            Console << e << ENDL; 
         }
      }

      CRichEditCtrl::OnTimer(nIDEvent);
   }
   
   /// <summary>Matches a token type against the current suggestion type</summary>
   /// <param name="t">The t.</param>
   /// <returns></returns>
   bool ScriptEdit::MatchSuggestionType(Compiler::TokenType t) const
   {
      switch (Focus)
      {
      case Suggestion::Variable:     return t == TokenType::Variable;
      case Suggestion::GameObject:   return t == TokenType::GameObject;
      case Suggestion::ScriptObject: return t == TokenType::ScriptObject;
      case Suggestion::Label:        return t == TokenType::Label;
      case Suggestion::Command:      return t == TokenType::Text;
      }

      return false;
   }
   
   /// <summary>Sets/resets/cancels the compiler timer.</summary>
   /// <param name="set">True to set/reset, false to cancel</param>
   void ScriptEdit::SetCompilerTimer(bool set)
   {
      // Set/Reset background compiler timer
      if (set)
         SetTimer(COMPILE_TIMER, 1500, nullptr);
      else
         KillTimer(COMPILE_TIMER);
   }

   /// <summary>Scrolls window to the position of a character</summary>
   /// <param name="pt">Character co-orindates</param>
   void ScriptEdit::SetScrollCoordinates(const CPoint& pt)
   {
      CPoint now = GetScrollCoordinates();
      CPoint diff = pt-now;
      LineScroll(diff.y, diff.x);
   }
   
   /// <summary>Shows the suggestion list</summary>
   void ScriptEdit::ShowSuggestions()
   {
      // Ensure does not exist
      if (Suggestions.GetSafeHwnd() != nullptr)
         throw InvalidOperationException(HERE, L"suggestion list already exists");

      // Show list
      if (!Suggestions.Create(this, GetCharPos(GetSelection().cpMin))) 
         throw Win32Exception(HERE, L"Unable to create suggestion list");

      // Update state
      State = InputState::Suggestions;
   }


   /// <summary>Scrolls/closes the suggestion list in response to caret movement or character input</summary>
   void ScriptEdit::UpdateSuggestions()
   {
      // Ensure exists
      if (Suggestions.GetSafeHwnd() == nullptr)
         throw InvalidOperationException(HERE, L"suggestion list does not exist");

      // Lex current line
      CommandLexer lex(GetLineText(-1));
      TokenIterator pos = lex.Find(GetCaretIndex());

      //Console << L"Updating from token '" << (lex.Valid(pos)?pos->Text:L"<end>") << L"' at index " << GetCaretIndex() << ENDL;
      
      // Close suggestions if caret has left the token
      if (!lex.Valid(pos) || !MatchSuggestionType(pos->Type))
      {
         //Console << Colour::Red << L"Match failed" << ENDL;
         CloseSuggestions();
         return;
      }

      // Highlight best match
      //Console << Colour::Green << L"Matching suggestion" << ENDL;
      Suggestions.MatchSuggestion(*pos);
   }

   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
NAMESPACE_END2(GUI,Controls)

