#include "stdafx.h"
#include "PreferencesDialog.h"
#include "afxdialogex.h"


/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Preferences)

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNAMIC(PreferencesDialog, CMFCPropertySheet)

   BEGIN_MESSAGE_MAP(PreferencesDialog, CMFCPropertySheet)
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   PreferencesDialog::PreferencesDialog(CWnd* parent) : CMFCPropertySheet(L"Preferences", parent, 0)
   {
      AddPage(&General);
      AddPage(&GameData);
      AddPage(&Editor);
      AddPage(&Compiler);
      SetLook(PropSheetLook::PropSheetLook_List);
   }

   PreferencesDialog::~PreferencesDialog()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   /// <summary>Performs data exchange.</summary>
   /// <param name="pDX">The dx.</param>
   void PreferencesDialog::DoDataExchange(CDataExchange* pDX)
   {
	   __super::DoDataExchange(pDX);
   }


   // ------------------------------ PROTECTED METHODS -----------------------------
   
   
   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
NAMESPACE_END2(GUI,Preferences)

