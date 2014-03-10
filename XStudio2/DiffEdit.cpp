#include "stdafx.h"
#include "DiffEdit.h"

/// <summary>User interface controls</summary>
NAMESPACE_BEGIN2(GUI,Controls)

   // --------------------------------- CONSTANTS ----------------------------------

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNAMIC(DiffEdit, ScriptEdit)

   BEGIN_MESSAGE_MAP(DiffEdit, ScriptEdit)
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   DiffEdit::DiffEdit() : DiffDocument(nullptr)
   {
   }

   DiffEdit::~DiffEdit()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   /// <summary>Initializes the control</summary>
   /// <param name="doc">The document.</param>
   /// <exception cref="Logic::ArgumentNullException">document is null</exception>
   /// <exception cref="Logic::Win32Exception">Unable to retrieve COM pointers</exception>
   void  DiffEdit::Initialize(GUI::Documents::DiffDocument* diff, ScriptDocument* doc)
   {
      REQUIRED(diff);

      // Set document
      DiffDocument = diff;

      // Set ReadOnly
      ReadOnly = TRUE;

      // Initialize
      __super::Initialize(doc);
   }

   /// <summary>Replace entire contents with plain text.</summary>
   /// <param name="txt">text.</param>
   /// <exception cref="Logic::InvalidOperationException">No document attached</exception>
   //void DiffEdit::SetPlainText(const wstring& txt)
   //{
   //   // Require document
   //   if (Document == nullptr)
   //      throw InvalidOperationException(HERE, L"Must attach document prior to displaying text");

   //   // Set text
   //   SetWindowText(txt.c_str());

   //   // Set default character format
   //   SetDefaultCharFormat(CharFormat(PrefsLib.ScriptViewFont, this));

   //   // Adjust gutter
   //   SetGutterWidth(GutterRect(this).Width());

   //   // Update highlighting
   //   UpdateHighlighting(0, GetLineCount()-1);
   //   EnsureVisible(0);

   //   // Reset Undo
   //   EmptyUndoBuffer();
   //}

   /// <summary>Replace entire contents with RTF.</summary>
   /// <param name="rtf">The RTF.</param>
   /// <exception cref="Logic::InvalidOperationException">No document attached</exception>
   /// <exception cref="Logic::Win32Exception">Unable to set text</exception>
   //void DiffEdit::SetRtf(const string& rtf)
   //{
   //   throw NotSupportedException(HERE, L"Cannot display RTF");

   //   // Require document
   //   if (Document == nullptr)
   //      throw InvalidOperationException(HERE, L"Must attach document prior to displaying text");

   //   // Set RTF
   //   __super::SetRtf(rtf);

   //   // Set default character format
   //   SetDefaultCharFormat(CharFormat(PrefsLib.ScriptViewFont, this));

   //   // Adjust gutter
   //   SetGutterWidth(GutterRect(this).Width());
   //   EnsureVisible(0);

   //   // Reset Undo
   //   EmptyUndoBuffer();
   //}

   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Refreshes the line numbers after a scroll</summary>
   //void DiffEdit::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
   //{
   //   // Redraw the line numbers after a scroll or drag
   //   if (nSBCode == SB_ENDSCROLL)
   //      RefreshGutter();

   //   // Reset tooltip
   //   __super::OnHScroll(nSBCode, nPos, pScrollBar);
   //}
   
   /// <summary>Blocks or forwards certain keys used in suggestion display</summary>
   /// <param name="pNMHDR">The notify header</param>
   /// <param name="pResult">message result</param>
   //void DiffEdit::OnInputMessage(NMHDR *pNMHDR, LRESULT *pResult)
   //{
   //   static const UINT ALLOW_INPUT = 0, BLOCK_INPUT = 1;

   //   // Get character
   //   MSGFILTER *pFilter = reinterpret_cast<MSGFILTER *>(pNMHDR);
   //   wchar chr = pFilter->wParam;

   //   // Reset tooltip
   //   __super::OnInputMessage(pNMHDR, pResult);
   //
   //   // Allow input by default
   //   *pResult = ALLOW_INPUT;

   //   try
   //   {
   //      if (State == InputState::Suggestions)
   //         switch (pFilter->msg)
   //         {
   //         case WM_CHAR:
   //            switch (chr)
   //            {
   //            // TAB: Insert suggestion.  Prevent focus switch
   //            case VK_TAB:
   //               InsertSuggestion();
   //               *pResult = BLOCK_INPUT;
   //               break;
   //         
   //            // ESCAPE: Close suggestions
   //            case VK_ESCAPE:
   //               CloseSuggestions();
   //               *pResult = BLOCK_INPUT;
   //               break;
   //            }
   //            break;

   //         // NAVIGATION: Update suggestions / Forward message
   //         case WM_KEYDOWN:
   //            switch (chr)
   //            {
   //            // TAB: Insert suggestion
   //            case VK_TAB:
   //               InsertSuggestion();
   //               *pResult = BLOCK_INPUT;
   //               break;

   //            // ESCAPE: Close suggestions
   //            case VK_ESCAPE:
   //               CloseSuggestions();
   //               *pResult = BLOCK_INPUT;
   //               break;

   //            // UP/DOWN/PAGEUP/PAGEDOWN: Navigate suggestions list
   //            case VK_UP:    
   //            case VK_DOWN:
   //            case VK_PRIOR: 
   //            case VK_NEXT:  
   //               SuggestionsList.SendMessage(pFilter->msg, pFilter->wParam, pFilter->lParam);
   //               *pResult = BLOCK_INPUT;
   //               return;
   //            }
   //            break;

   //         // CLICK: Close Suggestions
   //         case WM_LBUTTONDOWN:
   //         case WM_RBUTTONDOWN:
   //         case WM_MBUTTONDOWN:
   //            CloseSuggestions();
   //            break;

   //         // WHEEL: Forward to list
   //         case WM_MOUSEWHEEL:
   //            SuggestionsList.SendMessage(pFilter->msg, pFilter->wParam, pFilter->lParam);
   //            *pResult = BLOCK_INPUT;
   //            break;
   //         }
   //   }
   //   catch (ExceptionBase& e) { 
   //      Console.Log(HERE, e, GuiString(L"Unable to process input filter: message=%d wparam=%d lparam=%d char='%c'",pFilter->msg, pFilter->wParam, pFilter->lParam, chr) ); 
   //   }
   //}

   /// <summary>Refreshes the line numbers after a scroll</summary>
   //void DiffEdit::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
   //{
   //   // Redraw the line numbers after a scroll or drag
   //   if (nSBCode == SB_ENDSCROLL)
   //      RefreshGutter();

   //   // Reset Tooltip + Scroll
   //   __super::OnVScroll(nSBCode, nPos, pScrollBar);
   //}

   /// <summary>Updates the highlighting.</summary>
   /// <param name="first">first zero-based line number</param>
   /// <param name="last">last zero-based line number.</param>
   void DiffEdit::UpdateHighlighting(int first, int last)
   {
      // Perform syntax colouring
      __super::UpdateHighlighting(first, last);

      // Freeze window
      SuspendUndo(true);
      FreezeWindow(true);

      // Define grey background
      CharFormat cf(CFM_BACKCOLOR,0);
      cf.crBackColor = RGB(80,80,80);

      // Highlight phrases
      for (auto p : DiffDocument->Phrases)
      {
         SetSel(p.GetStart(), p.GetEnd());
         SetSelectionCharFormat(cf);
      }

      // Restore selection
      FreezeWindow(false);
      SuspendUndo(false);
   }

   // ------------------------------- PRIVATE METHODS ------------------------------
   


   
NAMESPACE_END2(GUI,Controls)



