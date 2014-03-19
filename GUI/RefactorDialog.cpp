#include "stdafx.h"
#include "RefactorDialog.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)

   // --------------------------------- APP WIZARD ---------------------------------
  
   //IMPLEMENT_DYNCREATE(RefactorDialog, CDialog)

   BEGIN_MESSAGE_MAP(RefactorDialog, CDialog)
      ON_WM_SIZE()
      ON_LBN_SELCHANGE(IDC_SYMBOL_LIST, OnSelectionChanged)
      ON_CLBN_CHKCHANGE(IDC_SYMBOL_LIST, OnCheckChanged)
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   RefactorDialog::RefactorDialog(ScriptDocument& doc, const wstring& text, SymbolList matches, CWnd* parent) 
       : CDialog(IDD_REFACTOR, parent), 
         AllSymbols(matches.begin(), matches.end()),
         Document(doc),
         DocumentText(text),
         PrevSelection(LB_ERR)
   {
   }

   RefactorDialog::~RefactorDialog()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------
   
   /// <summary>Populates the symbol matches.</summary>
   /// <returns></returns>
   BOOL RefactorDialog::OnInitDialog()
   {
      // Init
      __super::OnInitDialog();

      try
      {
         UINT i = 0;
         
         // Populate symbols list
         for (auto& s : AllSymbols)
         {
            // Format: (line) text
            VString line(L"(%d) %s%s", s.LineNumber, s.Commented ? L"* " : L"", s.LineText.TrimLeft(L" \t").c_str());

            // Insert item
            List.InsertString(i, line.c_str());
            List.SetCheck(i++, !s.Commented ? BST_CHECKED : BST_UNCHECKED);
         }
         
         // Populate edit
         RichEdit.Initialize(&Document);
         RichEdit.SetPlainText(DocumentText);
         RichEdit.HighlightSymbols(AllSymbols);

         // Scroll to first symbol
         if (!AllSymbols.empty())
            RichEdit.ScrollTo(AllSymbols.front());
      }
      catch (ExceptionBase& e) {
         theApp.ShowError(HERE, e, L"Unable to initialise the refactor dialog");
      }

      return TRUE;
   }
   
   /// <summary>Populates the accepted matches.</summary>
   void RefactorDialog::OnOK()
   { 
      // Populate 'Accepted' from checked items
      for (UINT i = 0, count = List.GetCount(); i < count; ++i)
         if (List.GetCheck(i) == BST_CHECKED)
            Accepted.push_back(AllSymbols[i]);

      // Close
      __super::OnOK();
   }

   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Adjusts the layout.</summary>
   void  RefactorDialog::AdjustLayout()
   {
      // Destroyed/Minimised
	   if (!GetSafeHwnd() || theApp.IsMimized())
         return;
         
      ClientRect wnd(this);

      // TODO: Layout code
   }
   
   /// <summary>Does the data exchange.</summary>
   /// <param name="pDX">The p dx.</param>
   void RefactorDialog::DoDataExchange(CDataExchange* pDX)
   {
      __super::DoDataExchange(pDX);
      DDX_Control(pDX, IDC_SYMBOL_LIST, List);
      DDX_Control(pDX, IDC_SYMBOL_EDIT, RichEdit);
   }
   
   
   /// <summary>Highlights and scrolls to a symbol.</summary>
   /// <param name="index">Zero-based index.</param>
   /// <param name="display">Whether to scroll symbol into view.</param>
   void RefactorDialog::HighlightSymbol(UINT index, bool display)
   {
      if (index >= AllSymbols.size())
         return;
      
      // Determine whether selected/highlighted/unchecked
      auto ht = index == List.GetCurSel()           ? RefactorEdit::HighlightType::Active
              : List.GetCheck(index) == BST_CHECKED ? RefactorEdit::HighlightType::Inactive
                                                    : RefactorEdit::HighlightType::None;

      // Lookup/Highlight 
      auto& s = AllSymbols[index];
      RichEdit.HighlightSymbol(s, ht);

      // scroll to symbol
      if (display)
         RichEdit.ScrollTo(s);
   }

   /// <summary>Toggles highlighting the selected token</summary>
   void RefactorDialog::OnCheckChanged()
   {
      // Require selection
      if (List.GetCurSel() == LB_ERR)
         return;

      // DEBUG:
      //Console << "RefactorDialog::OnCheckChanged()  List.GetCurSel=" << List.GetCurSel() << ENDL;

      // Highlight symbol
      HighlightSymbol(PrevSelection, false);
      HighlightSymbol(PrevSelection = List.GetCurSel(), true);
   }

   /// <summary>Displays the selected line</summary>
   void RefactorDialog::OnSelectionChanged()
   {
      // Require selection
      if (List.GetCurSel() == LB_ERR)
         return;

      // DEBUG:
      //Console << "RefactorDialog::OnSelectionChanged()  List.GetCurSel=" << List.GetCurSel() << ENDL;

      // Highlight symbol
      HighlightSymbol(PrevSelection, false);
      HighlightSymbol(PrevSelection = List.GetCurSel(), true);
   }

   /// <summary>Adjusts the layout on resize</summary>
   /// <param name="nType">Type of the n.</param>
   /// <param name="cx">The width.</param>
   /// <param name="cy">The height.</param>
   void RefactorDialog::OnSize(UINT nType, int cx, int cy)
   {
      __super::OnSize(nType, cx, cy);
      AdjustLayout();
   }
   
   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
NAMESPACE_END2(GUI,Windows)


