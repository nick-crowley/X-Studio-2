#include "stdafx.h"
#include "RefactorEdit.h"

/// <summary>User interface controls</summary>
NAMESPACE_BEGIN2(GUI,Controls)

   // --------------------------------- CONSTANTS ----------------------------------

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNAMIC(RefactorEdit, ScriptEdit)

   BEGIN_MESSAGE_MAP(RefactorEdit, ScriptEdit)
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   RefactorEdit::RefactorEdit() 
   {
   }

   RefactorEdit::~RefactorEdit()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   /// <summary>Initializes the control</summary>
   /// <param name="doc">Source script document.</param>
   /// <exception cref="Logic::ArgumentNullException">document is null</exception>
   /// <exception cref="Logic::Win32Exception">Unable to retrieve COM pointers</exception>
   void  RefactorEdit::Initialize(ScriptDocument* doc)
   {
      // Set ReadOnly
      ReadOnly = TRUE;

      // Initialize
      __super::Initialize(doc);
   }

   /// <summary>Sets highlighting for a single symbol.</summary>
   /// <param name="s">symbol.</param>
   /// <param name="ht">Highlight type.</param>
   void RefactorEdit::HighlightSymbol(const Symbol& s, HighlightType ht)
   {
      // Freeze
      SuspendUndo(true);
      FreezeWindow(true);

      // Define normal/refactor highlight
      CharFormat cf(CFM_BACKCOLOR, 0);
      cf.crBackColor = (ht == HighlightType::Active   ? PrefsLib.RefactorSelection
                      : ht == HighlightType::Inactive ? PrefsLib.RefactorHighlight 
                                                      : PrefsLib.BackgroundColour);

      // Highlight
      UINT offset = LineIndex(s.LineNumber-1);
      SetSel(offset+s.Token.Start, offset+s.Token.End);
      SetSelectionCharFormat(cf);

      // Restore 
      FreezeWindow(false);
      SuspendUndo(false);
   }

   /// <summary>Highlights all symbols with 'inactive' highlighting</summary>
   /// <param name="symbols">symbols collection.</param>
   void RefactorEdit::HighlightSymbols(const vector<Symbol> symbols)
   {
      // Freeze
      SuspendUndo(true);
      FreezeWindow(true);

      // Define refactor highlight
      CharFormat cf(CFM_BACKCOLOR, 0);
      cf.crBackColor = PrefsLib.RefactorHighlight;

      // Highlight symbols
      for (auto& s : symbols)
      {
         UINT offset = LineIndex(s.LineNumber-1);

         // Highlight
         SetSel(offset+s.Token.Start, offset+s.Token.End);
         SetSelectionCharFormat(cf);
      }

      // Restore 
      FreezeWindow(false);
      SuspendUndo(false);
   }
   
   /// <summary>Scrolls edit to display a symbol</summary>
   /// <param name="s">symbol.</param>
   void  RefactorEdit::ScrollTo(const Symbol& s)
   {
      // Scroll to a few lines before symbol
      EnsureVisible( s.LineNumber-4 );
   }

   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Updates the highlighting.</summary>
   /// <param name="first">first zero-based line number</param>
   /// <param name="last">last zero-based line number.</param>
   //void RefactorEdit::UpdateHighlighting(int first, int last)
   //{
   //   // Perform syntax colouring
   //   __super::UpdateHighlighting(first, last);

   //   // Freeze window
   //   SuspendUndo(true);
   //   FreezeWindow(true);

   //   // Define grey background
   //   CharFormat cf(CFM_BACKCOLOR,0);
   //   cf.crBackColor = RGB(80,80,80);

   //   // Highlight phrases
   //   for (auto p : DiffDocument->Phrases)
   //   {
   //      SetSel(p.GetStart(), p.GetEnd());
   //      SetSelectionCharFormat(cf);
   //   }

   //   // Restore selection
   //   FreezeWindow(false);
   //   SuspendUndo(false);
   //}

   // ------------------------------- PRIVATE METHODS ------------------------------
   


   
NAMESPACE_END2(GUI,Controls)



