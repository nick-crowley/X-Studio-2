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
      ON_WM_CHAR()
      ON_WM_KEYDOWN()
      ON_NOTIFY_REFLECT(EN_MSGFILTER, &ScriptEdit::OnInputMessage)
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   ScriptEdit::ScriptEdit()
   {
   }

   ScriptEdit::~ScriptEdit()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

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

   /// <summary>Get line text</summary>
   /// <param name="line">zero based line index, or -1 for current line</param>
   /// <returns></returns>
   wstring ScriptEdit::GetLineText(int line) const
   {
      // Use line containing caret if unspecified
      line = (line == -1 ? LineFromChar(-1) : line);

      // Allocate buffer long enough to hold text + buffer length stored in first DWORD
      int len = LineLength(LineIndex(line));
      len = max(4, len);
      
      // Get text. 
      CharArrayPtr txt(new WCHAR[len+1]);
      len = CRichEditCtrl::GetLine(line, txt.get(), len);

      // Strip \v if present
      if (txt.get()[max(0,len-1)] == '\v')
         --len;

      // Null terminate
      txt.get()[len] = NULL;
      return txt.get();
   }

   /// <summary>Highlights errors indicated by the compiler</summary>
   /// <param name="t">Script tree</param>
   void ScriptEdit::HighlightErrors(ScriptParser::ScriptTree& t)
   {
      CHARFORMAT2 cf;

      // Freeze window
      FreezeWindow(true);

      //Console << L"Highlighting " << t.GetErrors().size() << L" errors" << ENDL;

      // Examine errors
      for (const auto& err : t.GetErrors())
      {
         // Define underline
         cf.cbSize = sizeof(cf);
         cf.dwMask = CFM_UNDERLINE | CFM_UNDERLINETYPE;
         cf.dwEffects = CFE_UNDERLINE;
         cf.bUnderlineType = CFU_UNDERLINEWAVE;

         // TEST: Undocumented underline colour
         BYTE* colour = &cf.bRevAuthor + 1;
         //cf.bUnderlineColor = 0x01;
         *colour = 0x02;

         // Format text
         UINT start = LineIndex(err.Line-1);
         SetSel(start+err.Start, start+err.End);
         SetSelectionCharFormat(cf);

         Console << L"Syntax error on line " << err.Line << L": '" << (const WCHAR*)GetSelText() << ENDL;
      }

      // UnFreeze window
      FreezeWindow(false);
   }

   /// <summary>Replace entire contents with RTF.</summary>
   /// <param name="rtf">The RTF.</param>
   void ScriptEdit::SetRtf(const string& rtf)
   {
      SETTEXTEX opt = {ST_DEFAULT, CP_ACP};
      SendMessage(EM_SETTEXTEX, (WPARAM)&opt, (LPARAM)rtf.c_str());
   }
   
   // ------------------------------ PROTECTED METHODS -----------------------------
   
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
   CPoint ScriptEdit::GetScrollCoordinates()
   {
      // Preserve scroll position
      int pos = CharFromPos(CPoint(0,0));
      return CPoint(pos-LineIndex(pos), LineFromChar(pos));
   }

   /// <summary>Gets the co-ordinates of the current selection</summary>
   /// <returns></returns>
   CHARRANGE ScriptEdit::GetSelection() const
   {
      CHARRANGE sel;
      CRichEditCtrl::GetSel(sel);
      return sel;
   }
   
   bool ScriptEdit::IsSuggestionInitiator(wchar ch) const
   {
      return ch == '$';
   }

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

   void ScriptEdit::UpdateSuggestions()
   {
      // Ensure exists
      if (Suggestions.GetSafeHwnd() == nullptr)
         throw InvalidOperationException(HERE, L"suggestion list does not exist");
   }

   void ScriptEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
   {
      //try
      //{
      //   // Normal: Display suggestions if character is initiator
      //   if (State == InputState::Normal && IsSuggestionInitiator(nChar))
      //      ShowSuggestions();

      //   // Suggestions: Update match
      //   else if (State == InputState::Suggestions)
      //      UpdateSuggestions();
      //}
      //catch (ExceptionBase& e) { 
      //   Console << e; 
      //}

      // Process char
      CRichEditCtrl::OnChar(nChar, nRepCnt, nFlags);
   }

   
   void ScriptEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
   {
      //try
      //{
      //   // Suggestions: Close on escape
      //   if (State == InputState::Suggestions && nChar == VK_ESCAPE)
      //      CloseSuggestions();
      //}
      //catch (ExceptionBase& e) { 
      //   Console << e; 
      //}

      // Process keypress
      CRichEditCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
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
   

   void ScriptEdit::OnInputMessage(NMHDR *pNMHDR, LRESULT *pResult)
   {
      MSGFILTER *pFilter = reinterpret_cast<MSGFILTER *>(pNMHDR);
      wchar chr = pFilter->wParam;
   
      try
      {
         switch (pFilter->msg)
         {
         case WM_CHAR:
            // Normal mode: Display suggestions if character is initiator
            if (State == InputState::Normal && IsSuggestionInitiator(chr))
               ShowSuggestions();

            // Suggestions mode: Update match
            else if (State == InputState::Suggestions)
               UpdateSuggestions();
            break;

         case WM_KEYDOWN:
            // Suggestions mode: Close on escape
            if (State == InputState::Suggestions && chr == VK_ESCAPE)
               CloseSuggestions();
            break;
         }
      }
      catch (ExceptionBase& e) { 
         Console << e; 
      }

      // Pass to base
      *pResult = ALLOW_INPUT;
   }


   /// <summary>Performs syntax colouring on the current line</summary>
   void ScriptEdit::OnTextChange()
   {
      //Console << L"OnChange" << ENDL;

      // Freeze window
      FreezeWindow(true);
      
      // Set/Reset background compiler timer
      SetCompilerTimer(true);

      // Get index of first character
      UINT start = LineIndex(-1);

      try 
      {
         // Lex current line
         CommandLexer lex(GetLineText(-1));
         for (const auto& tok : lex.Tokens)
         {
            CHARFORMAT2 cf;
            cf.cbSize = sizeof(cf);
            cf.dwMask = CFM_COLOR | CFM_UNDERLINE | CFM_UNDERLINETYPE;
            cf.bUnderlineType = NULL;
            cf.dwEffects = NULL;
      
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

            // Set char format
            SetSel(start+tok.Start, start+tok.End);
            SetSelectionCharFormat(cf);
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

   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
NAMESPACE_END2(GUI,Controls)



