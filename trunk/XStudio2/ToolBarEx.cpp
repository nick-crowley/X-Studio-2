#include "stdafx.h"
#include "ToolBarEx.h"

/// <summary>User interface controls</summary>
NAMESPACE_BEGIN2(GUI,Controls)

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNCREATE(ToolBarEx, CMFCToolBar)

   // -------------------------------- CONSTRUCTION --------------------------------

   ToolBarEx::ToolBarEx()
   {
   }

   ToolBarEx::~ToolBarEx()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   /// <summary>Creates a toolbar with normal and disabled icons</summary>
   /// <param name="parent">Parent window</param>
   /// <param name="nResID">Hot icons resource ID.</param>
   /// <param name="nDisabledResID">Disabled icons resource ID</param>
   /// <returns></returns>
   BOOL  ToolBarEx::Create(CWnd* parent, UINT nID, UINT nDisabledResID)
   {
      try
      {
         // Create
         if (!CMFCToolBar::Create(parent, AFX_DEFAULT_TOOLBAR_STYLE, nID))
            throw Win32Exception(HERE, GuiString(L"Unable to create toolbar %d", nID));

         // Load toolbar + bitmaps
	      LoadToolBar(nID, nID, nID, TRUE, nDisabledResID);
	      CleanUpLockedImages();
	      LoadBitmap(nID, nID, nID, TRUE, nDisabledResID);

         // Set styles
         SetPaneStyle(GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	      SetPaneStyle(GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	      SetOwner(parent);

         // Route commands via this control , not via the parent frame:
	      SetRouteCommandsViaFrame(FALSE);
         return TRUE;
      }
      catch (ExceptionBase& e) {
         Console.Log(HERE, e);
         return FALSE;
      }
   }
   
   // ------------------------------ PROTECTED METHODS -----------------------------
   
   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
NAMESPACE_END2(GUI,Controls)

