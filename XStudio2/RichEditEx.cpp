#include "stdafx.h"
#include "RichEditEx.h"
#include "Logic/CommandLexer.h"

/// <summary>User interface controls</summary>
NAMESPACE_BEGIN2(GUI,Controls)

   // --------------------------------- CONSTANTS ----------------------------------

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNAMIC(RichEditEx, CRichEditCtrl)

   BEGIN_MESSAGE_MAP(RichEditEx, CRichEditCtrl)
      ON_WM_HSCROLL_REFLECT()
      ON_WM_KILLFOCUS()
      ON_WM_SETFOCUS()
      ON_WM_VSCROLL_REFLECT()
      ON_CONTROL_REFLECT(EN_CHANGE, &RichEditEx::OnTextChange)
      ON_NOTIFY_REFLECT(EN_MSGFILTER, &RichEditEx::OnInputMessage)
      ON_NOTIFY_REFLECT(EN_PROTECTED, &RichEditEx::OnProtectedMessage)
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   RichEditEx::RichEditEx() : fnShowTooltip(Tooltip.RequestData.Register(this, &RichEditEx::OnRequestTooltip))
   {
   }

   RichEditEx::~RichEditEx()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   /// <summary>Get undo operation name</summary>
   const wchar*  GetString(UNDONAMEID id)
   {
      switch (id)
      {
      case UID_UNKNOWN:    return L"Last Action";
      case UID_TYPING:     return L"Typing";
      case UID_DELETE:     return L"Delete";
      case UID_DRAGDROP:   return L"Drag n Drop";
      case UID_CUT:        return L"Cut";
      case UID_PASTE:      return L"Paste";
      //case UID_AUTOTABLE:  return L"Table";
      }
      return L"Invalid";
   }


   // ------------------------------- PUBLIC METHODS -------------------------------

   
   /// <summary>Ensures a line is visible.</summary>
   /// <param name="line">1-based line number</param>
   /// <returns></returns>
   bool  RichEditEx::EnsureVisible(int line)
   {
      SetScrollCoordinates(CPoint(0, max(0,line)));
      return true;
   }
   
   /// <summary>Finds and highlights the next match, if any</summary>
   /// <param name="start">starting offset</param>
   /// <param name="m">Match data</param>
   /// <returns>True if found, false otherwise</returns>
   /// <exception cref="Logic::ComException">Text Object Model error</<exception>
   bool  RichEditEx::FindNext(UINT start, MatchData& m) const
   {
      try
      {
         // Selection: Validate search position
         if (m.Target == SearchTarget::Selection && (start < (UINT)m.RangeStart || start >= (UINT)m.RangeEnd))
            return false;

         // Get search range  (Use entire document if not searching selection)
         TextRangePtr limits = TextDocument->Range(start, m.Target != SearchTarget::Selection ? GetTextLength() : m.RangeEnd);
         
         // Convert TOM search flags
         UINT flags = (m.MatchCase ? tomMatchCase : 0) | (m.MatchWord ? tomMatchWord : 0) | (m.UseRegEx ? tomMatchPattern : 0);

         // Find next match
         TextRangePtr match(limits->Duplicate);
         if (!match->FindText(m.SearchTerm.c_str(), tomForward, flags) || match->InRange(limits) != tomTrue)
         {
            // Clear match/selection
            const_cast<RichEditEx*>(this)->SetSel(m.End, m.End);
            m.Clear();
            return false;
         }

         // Set match location
         auto line = LineFromChar(match->Start);
         m.SetMatch(match->Start, match->End - match->Start, line, GetLineTextEx(line));

         // Select text
         match->Select();
         return true;
      }
      catch (_com_error& e) {
         throw ComException(HERE, e);
      }
   }

   /// <summary>Gets entire script as a string delimited by single char (vertical tab) line breaks.</summary>
   /// <returns></returns>
   wstring  RichEditEx::GetAllText() const
   {
      CStringW str;
      // Get entire text
      GetTextRange(0, GetTextLength(), str);
      return str.GetString();
   }
   
   /// <summary>Gets the character index of the caret with respect to the current line.</summary>
   /// <returns></returns>
   int RichEditEx::GetCaretIndex() const
   {
      return GetSelection().cpMin - LineIndex(-1);
   }

   /// <summary>Gets the caret position.</summary>
   /// <returns>Zero-based character index, 1-based line index</returns>
   POINT RichEditEx::GetCaretLocation() const
   {
      return POINT {GetCaretIndex(), 1+LineFromChar(-1)};
   }

   /// <summary>Gets the length of the line.</summary>
   /// <param name="line">The zero-based line index, or -1 for current line</param>
   /// <returns></returns>
   int RichEditEx::GetLineLength(int line) const
   {
      return CRichEditCtrl::LineLength(LineIndex(line));
   }
   
   /// <summary>Get line text</summary>
   /// <param name="line">zero based line index, or -1 for current line</param>
   /// <returns></returns>
   wstring RichEditEx::GetLineText(int line) const
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
   
   /// <summary>Get line text without indentation</summary>
   /// <param name="line">zero based line index, or -1 for current line</param>
   /// <returns></returns>
   GuiString RichEditEx::GetLineTextEx(int line) const
   {
      return GuiString(GetLineText(line)).TrimLeft(L" \t");
   }
   
   /// <summary>Gets entire script as array of plain-text lines.</summary>
   /// <returns></returns>
   LineArray RichEditEx::GetLines() const
   {
      LineArray lines; 
      // Get lines
      for (int i = 0; i < GetLineCount(); i++)
         lines.push_back(GetLineText(i));
      return lines;   
   }
   
   /// <summary>Gets the co-ordinates of the current selection</summary>
   /// <returns></returns>
   CHARRANGE RichEditEx::GetSelection() const
   {
      CHARRANGE sel;
      CRichEditCtrl::GetSel(sel);
      return sel;
   }

   /// <summary>Determines whether text selection exists</summary>
   /// <returns></returns>
   bool RichEditEx::HasSelection() const
   {
      auto sel = GetSelection();
      return sel.cpMin != sel.cpMax;
   }

   /// <summary>Query whether set to read-only.</summary>
   /// <returns></returns>
   bool RichEditEx::IsReadOnly() const
   {
      return (GetOptions() & ECO_READONLY) != 0;
   }

   /// <summary>Gets the length of the line by character index.</summary>
   /// <param name="nChar">The zero-based character index, or -1 for current line</param>
   /// <returns></returns>
   int RichEditEx::LineLength(int nChar) const
   {
      return CRichEditCtrl::LineLength(nChar);
   }


   /// <summary>Initializes the control</summary>
   /// <param name="bk">Background colour</param>
   /// <exception cref="Logic::Win32Exception">Unable to retrieve COM pointers</exception>
   void  RichEditEx::Initialize(COLORREF bk)
   {
      // Set background colour
      SetBackgroundColor(FALSE, bk);

      // Notify on change/scroll/input/paste
      SetEventMask(ENM_CHANGE | ENM_SELCHANGE | ENM_SCROLL | ENM_KEYEVENTS | ENM_MOUSEEVENTS | ENM_PROTECTED);

      // Set undo limit + options
      SetTextMode(TM_RICHTEXT | TM_MULTILEVELUNDO | TM_MULTICODEPAGE);
      SetUndoLimit(100);

      // Extend character limit
      LimitText(256*1024);

      // Create tooltip
      Tooltip.Create(this, this);

      // Get IRichEditOle interface
      IRichEditOlePtr edit(GetIRichEditOle(), false);
      if (!edit)
         throw Win32Exception(HERE, L"Unable to get IRichEditOle interface");
      
      // Get ITextDocument interface
      TextDocument = edit;
      if (!TextDocument)
         throw Win32Exception(HERE, L"Unable to get ITextDocument interface");
   }

   /// <summary>Relay mouse events to tooltip</summary>
   /// <param name="pMsg">MSG.</param>
   /// <returns></returns>
   BOOL RichEditEx::PreTranslateMessage(MSG* pMsg)
   {
      // Relay mouse messages to tooltip
      if(pMsg->message== WM_LBUTTONDOWN ||
         pMsg->message== WM_LBUTTONUP ||
         pMsg->message== WM_MOUSEMOVE)
      {
         Tooltip.RelayEvent(pMsg);
      }

      return __super::PreTranslateMessage(pMsg);
   }

   /// <summary>Replaces the current match</summary>
   /// <param name="m">Match data</param>
   /// <returns>True if replaced, false if match was no longer selected</returns>
   /// <exception cref="Logic::ComException">Text Object Model error</<exception>
   bool  RichEditEx::Replace(MatchData& m)
   {
      try
      {
         // Do nothing if match no longer selected
         if (!m.Compare(GetSelection()))
            return false;

         // Selection: Preserve text selection range
         TextRangePtr range = TextDocument->Range(m.RangeStart, m.RangeEnd);

         // RegEx: Format replacement using regEx
         if (m.UseRegEx)
         {
            wsmatch matches;
            wstring text = (const wchar*)GetSelText();
         
            // Format replacement
            if (regex_match(text, matches, m.RegEx))
               ReplaceSel(matches.format(m.ReplaceTerm).c_str(), TRUE);
         }
         else
            // Basic: Replace selection
            ReplaceSel(m.ReplaceTerm.c_str(), TRUE);

         // Selection: Use TOM to maintain the text selection range
         if (m.Target == SearchTarget::Selection)
            m.UpdateRange(range->Start, range->End);
      
         // Re-supply line text (for feedback)
         m.UpdateLineText(GetLineTextEx(m.LineNumber-1));
         return true;
      }
      catch (_com_error& e) {
         throw ComException(HERE, e);
      }
   }

   /// <summary>Replace entire contents with RTF.</summary>
   /// <param name="rtf">The RTF.</param>
   /// <exception cref="Logic::Win32Exception">Unable to set text</exception>
   void RichEditEx::SetRtf(const string& rtf)
   {
      try
      {
         FreezeWindow(true);

         // Replace contents with RTF
         SETTEXTEX opt = {ST_DEFAULT, CP_ACP};
         if (!SendMessage(EM_SETTEXTEX, (WPARAM)&opt, (LPARAM)rtf.c_str()))
            throw Win32Exception(HERE, L"Unable to set script text");

         // Protect text
         CharFormat cf(CFM_PROTECTED, CFE_PROTECTED);
         SendMessage(EM_SETCHARFORMAT, SCF_ALL, (LPARAM)(CHARFORMAT*)&cf);

         // Clear 'Undo' buffer
         EmptyUndoBuffer();

         // Reset tooltip
         Tooltip.Reset();

         FreezeWindow(false);
      }
      catch (ExceptionBase&) {
         FreezeWindow(false);
         throw;
      }

      
   }

   /// <summary>Suspend or resumes undo buffer</summary>
   /// <param name="suspend">enable/disable</param>
   void  RichEditEx::SuspendUndo(bool suspend)
   {
      try
      {
         TextDocument->Undo(suspend ? tomSuspend : tomResume);
      }
      catch (_com_error& e) {
         Console.Log(HERE, ComException(HERE, e));
      }
   }
   
   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Freezes or unfreezes the window.</summary>
   /// <param name="freeze">True to freeze, false to restore</param>
   /// <param name="invalidate">True to invalidate after unfreezing</param>
   void RichEditEx::FreezeWindow(bool freeze, bool invalidate)
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
   
   /// <summary>Gets the character co-ordinates of the cursor.</summary>
   /// <returns>Character index within the line, and zero-based line number</returns>
   CPoint  RichEditEx::GetCursorLocation() const
   {
      // Get char index + line number
      UINT charPos = CharFromPos(CursorPoint(this)),
           line = LineFromChar(charPos);

      // Localise char index within line
      return CPoint(charPos-LineIndex(line), line);
   }

   /// <summary>Gets the character index of the end of a line.</summary>
   /// <param name="line">The zero-based line index, or -1 for current line</param>
   /// <returns></returns>
   int  RichEditEx::GetLineEnd(int line) const
   {
      return GetLineStart(line) + GetLineLength(line);
   }

   /// <summary>Gets the character index of the start of a line.</summary>
   /// <param name="line">The zero-based line index, or -1 for current line</param>
   /// <returns></returns>
   int RichEditEx::GetLineStart(int line) const
   {
      return LineIndex(line);
   }

   /// <summary>Gets the coordinates of the first character</summary>
   /// <returns>Character co-orindates</returns>
   CPoint RichEditEx::GetScrollCoordinates() const
   {
      // Preserve scroll position
      int pos = CharFromPos(CPoint(0,0));
      return CPoint(pos-LineIndex(pos), LineFromChar(pos));
   }

   /// <summary>Starts or complets an undo group.</summary>
   /// <param name="start">start/stop.</param>
   void  RichEditEx::GroupUndo(bool start)
   {
      try
      {
         if (start)
            TextDocument->BeginEditCollection();
         else
            TextDocument->EndEditCollection();
      }
      catch (_com_error& e) {
         Console.Log(HERE, ComException(HERE, e));
      }
   }

   /// <summary>Reset the tooltip</summary>
   void RichEditEx::HScroll(UINT nSBCode, UINT nPos)
   {
      // Reset tooltip
      Tooltip.Reset();

      // Scroll
      //__super::OnHScroll(nSBCode, nPos, nullptr);
   }

   /// <summary>Resets the tooltip</summary>
   /// <param name="pNMHDR">The notify header</param>
   /// <param name="pResult">message result</param>
   void RichEditEx::OnInputMessage(NMHDR *pNMHDR, LRESULT *pResult)
   {
      static const UINT ALLOW_INPUT = 0, BLOCK_INPUT = 1;

      // Get character
      MSGFILTER *pFilter = reinterpret_cast<MSGFILTER *>(pNMHDR);
   
      // Allow input by default
      *pResult = ALLOW_INPUT;

      switch (pFilter->msg)
      {
      // KEYBOARD:
      case WM_CHAR:
      case WM_KEYDOWN:
      // MOUSE:
      case WM_LBUTTONDOWN:
      case WM_RBUTTONDOWN:
      case WM_MBUTTONDOWN:
      // WHEEL: 
      case WM_MOUSEWHEEL:
         Tooltip.Reset();
         break;
      }
   }

   /// <summary>Closes the suggestion when focus lost</summary>
   /// <param name="pNewWnd">New window.</param>
   void RichEditEx::OnKillFocus(CWnd* pNewWnd)
   {
      // Deactivate tooltip
      Tooltip.Activate(FALSE);  

      // Kill focus
      __super::OnKillFocus(pNewWnd);
   }
   
   /// <summary>Called when a message would change protected text.</summary>
   /// <param name="pNMHDR">The NMHDR.</param>
   /// <param name="pResult">The result.</param>
   void RichEditEx::OnProtectedMessage(NMHDR *pNMHDR, LRESULT *pResult)
   {
      static const UINT ALLOW_INPUT = 0, BLOCK_INPUT = 1;

      // Get message
      ENPROTECTED *pProtected = reinterpret_cast<ENPROTECTED *>(pNMHDR);
      
      // Block RichEdit from invoking paste itself, then invoke manually so we can highlight text
      switch (pProtected->msg)
      {
      case WM_PASTE:
         // Manually invoke
         if (!IsReadOnly())
            PasteFormat(CF_UNICODETEXT);

         *pResult = BLOCK_INPUT;
         break;

      default:
         *pResult = ALLOW_INPUT;
         break;
      }
   }
   
   /// <summary>Supply tooltip data</summary>
   /// <param name="data">The data.</param>
   void RichEditEx::OnRequestTooltip(CustomTooltip::TooltipData* data)
   {
      // None: show nothing
      *data = CustomTooltip::NoTooltip;
   }
   
   /// <summary>Enable tooltip.</summary>
   /// <param name="pOldWnd">The old WND.</param>
   void RichEditEx::OnSetFocus(CWnd* pOldWnd)
   {
      CRichEditCtrl::OnSetFocus(pOldWnd);

      // Activate tooltip
      Tooltip.Activate(TRUE);   
   }

   /// <summary>Performs syntax colouring on the current line</summary>
   void RichEditEx::OnTextChange()
   {
      // Reset tooltip
      Tooltip.Reset();
   }

   /// <summary>Reset the tooltip</summary>
   void RichEditEx::VScroll(UINT nSBCode, UINT nPos)
   {
      // Reset tooltip
      Tooltip.Reset();

      // Scroll
      //__super::OnVScroll(nSBCode, nPos, nullptr);
   }
   
   /// <summary>Pastes text from clipboard.</summary>
   /// <param name="nClipFormat">The clipboard format.</param>
   void  RichEditEx::PasteFormat(UINT nClipFormat)
   {
      // Paste
      if (CanPaste(nClipFormat) && !IsReadOnly())
         PasteSpecial(nClipFormat);
   }

   /// <summary>Scrolls window to the position of a character</summary>
   /// <param name="pt">Character co-orindates</param>
   void RichEditEx::SetScrollCoordinates(const CPoint& pt)
   {
      CPoint now = GetScrollCoordinates();
      CPoint diff = pt-now;
      LineScroll(diff.y, diff.x);
   }
   
   // ------------------------------- PRIVATE METHODS ------------------------------
   


   
NAMESPACE_END2(GUI,Controls)



