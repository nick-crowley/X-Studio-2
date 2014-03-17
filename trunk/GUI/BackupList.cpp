#include "stdafx.h"

#include "BackupList.h"
#include "Helpers.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/// <summary>User interface Controls</summary>
NAMESPACE_BEGIN2(GUI,Controls)

   // --------------------------------- APP WIZARD ---------------------------------
   
   BEGIN_MESSAGE_MAP(BackupList, CListBox)
      ON_WM_CREATE()
	   //ON_WM_CONTEXTMENU()
      ON_WM_SETTINGCHANGE()
      ON_WM_ERASEBKGND()
   END_MESSAGE_MAP()

   // -------------------------------- CONSTRUCTION --------------------------------

   BackupList::BackupList()
   {
   }

   BackupList::~BackupList()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Called when [create].</summary>
   /// <param name="lpCreateStruct">The lp create structure.</param>
   /// <returns></returns>
   int BackupList::OnCreate(LPCREATESTRUCT lpCreateStruct)
   {
	   if (__super::OnCreate(lpCreateStruct) == -1)
		   return -1;

      // Set font
      SetFont(&theApp.ToolWindowFont);
	   return 0;
   }

   /// <summary>Displays context menu.</summary>
   /// <param name="">window clicked</param>
   /// <param name="point">The point.</param>
   //void BackupList::OnContextMenu(CWnd* pWnd, CPoint point)
   //{
   //   BOOL none = FALSE;
   //       
   //   // Select item beneath cursor
   //   int item = ItemFromPoint(CursorPoint(this), none);
   //   SetCurSel(!none ? item : LB_ERR);

   //   // Popup
   //   SetFocus();
   //   theApp.GetContextMenuManager()->ShowPopupMenu(IDM_BACKUP_POPUP, point.x, point.y, this);
   //}

   /// <summary>Display grey background when list is disabled.</summary>
   /// <param name="dc">The dc.</param>
   /// <returns></returns>
   BOOL BackupList::OnEraseBkgnd(CDC* dc)
   {
      ClientRect rc(this);
      
      Console << HERE << ENDL;

      dc->FillSolidRect(rc, GetSysColor(IsWindowEnabled() ? COLOR_WINDOW : COLOR_GRAYTEXT));
      return TRUE;
      //return CListBox::OnEraseBkgnd(pDC);
   }

   /// <summary>Updates window font.</summary>
   /// <param name="uFlags">The u flags.</param>
   /// <param name="lpszSection">The LPSZ section.</param>
   void BackupList::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
   {
      // Update font
      SetFont(&theApp.ToolWindowFont);
   }
   
   // ------------------------------- PRIVATE METHODS ------------------------------
   
   

NAMESPACE_END2(GUI,Controls)

