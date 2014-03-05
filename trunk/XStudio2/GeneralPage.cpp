#include "stdafx.h"
#include "GeneralPage.h"
#include "afxdialogex.h"


/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Preferences)

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNAMIC(GeneralPage, PreferencesPage)

   BEGIN_MESSAGE_MAP(GeneralPage, PreferencesPage)
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   GeneralPage::GeneralPage() : PreferencesPage(GeneralPage::IDD)
   {
   }

   GeneralPage::~GeneralPage()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   /// <summary>Performs data exchange.</summary>
   /// <param name="pDX">The dx.</param>
   void GeneralPage::DoDataExchange(CDataExchange* pDX)
   {
	   __super::DoDataExchange(pDX);
   }
   
   /// <summary>Populates property grid.</summary>
   /// <returns></returns>
   BOOL GeneralPage::OnInitDialog()
   {
      try
      {
         // Create base
	      if (!__super::OnInitDialog())
		      throw Win32Exception(HERE, L"Failed to create dialog base");


         // DEBUG: Populate grid
         auto prop = new CMFCPropertyGridProperty(L"Example", L"Value", L"Description");
         Grid.AddProperty(prop);

	      return TRUE;
      }
      catch (ExceptionBase& e) {
         Console.Log(HERE, e);
         return FALSE;
      }
   }


   void GeneralPage::OnOK()
   {
      // TODO: Add your specialized code here and/or call the base class

      __super::OnOK();
   }


   // ------------------------------ PROTECTED METHODS -----------------------------
   
   
   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
NAMESPACE_END2(GUI,Preferences)


