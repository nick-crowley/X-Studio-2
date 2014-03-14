#include "stdafx.h"
#include "DiffEdit.h"
#include "DiffView.h"

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
   /// <param name="view">View type.</param>
   /// <param name="diff">Diff document.</param>
   /// <param name="doc">Source script document.</param>
   /// <exception cref="Logic::ArgumentNullException">document is null</exception>
   /// <exception cref="Logic::Win32Exception">Unable to retrieve COM pointers</exception>
   void  DiffEdit::Initialize(DiffViewType view, GUI::Documents::DiffDocument* diff, ScriptDocument* doc)
   {
      REQUIRED(diff);

      // Set document
      DiffDocument = diff;
      ViewType = view;

      // Set ReadOnly
      ReadOnly = TRUE;

      // Initialize
      __super::Initialize(doc);
   }
   
   /// <summary>Scrolls the opposing view, if this view has focus</summary>
   void  DiffEdit::ScrollSibling()
   {
      // Focus: Command alternate view to scroll
      if (::GetFocus() == *this)
      {
         //Console << "Adjusting sibling scroll co-ords" << ENDL;
         DiffDocument->GetView(!ViewType)->RichEdit.SetScrollCoordinates(GetScrollCoordinates());
      }
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
   
   /// <summary>Command opposing view to scroll</summary>
   void DiffEdit::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* bar)
   {
      //Console << "DiffEdit::OnHScroll" << ENDL;

      // Refresh gutter
      __super::OnHScroll(nSBCode, nPos, bar);

      // Drag: Scroll manually  [notification not received by view]
      if (nSBCode == SB_THUMBTRACK)
         ScrollSibling();
   }
   
   /// <summary>Command opposing view to scroll</summary>
   void DiffEdit::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* bar)
   {
      //Console << "DiffEdit::OnVScroll" << ENDL;

      // Refresh gutter
      __super::OnVScroll(nSBCode, nPos, bar);

      // Drag: Scroll manually  [notification not received by view]
      if (nSBCode == SB_THUMBTRACK)
         ScrollSibling();
   }
   

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
      cf.crBackColor = PrefsLib.DifferenceHighlight;

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



