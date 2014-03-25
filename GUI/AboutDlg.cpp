#include "stdafx.h"
#include "AboutDlg.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)

   // --------------------------------- APP WIZARD ---------------------------------
  
   BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
   END_MESSAGE_MAP()

   // -------------------------------- CONSTRUCTION --------------------------------

   CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   /// <summary>initialize dialog.</summary>
   /// <returns></returns>
   BOOL CAboutDlg::OnInitDialog()
   {
      __super::OnInitDialog();

      // Title: About X-Studio II
      auto title = VString(L"About %s", GuiString(AFX_IDS_APP_TITLE).c_str());
      SetWindowText(title.c_str());

      // Version: X-Studio II  (v1.0)
      auto version = VString(L"%s (%s)", GuiString(AFX_IDS_APP_TITLE).c_str(), BUILD_NAME);
      SetDlgItemText(IDC_NAME_STATIC, version.c_str());

      // Display
      CenterWindow();
      return TRUE;
   }

   // ------------------------------ PROTECTED METHODS -----------------------------

   void CAboutDlg::DoDataExchange(CDataExchange* pDX)
   {
	   __super::DoDataExchange(pDX);
   }

   // ------------------------------- PRIVATE METHODS ------------------------------


NAMESPACE_END2(GUI,Windows)

