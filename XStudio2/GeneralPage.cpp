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

         Populate();
	      return TRUE;
      }
      catch (ExceptionBase& e) {
         Console.Log(HERE, e);
         return FALSE;
      }
   }


   void GeneralPage::OnOK()
   {
      Console << "GeneralPage::OnOK() LargeToolbars=" << LargeToolbars->GetBool() << ENDL;

      __super::OnOK();
   }


   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Populates this page.</summary>
   void GeneralPage::Populate()
   {
      // Populate grid
      Grid.AddProperty(new PropertyBase(*this, L"General"));
      Grid.AddProperty(LargeToolbars = new LargeToolbarsProperty(*this));
      Grid.AddProperty(ToolWindowFont = new ToolWindowFontProperty(*this));
   }

   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
NAMESPACE_END2(GUI,Preferences)


