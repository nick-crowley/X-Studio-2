#include "stdafx.h"

#include "OutputList.h"
#include "Helpers.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/// <summary>User interface Controls</summary>
NAMESPACE_BEGIN2(GUI,Controls)

   // --------------------------------- APP WIZARD ---------------------------------
   
   BEGIN_MESSAGE_MAP(OutputList, CListCtrl)
      ON_WM_CREATE()
	   ON_WM_CONTEXTMENU()
	   ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	   ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	   ON_WM_WINDOWPOSCHANGING()
      ON_WM_SETTINGCHANGE()
      ON_WM_SIZE()
   END_MESSAGE_MAP()

   // -------------------------------- CONSTRUCTION --------------------------------

   OutputList::OutputList()
   {
   }

   OutputList::~OutputList()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   /// <summary>Creates the specified parent.</summary>
   /// <param name="parent">The parent.</param>
   /// <param name="style">The style.</param>
   /// <param name="nID">The n identifier.</param>
   /// <param name="images">The images.</param>
   void OutputList::Create(CWnd* parent, DWORD style, UINT nID, CImageList* images)
   {
      CRect rc;
      rc.SetRectEmpty();

      // Create
      if (!CreateEx(LVS_EX_FULLROWSELECT, style, rc, parent, nID))
         throw Win32Exception(HERE, L"Unable to create output window listview");

      // Set images + font
      SetImageList(images, LVSIL_SMALL);
      SetFont(&theApp.ToolWindowFont);
   }

   /// <summary>Add progress item to bottom of list.</summary>
   /// <param name="wp">The wp.</param>
   void OutputList::InsertItem(const WorkerProgress& wp)
   {
      // Create item
      LVItem item(GetItemCount(), wp.Text, NULL, LVIF_TEXT | LVIF_IMAGE | LVIF_INDENT);
      item.iImage = (UINT)wp.Type;
      item.iIndent = wp.Indent;

      // Insert as last item
      CListCtrl::InsertItem(&item);
      EnsureVisible(GetItemCount()-1, FALSE);
   }

   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Called when [create].</summary>
   /// <param name="lpCreateStruct">The lp create structure.</param>
   /// <returns></returns>
   int OutputList::OnCreate(LPCREATESTRUCT lpCreateStruct)
   {
	   if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		   return -1;

      // Set font
      SetFont(&theApp.ToolWindowFont);

      // Insert hidden column
      InsertColumn(0, L"Text");
      SetExtendedStyle(LVS_EX_FULLROWSELECT);
	   return 0;
   }

   /// <summary>Displays context menu.</summary>
   /// <param name="">The .</param>
   /// <param name="point">The point.</param>
   void OutputList::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
   {
      CMenu menu;
	   menu.LoadMenu(IDM_OUTPUT_POPUP);

      // Create MFC custom menu
	   CMFCPopupMenu* customMenu = new CMFCPopupMenu;
		if (!menu.GetSubMenu(0) || !customMenu->Create(this, point.x, point.y, menu.GetSubMenu(0)->m_hMenu, FALSE, TRUE))
			return;

      // Show
		((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(customMenu);
		UpdateDialogControls(this, FALSE);
	   SetFocus();
   }

   /// <summary>Copies item to clipboard</summary>
   void OutputList::OnEditCopy()
   {
	   MessageBox(_T("Not implemented"));
   }

   /// <summary>Clears all items</summary>
   void OutputList::OnEditClear()
   {
	   DeleteAllItems();
   }

   
   /// <summary>Updates window font.</summary>
   /// <param name="uFlags">The u flags.</param>
   /// <param name="lpszSection">The LPSZ section.</param>
   void OutputList::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
   {
      // Update font
      SetFont(&theApp.ToolWindowFont);
   }
   
   /// <summary>Adjust column width</summary>
   /// <param name="nType">Type of the n.</param>
   /// <param name="cx">The width.</param>
   /// <param name="cy">The height.</param>
   void OutputList::OnSize(UINT nType, int cx, int cy)
   {
      CListCtrl::OnSize(nType, cx, cy);

      // Adjust column
      CRect  rc;
      GetClientRect(&rc);
      SetColumnWidth(0, rc.Width());
   }


   // ------------------------------- PRIVATE METHODS ------------------------------
   
   

NAMESPACE_END2(GUI,Controls)


