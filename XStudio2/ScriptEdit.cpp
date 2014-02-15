#include "stdafx.h"
#include "ScriptEdit.h"
#include "Logic/CommandLexer.h"
#include "ScriptDocument.h"
   
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
      ON_WM_PAINT()
      ON_WM_HSCROLL()
      ON_WM_VSCROLL()
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   ScriptEdit::ScriptEdit() : SuggestionType(Suggestion::None), Document(nullptr)
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

   /// <summary>Ensures a line is visible.</summary>
   /// <param name="line">1-based line number</param>
   /// <returns></returns>
   bool  ScriptEdit::EnsureVisible(int line)
   {
      SetScrollCoordinates(CPoint(0, max(0,line)));
      return true;
   }
   
   /// <summary>Gets the character index of the caret with respect to the current line.</summary>
   /// <returns></returns>
   int ScriptEdit::GetCaretIndex() const
   {
      return GetSelection().cpMin - LineIndex(-1);
   }

   /// <summary>Gets the caret position.</summary>
   /// <returns>Zero-based character index, 1-based line index</returns>
   POINT ScriptEdit::GetCaretLocation() const
   {
      return POINT {GetCaretIndex(), 1+LineFromChar(-1)};
   }

   /// <summary>Gets the character index of the end of a line.</summary>
   /// <param name="line">The zero-based line index, or -1 for current line</param>
   /// <returns></returns>
   int  ScriptEdit::GetLineEnd(int line) const
   {
      return GetLineStart(line) + GetLineLength(line);
   }

   /// <summary>Gets the length of the line.</summary>
   /// <param name="line">The zero-based line index, or -1 for current line</param>
   /// <returns></returns>
   int ScriptEdit::GetLineLength(int line) const
   {
      return CRichEditCtrl::LineLength(LineIndex(line));
   }
   
   /// <summary>Gets the character index of the start of a line.</summary>
   /// <param name="line">The zero-based line index, or -1 for current line</param>
   /// <returns></returns>
   int ScriptEdit::GetLineStart(int line) const
   {
      return LineIndex(line);
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
      txt.get()[len] = '\0';
      return txt.get();
   }

   /// <summary>Gets entire script as array of plain-text lines.</summary>
   /// <returns></returns>
   LineArray ScriptEdit::GetLines() const
   {
      LineArray lines; 
      // Get lines
      for (int i = 0; i < GetLineCount(); i++)
         lines.push_back(GetLineText(i));
      return lines;   
   }
   
   /// <summary>Gets the co-ordinates of the current selection</summary>
   /// <returns></returns>
   CHARRANGE ScriptEdit::GetSelection() const
   {
      CHARRANGE sel;
      CRichEditCtrl::GetSel(sel);
      return sel;
   }

   /// <summary>Determines whether text selection exists</summary>
   /// <returns></returns>
   bool ScriptEdit::HasSelection() const
   {
      auto sel = GetSelection();
      return sel.cpMin != sel.cpMax;
   }

   /// <summary>Indents or outdents the selected lines.</summary>
   /// <param name="indent">True to indent, false to outdent.</param>
   void  ScriptEdit::IndentSelection(bool indent)
   {
      vector<GuiString>  lines;
      GuiString          output;

      // Get index of first/last line
      int first = LineFromChar(GetSelection().cpMin),
          last = LineFromChar(GetSelection().cpMax);
      
      // Select lines entirely
      SetSel(LineIndex(first), LineIndex(last)+GetLineLength(last));

      // Freeze window
      FreezeWindow(true);

      // DEBUG:
      //Console << "Indenting first=" << first << " last=" << last << " indent=" << indent << ENDL;

      // Get selected lines
      int length = 0;
      for (int i = first; i <= last; i++)
      {
         // Get line text
         lines.push_back( GetLineText(i) );
         GuiString& ln = lines.back();

         // In/Outdent by adding/removing tab from start of each line
         if (indent)
            ln.insert(0, L"   ");
         else if (ln.Left(3) == L"   ")
            ln.erase(0, 3);

         // Add to output
         length += ln.length();
         output += ln;

         // CRLF
         if (i != last)
         {
            output += L"\r";
            length++;
         }
      }

      //// Replace existing selection
      ReplaceSel(output.c_str(), TRUE);

      // Unfreeze window
      FreezeWindow(false);
      SetSel(LineIndex(first), LineIndex(first)+length);
   }

   /// <summary>Gets the length of the line by character index.</summary>
   /// <param name="nChar">The zero-based character index, or -1 for current line</param>
   /// <returns></returns>
   int ScriptEdit::LineLength(int nChar) const
   {
      return CRichEditCtrl::LineLength(nChar);
   }

   /// <summary>Sets the source document for this control</summary>
   /// <param name="doc">The document.</param>
   void  ScriptEdit::SetDocument(ScriptDocument* doc)
   {
      Document = doc;
   }

   /// <summary>Replace entire contents with RTF.</summary>
   /// <param name="rtf">The RTF.</param>
   /// <exception cref="Logic::InvalidOperationException">No document attached</exception>
   void ScriptEdit::SetRtf(const string& rtf)
   {
      if (Document == nullptr)
         throw InvalidOperationException(HERE, L"Must attach document prior to displaying text");

      // Extend character limit
      LimitText(256*1024);

      // Replace contents with RTF
      SETTEXTEX opt = {ST_DEFAULT, CP_ACP};
      if (!SendMessage(EM_SETTEXTEX, (WPARAM)&opt, (LPARAM)rtf.c_str()))
         throw Win32Exception(HERE, L"Unable to set script text");

      // Adjust gutter
      SetGutterWidth(GutterRect(this).Width()+10);

      // Clear 'Undo' buffer
      EmptyUndoBuffer();
   }
   
   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Closes the suggestion list</summary>
   void ScriptEdit::CloseSuggestions()
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
   /// <param name="invalidate">True to invalidate after unfreezing</param>
   void ScriptEdit::FreezeWindow(bool freeze, bool invalidate)
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
         if (invalidate)
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

   /// <summary>Determines whether document is connected</summary>
   /// <returns></returns>
   bool  ScriptEdit::HasDocument() const
   {
      return Document != nullptr;
   }
   
   /// <summary>Identifies the type of suggestion to display in response to a character press</summary>
   /// <param name="ch">The character just typed</param>
   /// <returns></returns>
   Suggestion  ScriptEdit::IdentifySuggestion(wchar ch) const
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
         if (lex.Match(pos=lex.begin(), TokenType::Variable) && lex.Match(pos, TokenType::BinaryOp, L"=") && lex.Match(pos, TokenType::Text)
             && GetCaretIndex() == (pos-1)->Start+1)
             return Suggestion::Command;

         
         // Rule: (variable '=')? constant/variable/null '->' char <caret>
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
         
         // constant/variable/null 
         if (lex.Match(pos, TokenType::ScriptObject) || lex.Match(pos, TokenType::Variable) || lex.Match(pos, TokenType::Null))
            // '->' char <caret>  { caret on 2nd letter }
            if (lex.Match(pos, TokenType::BinaryOp, L"->") && lex.Match(pos, TokenType::Text) && GetCaretIndex() == (pos-1)->Start+1)
               return Suggestion::Command;
       }
      }

      return Suggestion::None;
   }

   /// <summary>Inserts the current suggestion and closes the list</summary>
   void ScriptEdit::InsertSuggestion()
   {
      // Ensure exists
      if (SuggestionsList.GetSafeHwnd() == nullptr)
         throw InvalidOperationException(HERE, L"suggestion list does not exist");

      // DEBUG:
      Console << L"Inserting suggestion: " << Colour::Yellow << SuggestionsList.GetSelected() << ENDL;

      // Find token at caret
      CommandLexer lex(GetLineText(-1));
      TokenIterator pos = lex.Find(GetCaretIndex());

      // Replace token with suggestion
      if (pos != lex.end())
      {
         SetSel(LineIndex(-1)+pos->Start, LineIndex(-1)+pos->End);
         ReplaceSel(SuggestionsList.GetSelected().c_str(), TRUE);
      }
      else 
         // Error: Failed to identify token
         Console << Colour::Red << "Cannot find suggestion token: " << Colour::White << " caret at " << GetCaretIndex() << " : " << Colour::Yellow << GetLineText(-1) << ENDL;

      // Close
      CloseSuggestions();
   }
   
   /// <summary>Compiles script to highlight errors</summary>
   void ScriptEdit::OnBackgroundCompile()
   {
      CWaitCursor c;
      //Console << L"Background compiler activated" << ENDL;

      // Stop compiler timer
      SetCompilerTimer(false);
        
      // Freeze window
      FreezeWindow(true);

      try 
      { 
         // Parse script 
         ScriptParser parser(Document->Script, GetLines(), Document->Script.Game);
         
         // DEBUG:
         if (!parser.Errors.empty())
            parser.Print();
         Console << L"Background compiler found: " << parser.Errors.size() << L" errors" << ENDL;

         // Define error underline
         CharFormat cf(CFM_COLOR | CFM_UNDERLINE | CFM_UNDERLINETYPE, CFE_UNDERLINE);
         cf.bUnderlineType = CFU_UNDERLINEWAVE;
         cf.bUnderlineColor = 0x02;     //Undocumented underline colour
         cf.crTextColor = RGB(255,0,0);

         // Underline all errors
         for (const auto& err : parser.Errors)
         {
            FormatToken(LineIndex(err.Line-1), err, cf);
            Console << err.Line << L": " << Colour::Yellow << err.Message << Colour::White << L" : " << err.Text << ENDL;
         }

         // Raise 'Compile Complete'
         CompileComplete.Raise();
      }
      catch (ExceptionBase& e) 
      { 
         Console.Log(HERE, e); 
      }

      // UnFreeze window
      FreezeWindow(false);
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
         else if ((SuggestionType = IdentifySuggestion(nChar)) != Suggestion::None)
         {
            //Console << L"Identified " << GetString(SuggestionType) << L" from " << (wchar)nChar << ENDL;
            ShowSuggestions();
         }
      }
      catch (ExceptionBase& e) {
         Console.Log(HERE, e, GuiString(L"Unable to process '%c' character", (wchar)nChar)); 
      }
   }

   /// <summary>Refreshes the line numbers after a scroll</summary>
   void ScriptEdit::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
   {
      // Redraw the line numbers after a scroll or drag
      if (nSBCode == SB_ENDSCROLL)
         RefreshGutter();

      CRichEditCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
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
         if (State == InputState::Suggestions)
            switch (pFilter->msg)
            {
            case WM_CHAR:
               switch (chr)
               {
               // TAB: Insert suggestion.  Prevent focus switch
               case VK_TAB:
                  InsertSuggestion();
                  *pResult = BLOCK_INPUT;
                  break;
            
               // ESCAPE: Close suggestions
               case VK_ESCAPE:
                  CloseSuggestions();
                  *pResult = BLOCK_INPUT;
                  break;
               }
               break;

            // NAVIGATION: Update suggestions / Forward message
            case WM_KEYDOWN:
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
                  SuggestionsList.SendMessage(pFilter->msg, pFilter->wParam, pFilter->lParam);
                  *pResult = BLOCK_INPUT;
                  return;
               }
               break;

            // CLICK: Close Suggestions
            case WM_LBUTTONDOWN:
            case WM_RBUTTONDOWN:
            case WM_MBUTTONDOWN:
               CloseSuggestions();
               break;

            // WHEEL: Forward to list
            case WM_MOUSEWHEEL:
               SuggestionsList.SendMessage(pFilter->msg, pFilter->wParam, pFilter->lParam);
               *pResult = BLOCK_INPUT;
               break;
            }
      }
      catch (ExceptionBase& e) { 
         Console.Log(HERE, e, GuiString(L"Unable to process input filter: message=%d wparam=%d lparam=%d char='%c'",pFilter->msg, pFilter->wParam, pFilter->lParam, chr) ); 
      }
   }

   /// <summary>Updates the suggestion list in response to caret movement</summary>
   /// <param name="nChar">The character.</param>
   /// <param name="nRepCnt">The repeat count.</param>
   /// <param name="nFlags">The flags.</param>
   void ScriptEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
   {
      // Trap Tab/Shift+Tab (but not ctrl+Tab)
      if (nChar == VK_TAB && !HIBYTE(GetKeyState(VK_CONTROL)))
         OnTabKeyDown(HIBYTE(GetKeyState(VK_SHIFT)) != 0);
      else
         CRichEditCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
      
      // Suggestions: Update in response to caret movement
      if (State == InputState::Suggestions)
         try 
         {  
            switch (nChar)
            {
            // TAB/LEFT/RIGHT/HOME/END/DELETE/BACKSPACE: Update current match
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
            Console.Log(HERE, e, GuiString(L"Unable to process '%d' key (char '%c')", nChar, (wchar)nChar)); 
         }
   }

   /// <summary>Closes the suggestion when focus lost</summary>
   /// <param name="pNewWnd">New window.</param>
   void ScriptEdit::OnKillFocus(CWnd* pNewWnd)
   {
      try
      {
         // Close suggestions, unless they're gaining focus
         if (State == InputState::Suggestions && (!pNewWnd || SuggestionsList != *pNewWnd))
            CloseSuggestions();
      }
      catch (ExceptionBase& e) {
         Console.Log(HERE, e); 
      }

      CRichEditCtrl::OnKillFocus(pNewWnd);
   }
   
   /// <summary>Draws the line number gutter</summary>
   void ScriptEdit::OnPaint()
   {
      // Paint window
      CRichEditCtrl::OnPaint();

      // Get drawing dc
      ScriptEditDC dc(this);

      // Get first line number rectangle
      LineRect rect = dc.GetLineRect(GetFirstVisibleLine());

      // Draw all visible line numbers
      while (dc.RectVisible(rect))
      {
         // Draw 1-based line number
         auto s = GuiString(L"%d", 1+rect.LineNumber);
         dc.DrawText(s.c_str(), s.length(), rect, DT_RIGHT);

         // Move to next line
         rect.Advance();
      }
   
   }
   
   /// <summary>Called when tab key pressed.</summary>
   /// <param name="shift">whether SHIFT key is pressed</param>
   void ScriptEdit::OnTabKeyDown(bool shift)
   {
      // No selection: Insert spaces
      if (!HasSelection())
         ReplaceSel(L"    ", TRUE);
      
      // Indent/Outdent
      else 
         IndentSelection(!shift);
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
            case TokenType::Label:        cf.crTextColor = RGB(255,0,255);    break;
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
         Console.Log(HERE, e); 
      }

      // Restore selection
      FreezeWindow(false);
   }
   
   /// <summary>Compiles the current text</summary>
   /// <param name="nIDEvent">The timer identifier.</param>
   void ScriptEdit::OnTimer(UINT_PTR nIDEvent)
   {
      // Background compiler
      if (nIDEvent == COMPILE_TIMER)
         OnBackgroundCompile();

      // Used by RichEdit
      CRichEditCtrl::OnTimer(nIDEvent);
   }
   
   /// <summary>Refreshes the line numbers after a scroll</summary>
   void ScriptEdit::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
   {
      // Redraw the line numbers after a scroll or drag
      if (nSBCode == SB_ENDSCROLL)
         RefreshGutter();

      CRichEditCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
   }

   /// <summary>Matches a token type against the current suggestion type</summary>
   /// <param name="t">The t.</param>
   /// <returns></returns>
   bool ScriptEdit::MatchSuggestionType(Compiler::TokenType t) const
   {
      switch (SuggestionType)
      {
      case Suggestion::Variable:     return t == TokenType::Variable;
      case Suggestion::GameObject:   return t == TokenType::GameObject;
      case Suggestion::ScriptObject: return t == TokenType::ScriptObject;
      case Suggestion::Label:        return t == TokenType::Label;
      case Suggestion::Command:      return t == TokenType::Text;
      }

      return false;
   }
   
   /// <summary>Invalidates the line number gutter.</summary>
   void ScriptEdit::RefreshGutter()
   {
      GutterRect rc(this);
      rc.right += 20;
      InvalidateRect(rc, TRUE);
      UpdateWindow();
   }

   /// <summary>Selects an entire line.</summary>
   /// <param name="line">The zero-based line index, or -1 for current line</param>
   void  ScriptEdit::SelectLine(int line)
   {
      SetSel(GetLineStart(line), GetLineEnd(line));
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

   /// <summary>Sets the width of the line number gutter.</summary>
   /// <param name="width">width in pixels</param>
   void ScriptEdit::SetGutterWidth(UINT width)
   {
      ParaFormat pf(PFM_OFFSET);
      
      // Freeze+select text
      FreezeWindow(true);
      SetSel(0, -1);

      // Calculate width in twips
      ScriptEditDC dc(this);
      pf.dxOffset = dc.TwipsToPixels(width, LOGPIXELSX);

      // Set paragraph formatting for entire text
      SetParaFormat(pf);
      SetSel(0, 0);

      // Restore
      FreezeWindow(false);
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
      if (SuggestionsList.GetSafeHwnd() != nullptr)
         throw InvalidOperationException(HERE, L"suggestion list already exists");

      // Show list
      if (!SuggestionsList.Create(this, GetCharPos(GetSelection().cpMin), SuggestionType, &Document->Script)) 
         throw Win32Exception(HERE, L"Unable to create suggestion list");

      // Update state
      State = InputState::Suggestions;
   }


   /// <summary>Scrolls/closes the suggestion list in response to caret movement or character input</summary>
   void ScriptEdit::UpdateSuggestions()
   {
      // Ensure exists
      if (SuggestionsList.GetSafeHwnd() == nullptr)
         throw InvalidOperationException(HERE, L"suggestion list does not exist");

      // Find token at caret
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
      SuggestionsList.MatchSuggestion(*pos);
   }

   // ------------------------------- PRIVATE METHODS ------------------------------
   


   
NAMESPACE_END2(GUI,Controls)





