#include "stdafx.h"
#include "AboutDlg.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN(GUI)

   // --------------------------------- APP WIZARD ---------------------------------
  
   BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
   END_MESSAGE_MAP()

   // -------------------------------- CONSTRUCTION --------------------------------

   CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------


   // ------------------------------ PROTECTED METHODS -----------------------------

   void CAboutDlg::DoDataExchange(CDataExchange* pDX)
   {
	   CDialogEx::DoDataExchange(pDX);
   }

   // ------------------------------- PRIVATE METHODS ------------------------------


NAMESPACE_END(GUI)
