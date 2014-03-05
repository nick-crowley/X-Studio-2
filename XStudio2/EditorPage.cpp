
#include "stdafx.h"
#include "EditorPage.h"
#include "afxdialogex.h"


/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Preferences)

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNAMIC(EditorPage, PreferencesPage)

   BEGIN_MESSAGE_MAP(EditorPage, PreferencesPage)
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   EditorPage::EditorPage() : PreferencesPage(EditorPage::IDD)
   {
   }

   EditorPage::~EditorPage()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   /// <summary>Performs data exchange.</summary>
   /// <param name="pDX">The dx.</param>
   void EditorPage::DoDataExchange(CDataExchange* pDX)
   {
	   __super::DoDataExchange(pDX);
   }


   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Populates this page.</summary>
   void EditorPage::Populate()
   {
      // General
      auto group = new PropertyBase(*this, L"General");
      group->AddSubItem(ShowLineNumbers = new ShowLineNumbersProperty(*this));
      Grid.AddProperty(group);
   }
   
   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
NAMESPACE_END2(GUI,Preferences)

