#include "stdafx.h"
#include "RefactorDialog.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)

   // --------------------------------- APP WIZARD ---------------------------------
  
   //IMPLEMENT_DYNCREATE(RefactorDialog, CDialog)

   BEGIN_MESSAGE_MAP(RefactorDialog, CDialog)
      ON_WM_SIZE()
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   RefactorDialog::RefactorDialog(SymbolList matches, CWnd* parent) 
       : CDialog(IDD_REFACTOR, parent), AllSymbols(matches.begin(), matches.end())
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

      // Populate symbols
      UINT i = 0;
      for (auto& s : AllSymbols)
      {
         List.InsertString(i, s.LineText.c_str());
         List.SetCheck(i++, BST_CHECKED);
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
      DDX_Control(pDX, IDC_SYMBOL_LIST, List);
      __super::DoDataExchange(pDX);
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
   
   
   NAMESPACE_END2(GUI, Windows)


