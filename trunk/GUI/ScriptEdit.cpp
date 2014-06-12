#include "stdafx.h"
#include "ScriptEdit.h"
#include "RefactorDialog.h"
#include "SymbolDialog.h"
#include "Clipboard.h"
#include "../Logic/CommandLexer.h"
#include "../Logic/SyntaxHighlight.h"
#include "../Logic/IndentationStack.h"

/// <summary>User interface controls</summary>
NAMESPACE_BEGIN2(GUI,Controls)

   // --------------------------------- CONSTANTS ----------------------------------

   /// <summary>Background compiler timer ID</summary>
   const UINT  ScriptEdit::COMPILE_TIMER = 42;

   /// <summary>Used to defer keyboard messages</summary>
   #define UN_CHAR_NOTIFY      (WM_USER+1)
   #define UN_KEYDOWN_NOTIFY   (WM_USER+2)

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNAMIC(ScriptEdit, CRichEditCtrl)

   BEGIN_MESSAGE_MAP(ScriptEdit, CRichEditCtrl)
      ON_WM_TIMER()
      ON_WM_HSCROLL()
      ON_WM_KILLFOCUS()
      ON_WM_PAINT()
      ON_WM_SETFOCUS()
      ON_WM_SETTINGCHANGE()
      ON_WM_VSCROLL()
      ON_WM_HSCROLL_REFLECT()
      ON_WM_VSCROLL_REFLECT()
      ON_CONTROL_REFLECT(EN_CHANGE, &ScriptEdit::OnTextChange)
      ON_NOTIFY_REFLECT(EN_MSGFILTER, &ScriptEdit::OnInputMessage)
      ON_MESSAGE(UN_CHAR_NOTIFY, &ScriptEdit::OnCharNotify)
      ON_MESSAGE(UN_KEYDOWN_NOTIFY, &ScriptEdit::OnKeyDownNotify)
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   ScriptEdit::ScriptEdit() : Document(nullptr), Suggestions(this)
   {
   }

   ScriptEdit::~ScriptEdit()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   /// <summary>Get suggestion type name</summary>
   const wchar*  ScriptEdit::GetString(Suggestion& s) 
   {
      static const wchar* str[] = { L"None", L"Variable", L"Command", L"GameObject", L"ScriptObject", L"Label" };
      return str[(UINT)s];
   }

   // ------------------------------- PUBLIC METHODS -------------------------------
   
   /// <summary>Displays suggestions for the token at the caret.</summary>
   void ScriptEdit::AutoComplete() 
   {
      Suggestions.Display();
   }
      
   /// <summary>Determines whether token at caret has suggestions.</summary>
   /// <returns></returns>
   bool  ScriptEdit::CanAutoComplete() const
   {
      return Suggestions.CanDisplay();
   }
   
   /// <summary>Determines whether current line references a label.</summary>
   /// <returns></returns>
   bool  ScriptEdit::CanGotoLabel() const
   {
      // Require 'Goto' or 'Gosub'
      auto cmd = ScriptParser::Identify(Document->Script, GetLineText(-1));
      return cmd.Is(CMD_GOTO_LABEL) || cmd.Is(CMD_GOTO_SUB);
   }
   
   /// <summary>Determines whether current command has MSCI reference URL.</summary>
   /// <returns></returns>
   bool  ScriptEdit::CanLookupOnline() const
   {
      // Require MSCI reference URL
      auto cmd = ScriptParser::Identify(Document->Script, GetLineText(-1));
      return !cmd.URL.empty();
   }
   
   /// <summary>Determines current line references a script</summary>
   /// <returns></returns>
   bool  ScriptEdit::CanOpenScript() const
   {
      // Require command contain 'script-call' syntax
      auto cmd = ScriptParser::Identify(Document->Script, GetLineText(-1));
      return cmd.IsScriptCall();
   }
   
   /// <summary>Determines whether token at caret can be refactored.</summary>
   /// <returns></returns>
   bool  ScriptEdit::CanRefactor() const
   {
      // Lookup token @ caret
      CommandLexer lex(GetLineText(-1), true);
      TokenIterator pos = lex.Find(GetCaretIndex());

      // Ensure variable/label
      return lex.Valid(pos) && (pos->Type == TokenType::Label || pos->Type == TokenType::Variable);
   }

   /// <summary>Determines current line references a string.</summary>
   /// <returns></returns>
   bool  ScriptEdit::CanViewString() const
   {
      // Require command contain 'stringID/pageID' syntax
      auto cmd = ScriptParser::Identify(Document->Script, GetLineText(-1));
      return cmd.IsStringReference();
   }

   /// <summary>Toggles comment on the selected lines.</summary>
   /// <remarks>Because RichEdit 5.0 doesn't implement undo grouping, this operation is performed by manually reformatting
   /// affected block of text and replacing it in a single operation. Performing line-by-line adjustment is sadly not possible</remarks>
   void  ScriptEdit::CommentSelection()
   {
      GuiString  output;

      // Get first/last line of selection
      LineTextIterator first = sbegin(), last = send();
      
      // Select entire block of lines 
      SetSel(first->Start, last->End);
      FreezeWindow(true);

      // Choose state based on first line
      bool comment = !first->Commented;

      // Get selected lines
      int length = 0;
      for (auto it = first; it <= last; ++it)
      {
         GuiString txt = it->Text;

         // Comment: Add '*' to start of each line
         if (comment && !it->Commented && !it->NOP)
            txt.insert(txt.find_first_not_of(L' '), L"* ");

         // Uncomment: Remove '*' from start of each line
         else if (!comment && it->Commented)
         {
            UINT i = txt.find('*');
            // Erase '*' and any trailing whitespace
            while (i < txt.length() && (txt[i] == '*' || iswspace(txt[i])))
               txt.erase(i, 1);
         }

         // Add to output
         length += txt.length();
         output += txt;

         // CRLF  [except last line]
         if (it != last)
         {
            output += L"\r";
            length++;
         }
      }

      // Replace existing selection
      ReplaceSel(output.c_str(), TRUE);

      // Unfreeze window
      FreezeWindow(false);
      SetSel(first->Start, last->End);

      // Update highlighting
      UpdateHighlighting(first->Line, last->Line);
   }
   
   /// <summary>Formats the selection.</summary>
   void ScriptEdit::FormatDocument()
   {
      IndentationStack stack;
      CWaitCursor      wait;
      wstring          newText;
  
      // Freeze window
      //SuspendUndo(true);
      //FreezeWindow(true);

      try 
      { 
         // Feedback
         Console << Cons::UserAction << "Formatting document " << Document->GetFullPath() << ENDL;

         // Parse script into list of commands
         auto commands = ScriptParser(Document->Script, GetAllLines(), Document->Script.Game).ToList();
         
         // Generate new document text
         for (auto cmd = commands.begin(), end = commands.end(); cmd != end; ++cmd)
         {
            stack.PreDisplay(*cmd);
            newText += (stack.Indentation + (*cmd)->LineCode + L"\r\n");
            stack.PostDisplay(*cmd, commands.IsSubRoutine(cmd));
         }

         // Strip last CRLF
         if (!newText.empty())
            newText.erase(newText.length()-2);

         // Replace entire text
         SetSel(0, -1);
         ReplaceSel(newText.c_str(), TRUE);
      }
      catch (std::exception& e) { 
         Console.Log(HERE, e); 
      }

      // UnFreeze window
      //FreezeWindow(false);
      //SuspendUndo(false);

      // Update highlighting
      UpdateHighlighting(0, GetLineCount()-1);
   }

   /// <summary>Formats the selection.</summary>
   void ScriptEdit::FormatSelection()
   {
      IndentationStack stack;
      CWaitCursor      wait;
      wstring          newText;
  
      try 
      { 
         // Feedback
         Console << Cons::UserAction << "Formatting selection " << Document->GetFullPath() << ENDL;

         // Select entirety of selected lines
         auto first = sbegin(), last = send();
         SetSel(first->Start, last->End);
         //FreezeWindow(true);

         // Parse script into list of commands
         auto commands = ScriptParser(Document->Script, GetAllLines(), Document->Script.Game).ToList();

         // Generate selection text
         int index = 0;
         for (auto cmd = commands.begin(), end = commands.end(); cmd != end; ++cmd)
         {
            stack.PreDisplay(*cmd);

            // Calculate indentation for all lines, but only output relevant lines
            if (index >= first->Line && index <= last->Line)
               newText += (stack.Indentation + (*cmd)->LineCode + L"\r\n");

            // Abort after last line
            else if (index > last->Line)
               break;

            // Calc indent
            stack.PostDisplay(*cmd, commands.IsSubRoutine(cmd));
            ++index;
         }
         // Strip last CRLF
         if (!newText.empty())
            newText.erase(newText.length()-2);

         // Replace selection
         ReplaceSel(newText.c_str(), TRUE);

         // Update window
         //FreezeWindow(false);
         UpdateHighlighting(first->Line, last->Line);
      }
      catch (std::exception& e) 
      { 
         //FreezeWindow(false);
         Console.Log(HERE, e); 
      }
   }
   
   /// <summary>Indents or outdents the selected lines.</summary>
   /// <param name="indent">True to indent, false to outdent.</param>
   void  ScriptEdit::IndentSelection(bool indent)
   {
      GuiString  output;

      // Get first/last line of selection
      LineTextIterator first = sbegin(), last = send();
      
      // Select entire block of lines 
      SetSel(first->Start, last->End);
      FreezeWindow(true);

      // Get selected lines
      int length = 0;
      for (auto it = first; it <= last; ++it)
      {
         GuiString txt = it->Text;

         // Indent: Adding soft-tab to start of each line
         if (indent)
            txt.insert(0, Indent(PrefsLib.ScriptIndentation));

         // Outdent: Remove (up to) one soft-tab from start of each line. If less is available, trim that.
         else if (txt.find_first_not_of(L" \t") != GuiString::npos) 
            txt.erase(0, min(PrefsLib.ScriptIndentation, (int)txt.find_first_not_of(L" \t")));

         // Add to output
         length += txt.length();
         output += txt;

         // CRLF  [except last line]
         if (it != last)
         {
            output += L"\r";
            length++;
         }
      }

      // Replace existing selection
      ReplaceSel(output.c_str(), TRUE);

      // Unfreeze window
      FreezeWindow(false);
      SetSel(first->Start, last->End);

      // Highlight affected text
      UpdateHighlighting(first->Line, last->Line);
   }


   /// <summary>Initializes the control</summary>
   /// <param name="doc">The document.</param>
   /// <exception cref="Logic::ArgumentNullException">document is null</exception>
   /// <exception cref="Logic::Win32Exception">Unable to retrieve COM pointers</exception>
   void  ScriptEdit::Initialize(ScriptDocument* doc)
   {
      REQUIRED(doc);

      // Set document
      Document = doc;
      
      // Initialize
      RichEditEx::Initialize(RGB(0,0,0));

      // Setup tooltip
      AdjustTooltip(PrefsLib.ShowScriptTooltips, PrefsLib.ScriptTooltipDelay*1000, 30*1000);

      // Set default character format
      SetDefaultCharFormat(CharFormat(PrefsLib.ScriptViewFont, this));

      // Adjust gutter
      SetGutterWidth(GutterRect(this).Width());

      // Listen: Argument changed
      fnArgumentChanged = Document->ArgumentChanged.Register(this, &ScriptEdit::OnArgumentChanged);
   }

   /// <summary>Replace entire contents with plain text.</summary>
   /// <param name="txt">text.</param>
   /// <exception cref="Logic::InvalidOperationException">No document attached</exception>
   void ScriptEdit::SetPlainText(const wstring& txt)
   {
      // Require document
      if (Document == nullptr)
         throw InvalidOperationException(HERE, L"Must attach document prior to displaying text");

      // Set text
      SetWindowText(txt.c_str());

      // Set default character format
      SetDefaultCharFormat(CharFormat(PrefsLib.ScriptViewFont, this));

      // Adjust gutter
      SetGutterWidth(GutterRect(this).Width());

      // Update highlighting
      UpdateHighlighting(0, GetLineCount()-1);
      EnsureVisible(0);

      // Reset Undo
      EmptyUndoBuffer();
   }

   /// <summary>Replace entire contents with RTF.</summary>
   /// <param name="rtf">The RTF.</param>
   /// <exception cref="Logic::InvalidOperationException">No document attached</exception>
   /// <exception cref="Logic::Win32Exception">Unable to set text</exception>
   void ScriptEdit::SetRtf(const string& rtf)
   {
      // Require document
      if (Document == nullptr)
         throw InvalidOperationException(HERE, L"Must attach document prior to displaying text");

      // Set RTF
      RichEditEx::SetRtf(rtf);

      // Set default character format
      SetDefaultCharFormat(CharFormat(PrefsLib.ScriptViewFont, this));

      // Adjust gutter
      SetGutterWidth(GutterRect(this).Width());
      EnsureVisible(0);

      // Reset Undo
      EmptyUndoBuffer();
   }

   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Get line iterator for the specified line.</summary>
   /// <param name="line">Zero-based line number</param>
   /// <returns></returns>
   ScriptEdit::LineTextIterator  ScriptEdit::begin(int line)
   { 
      return LineTextIterator(*this, line);            
   }
   
   /// <summary>Get line iterator for the first line in the text selection, if any, otherwise the line containing the caret</summary>
   /// <returns></returns>
   ScriptEdit::LineTextIterator  ScriptEdit::sbegin() 
   { 
      return LineTextIterator(*this, HasSelection() ? LineFromChar(GetSelection().cpMin) : LineFromChar(-1)); 
   }
   
   /// <summary>Get line iterator for the last line.</summary>
   /// <returns></returns>
   ScriptEdit::LineTextIterator  ScriptEdit::end() 
   { 
      return LineTextIterator(*this, GetLineCount()-1); 
   }

   /// <summary>Get line iterator for the last line in the text selection, if any, otherwise the line containing the caret</summary>
   /// <returns></returns>
   ScriptEdit::LineTextIterator  ScriptEdit::send() 
   { 
      return LineTextIterator(*this, HasSelection() ? LineFromChar(GetSelection().cpMax) : LineFromChar(-1)); 
   }

   /// <summary>Selects and formats a token.</summary>
   /// <param name="offset">The character index of the line</param>
   /// <param name="t">The token</param>
   /// <param name="cf">The formatting characteristics</param>
   void  ScriptEdit::FormatToken(UINT offset, const TokenBase& t, CharFormat& cf)
   {
      SetSel(offset+t.Start, offset+t.End);
      SetSelectionCharFormat(cf);
   }
   
   /// <summary>Freezes or unfreezes the window.</summary>
   /// <param name="freeze">True to freeze, false to restore</param>
   /// <param name="invalidate">True to invalidate after unfreezing</param>
   void ScriptEdit::FreezeWindow(bool freeze, bool invalidate)
   {
      // Freeze/Restore window
      __super::FreezeWindow(freeze, invalidate);
         
      // Freeze/Restore suggestions list
      Suggestions.FreezeWindow(freeze, invalidate);
   }
   
   /// <summary>Determines whether document is connected</summary>
   /// <returns></returns>
   bool  ScriptEdit::HasDocument() const
   {
      return Document != nullptr;
   }
   
   /// <summary>Determines whether key is pressed</summary>
   /// <param name="vKey">Virtual key.</param>
   /// <returns></returns>
   bool ScriptEdit::IsKeyPressed(UINT vKey) const
   {
      return HIBYTE(GetKeyState(vKey)) != 0;
   }
   
   /// <summary>Query whether to allow a keyboard accelerator.</summary>
   /// <param name="ch">character.</param>
   /// <returns>True to allow, False to block</returns>
   bool ScriptEdit::OnAccelerator(wchar ch)
   {
      switch (ch)
      {
      // ALIGNMENT
      case L'L':
      case L'R':   
      case L'J':
      case L'E':
      // SUPERSCRIPT
      case 0xBB:  // Received instead of '+/=' key for some reason
      case L'=':
      case L'+':
      // LINESPACING
      case L'1':
      case L'2':
      case L'5':
         //Console << "Received shortcut key " << ch << " action=block" << ENDL;
         return false;
      }
      
      //Console << "Received shortcut key " << ch << " (" << (void*)ch << ") action=allow" << ENDL;
      return true;
   }
   
   /// <summary>Refresh entire document.</summary>
   void ScriptEdit::OnArgumentChanged()
   {
      UpdateHighlighting(0, GetLineCount()-1);
   }

   /// <summary>Compiles script to highlight errors</summary>
   void ScriptEdit::OnBackgroundCompile()
   {
      CWaitCursor c;

      // Stop compiler timer
      SetCompilerTimer(false);
        
      // Freeze window
      SuspendUndo(true);
      FreezeWindow(true);

      try 
      { 
         // Feedback
         Console << Cons::Heading << "Background compiling " << Document->GetFullPath() << ENDL;

         // Parse script 
         ScriptParser parser(Document->Script, GetAllLines(), Document->Script.Game);
         
         // DEBUG:
         /*if (!parser.Errors.empty())
            parser.Print();*/
         Console << L"Background compiler found: " << parser.Errors.size() << L" errors" << ENDL;

         // Underlining Errors?
         if (PrefsLib.BackgroundCompiler)
         {
            // Clear previous underlining
            CharFormat cf(CFM_UNDERLINE|CFM_UNDERLINETYPE, NULL);
            SetSel(0,-1);
            SetSelectionCharFormat(cf);

            // Define error underline
            cf = CharFormat(CFM_COLOR|CFM_UNDERLINE|CFM_UNDERLINETYPE, CFE_UNDERLINE);
            cf.bUnderlineType = CFU_UNDERLINEWAVE;
            //cf.bUnderlineColor = 0x02;     //Undocumented underline colour
            cf.crTextColor = PrefsLib.ErrorHighlight;

            // Underline all errors
            for (const auto& err : parser.Errors)
            {
               FormatToken(LineIndex(err.Line-1), err, cf);
               Console << err.Line << L": " << Cons::Yellow << err.Message << Cons::White << L" : " << err.Text << ENDL;
            }
         }

         // Raise 'Compile Complete'
         CompileComplete.Raise();
      }
      catch (std::exception& e) 
      { 
         Console.Log(HERE, e); 
      }

      // UnFreeze window
      FreezeWindow(false);
      SuspendUndo(false);
   }

   /// <summary>Notifies the suggestion mediator of character input</summary>
   /// <param name="nChar">The character.</param>
   /// <param name="nRepCnt">The repeat count.</param>
   /// <param name="nFlags">The flags.</param>
   void ScriptEdit::OnCharInternal(UINT nChar, UINT nRepCnt, UINT nFlags)
   {
      // DEBUG: Console << "ScriptEdit::OnCharInternal" << " nChar=" << (wchar)nChar << ENDL;

      // Notify suggestion mediator
      Suggestions.OnChar(nChar, nRepCnt, nFlags);
   }
   
   /// <summary>Called after a character message</summary>
   /// <param name="wParam">The w parameter.</param>
   /// <param name="lParam">The l parameter.</param>
   /// <returns></returns>
   LRESULT ScriptEdit::OnCharNotify(WPARAM wParam, LPARAM lParam)
   {
      OnCharInternal(wParam, LOWORD(lParam), HIWORD(lParam));
      return 0;
   }

   /// <summary>Called when ENTER is pressed. Inserts a newline with appropriate indentation</summary>
   void ScriptEdit::OnCharNewLine()
   {
      //GroupUndo(true);  Not implemented

      // Insert newline
      ReplaceSel(L"\n", TRUE);

      // Get line now preceeding the caret
      LineProxy line(*this, LineFromChar(-1) - 1);

      // Insert similar amount of indentation
      ReplaceSel(Indent(line.Indent).c_str(), TRUE);

      //GroupUndo(false);  Not implemented
   }

   /// <summary>Called when Tab or ShiftTab is pressed. Replaces hard tabs with spaces.</summary>
   /// <param name="shift">whether SHIFT key is pressed</param>
   void ScriptEdit::OnCharTab(bool shift)
   {
      // No selection: Insert spaces
      if (!HasSelection())
         ReplaceSel(L"    ", TRUE);
      
      // Indent/Outdent
      else 
         IndentSelection(!shift);
   }

   /// <summary>Refreshes the line numbers after a scroll</summary>
   void ScriptEdit::HScroll(UINT nSBCode, UINT nPos)
   {
      //Console << "ScriptEdit::HScroll" << ENDL;

      // Redraw the line numbers after a scroll or drag
      if (nSBCode == SB_ENDSCROLL)
         RefreshGutter();

      // Reset tooltip
      __super::HScroll(nSBCode, nPos);
   }
   
   /// <summary>Refreshes the line numbers after a scroll</summary>
   void ScriptEdit::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* bar)
   {
      //Console << "ScriptEdit::OnHScroll" << ENDL;

      // Redraw the line numbers after a scroll or drag
      if (nSBCode == SB_ENDSCROLL)
         RefreshGutter();

      // Reset tooltip
      __super::OnHScroll(nSBCode, nPos, bar);
   }
   
   /// <summary>Offers mouse/keyboard messages to suggestions mediator or self before passing to RichEdit</summary>
   /// <param name="pNMHDR">The notify header</param>
   /// <param name="pResult">message result</param>
   void ScriptEdit::OnInputMessage(NMHDR *pNMHDR, LRESULT *pResult)
   {
      static const UINT ALLOW_INPUT = 0, BLOCK_INPUT = 1;

      // Prepare
      MSGFILTER* mf = reinterpret_cast<MSGFILTER*>(pNMHDR);

      // Reset tooltip
      __super::OnInputMessage(pNMHDR, pResult);    // NB: Allows all input

      // Give suggestions mediator first chance
      if (Suggestions.WantMessage(mf->msg, mf->wParam, mf->lParam))
         *pResult = BLOCK_INPUT;

      // Give self second chance
      else if (WantMessage(mf->msg, mf->wParam, mf->lParam))
         *pResult = BLOCK_INPUT;

      else
      {
         *pResult = ALLOW_INPUT;
         
         // Re-Dispatch to self
         switch (mf->msg)
         {
         case WM_CHAR:    PostMessage(UN_CHAR_NOTIFY, mf->wParam, mf->lParam);     break;
         case WM_KEYDOWN: PostMessage(UN_KEYDOWN_NOTIFY, mf->wParam, mf->lParam);  break;
         }
      }
   }

   /// <summary>Notifies the suggestion mediator of keystrokes</summary>
   /// <param name="nChar">The character.</param>
   /// <param name="nRepCnt">The repeat count.</param>
   /// <param name="nFlags">The flags.</param>
   void ScriptEdit::OnKeyDownInternal(UINT nChar, UINT nRepCnt, UINT nFlags)
   {
      try
      {
         // DEBUG: Console << "ScriptEdit::OnKeyDownInternal" << " nChar=" << (wchar)nChar << ENDL;

         switch (nChar)
         {
         // HOME: Scroll to edge of window, not edge of gutter paragraph
         case VK_HOME:
            SendMessage(WM_HSCROLL, SB_LEFT, 0);
            break;
         }

         // Notify suggestions
         Suggestions.OnKeyDown(nChar, nRepCnt, nFlags);
      } 
      catch (ExceptionBase& e) {
         Console.Log(HERE, e, VString(L"Unable to process '%d' key (char '%c')", nChar, (wchar)nChar)); 
      }
   }
   
   /// <summary>Called after a key down message</summary>
   /// <param name="wParam">The w parameter.</param>
   /// <param name="lParam">The l parameter.</param>
   /// <returns></returns>
   LRESULT ScriptEdit::OnKeyDownNotify(WPARAM wParam, LPARAM lParam)
   {
      OnKeyDownInternal(wParam, LOWORD(lParam), HIWORD(lParam));
      return 0;
   }

   /// <summary>Closes the suggestion when focus lost</summary>
   /// <param name="pNewWnd">New window.</param>
   void ScriptEdit::OnKillFocus(CWnd* pNewWnd)
   {
      // Delegate to suggestions mediator
      Suggestions.OnKillFocus(pNewWnd);

      // Deactivate tooltip
      __super::OnKillFocus(pNewWnd);
   }
   
   /// <summary>Draws the line number gutter</summary>
   void ScriptEdit::OnPaint()
   {
      // Paint window
      __super::OnPaint();

      // Show Line Numbers?
      if (!PrefsLib.ShowLineNumbers)
         return;
      
      // Get drawing dc
      ScriptEditDC dc(this, L"Arial");
      dc.SetTextColor(PrefsLib.LineNumberColour);

      // Get first line number rectangle
      LineRect rect = dc.GetLineRect(GetFirstVisibleLine());

      // Draw all visible line numbers
      while (dc.RectVisible(rect))
      {
         // Draw 1-based line number
         auto s = VString(L"%d", 1+rect.LineNumber);
         dc.DrawText(s.c_str(), s.length(), rect, DT_RIGHT|DT_BOTTOM|DT_SINGLELINE);

         // Move to next line
         rect.Advance();
      }
   
   }
   

   /// <summary>Supply tooltip data</summary>
   /// <param name="data">The data.</param>
   void ScriptEdit::OnRequestTooltip(CustomTooltip::TooltipData* data)
   {
      // Initialise
      auto cursor = GetCursorLocation();
      auto line = GetLineText(cursor.y);
      
      // Find token beneath cursor
      CommandLexer lex(line);
      auto tok = lex.Tokens.Find(cursor.x);
      
      // None: show nothing
      if (!tok || ReadOnly)
         data->Cancel();
      
      // Valid: Provide approriate data
      else switch (tok->Type)
      {
      case TokenType::Text:         data->ResetTo(CommandTooltipData(Document->Script, line));   break;
      case TokenType::Label:        data->ResetTo(LabelTooltipData(*this, tok->ValueText));      break;
      case TokenType::ScriptObject: data->ResetTo(ScriptObjectTooltipData(tok->ValueText));      break;

      default: data->Cancel(); break;
      }  
   }
   
   /// <summary>Updates font and syntax highlighting.</summary>
   /// <param name="uFlags">The flags.</param>
   /// <param name="lpszSection">The section.</param>
   void ScriptEdit::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
   {
      // Suspend
      SuspendUndo(true);
      FreezeWindow(true);
         
      // Change font of all text
      SetSel(0, -1);
      SetSelectionCharFormat(CharFormat(PrefsLib.ScriptViewFont, this));
      
      // Restore
      FreezeWindow(false);
      SuspendUndo(false);

      // Adjust gutter
      SetGutterWidth(GutterRect(this).Width());

      // Update all highlighting
      SetBackgroundColor(FALSE, PrefsLib.BackgroundColour);
      UpdateHighlighting(0, GetLineCount()-1);

      // Update Tooltip
      AdjustTooltip(PrefsLib.ShowScriptTooltips, PrefsLib.ScriptTooltipDelay*1000, 30*1000);
      __super::OnSettingChange(uFlags, lpszSection);
   }
   
   /// <summary>Performs syntax colouring on the current line</summary>
   void ScriptEdit::OnTextChange()
   {
      // Set/Reset background compiler timer
      if (!ReadOnly)
         SetCompilerTimer(true);

      // Update current line
      UpdateHighlighting(-1, -1);

      // Reset tooltip  (Raises 'TEXT CHANGED')
      __super::OnTextChange();
   }
   
   /// <summary>Compiles the current text</summary>
   /// <param name="nIDEvent">The timer identifier.</param>
   void ScriptEdit::OnTimer(UINT_PTR nIDEvent)
   {
      // Background compiler
      if (nIDEvent == COMPILE_TIMER) 
         OnBackgroundCompile();

      // Used by RichEdit
      __super::OnTimer(nIDEvent);
   }
   
   /// <summary>Refreshes the line numbers after a scroll</summary>
   void ScriptEdit::VScroll(UINT nSBCode, UINT nPos)
   {
      //Console << "ScriptEdit::VScroll" << ENDL;

      // Redraw the line numbers after a scroll or drag
      if (nSBCode == SB_ENDSCROLL)
         RefreshGutter();

      // Reset Tooltip + Scroll
      __super::VScroll(nSBCode, nPos);
   }
   
   /// <summary>Refreshes the line numbers after a scroll</summary>
   void ScriptEdit::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* bar)
   {
      //Console << "ScriptEdit::OnVScroll" << ENDL;

      // Redraw the line numbers after a scroll or drag
      if (nSBCode == SB_ENDSCROLL)
         RefreshGutter();

      // Reset Tooltip + Scroll
      __super::OnVScroll(nSBCode, nPos, bar);
   }

   /// <summary>Pastes text from clipboard.</summary>
   /// <param name="nClipFormat">The clipboard format.</param>
   void  ScriptEdit::PasteFormat(UINT nClipFormat)
   {
      auto prevLine = LineFromChar(-1);

      // LanguageString: Insert text  [Clipboard CF_UNICODETEXT contains the XML equivlent, we want plaintext]
      if (theClipboard.HasLanguageString())     
         ReplaceSel(theClipboard.GetLanguageString().Text.c_str(), TRUE);     //if (IsClipboardFormatAvailable(Clipboard::CF_LANGUAGE_STRING))

      // UnicodeText: Paste normally
      else if (CanPaste(nClipFormat))
         PasteSpecial(nClipFormat);

      // DEBUG:
      //Console << "inital line=" << prevLine << " newLine=" << LineFromChar(-1) << ENDL;

      // Highlight pasted text
      UpdateHighlighting(prevLine, LineFromChar(-1));
   }

   /// <summary>Relay mouse events to tooltip</summary>
   /// <param name="pMsg">MSG.</param>
   /// <returns></returns>
   BOOL ScriptEdit::PreTranslateMessage(MSG* pMsg)
   {
      static UINT BLOCK_MSG = 1, ALLOW_MSG = 0;

      // Block undesireable shortcut keys
      if(pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST)
         if (IsKeyPressed(VK_CONTROL) && !OnAccelerator((wchar)pMsg->wParam))
            return BLOCK_MSG;

      // Default
      return __super::PreTranslateMessage(pMsg);
   }

   
   /// <summary>Refactors the symbol at the caret.</summary>
   void ScriptEdit::Refactor()
   {
      SymbolList matches;
      list<TextRangePtr> tokens;
      
      try
      {
         // Validate
         if (!CanRefactor())
            return;

         // Lookup token @ caret
         CommandLexer  lex(GetLineText(-1), true);
         TokenIterator symbol = lex.Find(GetCaretIndex());

         // Identify type
         SymbolType type = (symbol->Type == TokenType::Variable ? SymbolType::Variable : SymbolType::Label);

         // Display 'rename symbol' dialog
         SymbolDialog symbolDlg(symbol->ValueText, type, this);
         if (symbolDlg.DoModal() == IDCANCEL)
            return;

         // Feedback
         Console << Cons::UserAction << "Refactoring " << Cons::Yellow << symbol->Text 
                 << Cons::White << " into " << Cons::Yellow << symbolDlg.RenameText;

         // Find all matches
         ScriptParser parser(Document->Script, GetAllLines(), Document->Script.Game);
         parser.FindAll(symbol->ValueText, type, matches);

         // Display refactor dialog
         RefactorDialog refactorDlg(*Document, GetAllText(), matches, this);
         if (symbolDlg.Preview && refactorDlg.DoModal() == IDCANCEL)
         {
            Console << Cons::UserAction << "Refactoring aborted" << ENDL;
            return;
         }

         // Generate text ranges for all replacements
         for (auto& m : symbolDlg.Preview ? refactorDlg.Accepted : matches)
         {
            auto offset = LineIndex(m.LineNumber-1);
            tokens.push_back( TextDocument->Range(offset+m.Token.Start, offset+m.Token.End) );
         }

         // Perform replacements
         for (auto& tok : tokens)
            tok->Text = symbolDlg.RenameText.c_str();
      }
      catch (ExceptionBase& e) {
         theApp.ShowError(HERE, e);
      }
   }

   /// <summary>Invalidates the line number gutter.</summary>
   void ScriptEdit::RefreshGutter()
   {
      GutterRect rc(this);
      rc.right += 20;
      InvalidateRect(rc, TRUE);
      UpdateWindow();
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
      ParaFormat pf(PFM_OFFSET|PFM_STARTINDENT);
      
      // Show Line Numbers?
      if (!PrefsLib.ShowLineNumbers)
         width = 0;

      // Freeze+select text
      SuspendUndo(true);
      FreezeWindow(true);
      SetSel(0, -1);

      // Calculate width in twips
      ScriptEditDC dc(this);
      pf.dxOffset = 0;
      pf.dxStartIndent = dc.PixelsToTwips(width+10, LOGPIXELSX);

      // Set paragraph formatting for entire text
      SetParaFormat(pf);
      SetSel(0, 0);

      // Restore
      FreezeWindow(false);
      SuspendUndo(false);
   }

   /// <summary>Updates the highlighting.</summary>
   /// <param name="first">first zero-based line number</param>
   /// <param name="last">last zero-based line number.</param>
   void ScriptEdit::UpdateHighlighting(int first, int last)
   {
      // Freeze window
      SuspendUndo(true);
      FreezeWindow(true);

      try 
      {
         SyntaxHighlight  Highlights;
         CharFormat cf(CFM_COLOR | CFM_UNDERLINE | CFM_UNDERLINETYPE, NULL);
         
         // Caret:
         if (first == -1)
            first = last = LineFromChar(-1);

         // Format lines
         for (int i = first; i <= last; i++)
         {
            UINT offset = LineIndex(i);

            // Lex current line
            CommandLexer lex(GetLineText(i));

            // Format tokens
            for (const auto& tok : lex.Tokens)
            {
               cf.crTextColor = Highlights.GetColour(Document->Script, tok);
               FormatToken(offset, tok, cf);
            }
         }
      }
      catch (ExceptionBase& e) { 
         Console.Log(HERE, e); 
      }

      // Restore selection
      FreezeWindow(false);
      SuspendUndo(false);
   }
   
   /// <summary>Query whether keyboard message should be intercepted</summary>
   /// <param name="msg">Keyboard message</param>
   /// <param name="wParam">parameter</param>
   /// <param name="lParam">parameter</param>
   /// <returns>True to block input message, False to allow</returns>
   bool ScriptEdit::WantMessage(UINT msg, WPARAM wParam, LPARAM lParam)
   {
      wchar chr = static_cast<wchar>(wParam);

      try
      {
         switch (msg)
         {
         // CHARACTERS:
         case WM_CHAR:     // DEBUG: Console << "ScriptEdit::WantMessage" << " msg=WM_CHAR chr=" << chr << ENDL;
            switch (chr)
            {
            // TAB(+SHIFT): Invoke indentation handler   
            case VK_TAB:
               if (IsKeyPressed(VK_CONTROL)) // exclude Ctrl+Tab 'switch document'
                  return false;

               OnCharTab(IsKeyPressed(VK_SHIFT));
               return true;

            // ENTER: Invoke newline indentation handler
            case VK_RETURN: 
               OnCharNewLine();
               return true;
            }
            break;

         // KEYSTROKES: 
         case WM_KEYDOWN:     // DEBUG: Console << "ScriptEdit::WantMessage" << " msg=WM_KEYDOWN chr=" << chr << ENDL;
            switch (chr)
            {
            // TAB/ESCAPE: Block keystroke
            case VK_TAB:
            case VK_RETURN:
               return true;
            }
            break;
         }

         return false;
      }
      catch (ExceptionBase& e) { 
         Console.Log(HERE, e, VString(L"Unable to process input filter: message=%d wparam=%d lparam=%d char='%c'", msg, wParam, lParam, chr) ); 
         return false;
      }
   }
      
   // ------------------------------- PRIVATE METHODS ------------------------------
   


   
NAMESPACE_END2(GUI,Controls)



