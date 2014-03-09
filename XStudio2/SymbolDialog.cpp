#include "stdafx.h"
#include "SymbolDialog.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)

   // --------------------------------- APP WIZARD ---------------------------------
  
   BEGIN_MESSAGE_MAP(SymbolDialog, CDialog)
      ON_WM_SIZE()
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   SymbolDialog::SymbolDialog(const wstring& symbol, SymbolType type, CWnd* parent) 
      : CDialog(IDD_SYMBOL, parent),
        SymbolName(symbol),
        Preview(TRUE),
        Type(type)
   {
   }

   SymbolDialog::~SymbolDialog()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   /// <summary>Display symbol being renamed.</summary>
   /// <returns></returns>
   BOOL SymbolDialog::OnInitDialog()
   {
      __super::OnInitDialog();

      // Populate
      SymbolText = GuiString(L"%s::%s", Type == SymbolType::Variable ? L"Variable" : L"Label", SymbolName.c_str());
      RenameText = SymbolName;
      UpdateData(FALSE);

      // Set focus to name edit
      GetDlgItem(IDC_RENAME_EDIT)->SetFocus();
      return FALSE;
   }
   
   /// <summary>Validate new name</summary>
   void SymbolDialog::OnOK()
   { 
      UpdateData(TRUE);

      // Require new name
      if (RenameText.empty() || RenameText == SymbolName)
         return;

      // Variable: Prepend with '$'
      if (Type == SymbolType::Variable && RenameText[0] != '$')
         RenameText.insert(0, L"$");

      // Close
      __super::OnOK();
   }

   // ------------------------------ PROTECTED METHODS -----------------------------
   
   void  SymbolDialog::AdjustLayout()
   {
      // Destroyed/Minimised
	   if (!GetSafeHwnd() || theApp.IsMimized())
         return;
         
      CRect wnd;
      GetClientRect(wnd);

      // TODO: Layout code
   }

   /// <summary>Does the data exchange.</summary>
   /// <param name="pDX">The p dx.</param>
   void SymbolDialog::DoDataExchange(CDataExchange* pDX)
   {
      __super::DoDataExchange(pDX);
      DDX_Text(pDX, IDC_SYMBOL_EDIT, SymbolText);
      DDX_Text(pDX, IDC_RENAME_EDIT, RenameText);
      DDX_Check(pDX, IDC_PREVIEW_CHECK, Preview);
   }
   
   void SymbolDialog::OnSize(UINT nType, int cx, int cy)
   {
      __super::OnSize(nType, cx, cy);
      AdjustLayout();
   }
   
   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
NAMESPACE_END2(GUI,Windows)
