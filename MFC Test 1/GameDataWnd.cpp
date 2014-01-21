#include "stdafx.h"
#include "GameDataWnd.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN(GUI)

   // --------------------------------- APP WIZARD ---------------------------------
  
   BEGIN_MESSAGE_MAP(CGameDataWnd, CDockablePane)
	   ON_WM_CREATE()
	   ON_WM_SIZE()
	   ON_WM_CONTEXTMENU()
	   ON_WM_PAINT()
	   ON_WM_SETFOCUS()
   END_MESSAGE_MAP()

   // -------------------------------- CONSTRUCTION --------------------------------

   CGameDataWnd::CGameDataWnd()
   {
   }


   CGameDataWnd::~CGameDataWnd()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   // ------------------------------ PROTECTED METHODS -----------------------------

   void CGameDataWnd::OnContextMenu(CWnd* pWnd, CPoint point)
   {
	   //CTreeCtrl* pWndTree = (CTreeCtrl*) &TreeView;
	   //ASSERT_VALID(pWndTree);

	   //if (pWnd != pWndTree)
	   //{
		  // CDockablePane::OnContextMenu(pWnd, point);
		  // return;
	   //}

	   //if (point != CPoint(-1, -1))
	   //{
		  // // Select clicked item:
		  // CPoint ptTree = point;
		  // pWndTree->ScreenToClient(&ptTree);

		  // UINT flags = 0;
		  // HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
		  // if (hTreeItem != NULL)
		  // {
			 //  pWndTree->SelectItem(hTreeItem);
		  // }
	   //}

	   //pWndTree->SetFocus();
	   //theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EXPLORER, point.x, point.y, this, TRUE);
   }


   int CGameDataWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
   {
	   if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		   return -1;

	   CRect rectDummy;
	   rectDummy.SetRectEmpty();

	   // Create view:
	   if (!ListView.Create(WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_OWNERDATA, rectDummy, this, 1))
	   {
		   TRACE0("Failed to create file view\n");
		   return -1;      // fail to create
	   }

	   // Load view images:
	   /*Images.Create(IDB_FILE_VIEW, 16, 0, RGB(255, 0, 255));
	   ListView.SetImageList(&Images, LVSIL_NORMAL);*/

	   // Resize
	   AdjustLayout();
	   return 0;
   }

   void CGameDataWnd::OnPaint()
   {
	   CPaintDC dc(this); 

	   CRect rc;
	   ListView.GetWindowRect(rc);
	   ScreenToClient(rc);

      // Draw border
	   rc.InflateRect(1, 1);
	   dc.Draw3dRect(rc, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
   }

   void CGameDataWnd::OnSetFocus(CWnd* pOldWnd)
   {
	   CDockablePane::OnSetFocus(pOldWnd);

	   ListView.SetFocus();
   }

   void CGameDataWnd::OnSize(UINT nType, int cx, int cy)
   {
	   CDockablePane::OnSize(nType, cx, cy);
	   AdjustLayout();
   }


   // ------------------------------- PRIVATE METHODS ------------------------------
   
   void CGameDataWnd::AdjustLayout()
   {
	   if (GetSafeHwnd() == NULL || (AfxGetMainWnd() != NULL && AfxGetMainWnd()->IsIconic()))
         return;

	   CRect rcClient;
	   GetClientRect(rcClient);

	   /*int barHeight = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	   m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), barHeight, SWP_NOACTIVATE | SWP_NOZORDER);
	   m_wndPropList.SetWindowPos(NULL, rectClient.left, rectClient.top + barHeight, rectClient.Width(), rectClient.Height() - barHeight, SWP_NOACTIVATE | SWP_NOZORDER);*/

      ListView.SetWindowPos(NULL, rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
   }
   
NAMESPACE_END(GUI)
