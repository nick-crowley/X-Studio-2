#include "stdafx.h"
#include "PreferencesPage.h"
#include "afxdialogex.h"


/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Preferences)

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNAMIC(PreferencesPage, CMFCPropertyPage)

   BEGIN_MESSAGE_MAP(PreferencesPage, CMFCPropertyPage)
      ON_WM_SIZE()
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   PreferencesPage::PreferencesPage(UINT nID) : CMFCPropertyPage(nID)
   {
   }

   PreferencesPage::~PreferencesPage()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   /// <summary>Initializes the property grid.</summary>
   /// <returns></returns>
   BOOL PreferencesPage::OnInitDialog()
   {
      try
      {
         // Create base
	      if (!__super::OnInitDialog())
		      throw Win32Exception(HERE, L"Failed to create dialog base");

	      ClientRect wnd(this);

         // Create property grid
	      if (!Grid.Create(WS_VISIBLE | WS_CHILD, wnd, this, IDC_PROPERTY_GRID))
	         throw Win32Exception(HERE, L"Failed to create Properties Grid");

	      // Grid
         Grid.EnableHeaderCtrl(FALSE);
	      Grid.EnableDescriptionArea();
	      Grid.SetVSDotNetLook();
	      Grid.MarkModifiedProperties();

         // Adjust layout
         AdjustLayout();
	      return TRUE;
      }
      catch (ExceptionBase& e) {
         Console.Log(HERE, e);
         return FALSE;
      }
   }


   void PreferencesPage::OnOK()
   {
      // TODO: Add your specialized code here and/or call the base class

      __super::OnOK();
   }

   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Adjusts the layout.</summary>
   void PreferencesPage::AdjustLayout()
   {
      //ClientRect wnd(this);

      // Stretch across entire dialog
      /*wnd.DeflateRect(8,8);
      Grid.SetWindowPos(nullptr, -1, -1, wnd.Width(), wnd.Height(), SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);*/

      //CtrlRect  ctrl(this, &Grid);

      // Allow space for page header
      //Grid.SetWindowPos(nullptr, ctrl.left, 30, -1, -1, SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);
   }

   
   /// <summary>Performs data exchange.</summary>
   /// <param name="pDX">The dx.</param>
   void PreferencesPage::DoDataExchange(CDataExchange* pDX)
   {
	   __super::DoDataExchange(pDX);
   }
   
   /// <summary>Adjusts the layout when resized.</summary>
   /// <param name="nType">Type of the n.</param>
   /// <param name="cx">The width.</param>
   /// <param name="cy">The height.</param>
   void PreferencesPage::OnSize(UINT nType, int cx, int cy)
   {
      __super::OnSize(nType, cx, cy);

      // do layout
      AdjustLayout();
   }

   /// <summary>Stub.</summary>
   void PreferencesPage::Populate()
   {
   }

   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
NAMESPACE_END2(GUI,Preferences)


