
#include "stdafx.h"
#include "MainWnd.h"
#include "ProjectWnd.h"
#include "Application.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)


   // --------------------------------- APP WIZARD ---------------------------------
  
   BEGIN_MESSAGE_MAP(CProjectWnd, CDockablePane)
	   ON_WM_CREATE()
	   ON_WM_SIZE()
	   ON_WM_CONTEXTMENU()
	   ON_COMMAND(ID_PROPERTIES, OnProperties)
	   ON_COMMAND(ID_OPEN, OnFileOpen)
	   ON_COMMAND(ID_OPEN_WITH, OnFileOpenWith)
	   ON_COMMAND(ID_DUMMY_COMPILE, OnDummyCompile)
	   ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	   ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	   ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	   ON_WM_PAINT()
	   ON_WM_SETFOCUS()
   END_MESSAGE_MAP()

   // -------------------------------- CONSTRUCTION --------------------------------

   CProjectWnd::CProjectWnd()
   {
   }

   CProjectWnd::~CProjectWnd()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   // ------------------------------ PROTECTED METHODS -----------------------------

   /// <summary>Adjusts the layout.</summary>
   void CProjectWnd::AdjustLayout()
   {
	   if (GetSafeHwnd() == NULL)
	   {
		   return;
	   }

	   CRect rectClient;
	   GetClientRect(rectClient);

	   int cyTlb = Toolbar.CalcFixedLayout(FALSE, TRUE).cy;

	   Toolbar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	   TreeView.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
   }

   void CProjectWnd::FillFileView()
   {
	   HTREEITEM hRoot = TreeView.InsertItem(_T("FakeApp files"), 0, 0);
	   TreeView.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);

	   HTREEITEM hSrc = TreeView.InsertItem(_T("FakeApp Source Files"), 0, 0, hRoot);

	   TreeView.InsertItem(_T("FakeApp.cpp"), 1, 1, hSrc);
	   TreeView.InsertItem(_T("FakeApp.rc"), 1, 1, hSrc);
	   TreeView.InsertItem(_T("FakeAppDoc.cpp"), 1, 1, hSrc);
	   TreeView.InsertItem(_T("FakeAppView.cpp"), 1, 1, hSrc);
	   TreeView.InsertItem(_T("MainFrm.cpp"), 1, 1, hSrc);
	   TreeView.InsertItem(_T("StdAfx.cpp"), 1, 1, hSrc);

	   HTREEITEM hInc = TreeView.InsertItem(_T("FakeApp Header Files"), 0, 0, hRoot);

	   TreeView.InsertItem(_T("FakeApp.h"), 2, 2, hInc);
	   TreeView.InsertItem(_T("FakeAppDoc.h"), 2, 2, hInc);
	   TreeView.InsertItem(_T("FakeAppView.h"), 2, 2, hInc);
	   TreeView.InsertItem(_T("Resource.h"), 2, 2, hInc);
	   TreeView.InsertItem(_T("MainFrm.h"), 2, 2, hInc);
	   TreeView.InsertItem(_T("StdAfx.h"), 2, 2, hInc);

	   HTREEITEM hRes = TreeView.InsertItem(_T("FakeApp Resource Files"), 0, 0, hRoot);

	   TreeView.InsertItem(_T("FakeApp.ico"), 2, 2, hRes);
	   TreeView.InsertItem(_T("FakeApp.rc2"), 2, 2, hRes);
	   TreeView.InsertItem(_T("FakeAppDoc.ico"), 2, 2, hRes);
	   TreeView.InsertItem(_T("FakeToolbar.bmp"), 2, 2, hRes);

	   TreeView.Expand(hRoot, TVE_EXPAND);
	   TreeView.Expand(hSrc, TVE_EXPAND);
	   TreeView.Expand(hInc, TVE_EXPAND);
   }

   int CProjectWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
   {
	   if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		   return -1;

	   CRect rectDummy;
	   rectDummy.SetRectEmpty();

	   // Create view:
	   const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;

	   if (!TreeView.Create(dwViewStyle, rectDummy, this, 4))
	   {
		   TRACE0("Failed to create file view\n");
		   return -1;      // fail to create
	   }

	   // Load view images:
	   Images.Create(IDB_PROJECT_ICONS, 16, 0, RGB(255, 0, 255));
	   TreeView.SetImageList(&Images, TVSIL_NORMAL);

	   Toolbar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROJECT);
	   Toolbar.LoadToolBar(IDR_PROJECT, 0, 0, TRUE /* Is locked */);

	   OnChangeVisualStyle();

	   Toolbar.SetPaneStyle(Toolbar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);

	   Toolbar.SetPaneStyle(Toolbar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	   Toolbar.SetOwner(this);

	   // All commands will be routed via this control , not via the parent frame:
	   Toolbar.SetRouteCommandsViaFrame(FALSE);

	   // Fill in some static tree view data (dummy code, nothing magic here)
	   FillFileView();
	   AdjustLayout();

	   return 0;
   }

   void CProjectWnd::OnChangeVisualStyle()
   {
	   Toolbar.CleanUpLockedImages();
	   Toolbar.LoadBitmap(IDR_PROJECT, 0, 0, TRUE /* Locked */);

	   Images.DeleteImageList();

	   CBitmap bmp;
	   if (!bmp.LoadBitmap(IDB_PROJECT_ICONS))
	   {
		   TRACE(_T("Can't load bitmap: %x\n"), IDB_PROJECT_ICONS);
		   ASSERT(FALSE);
		   return;
	   }

	   BITMAP bmpObj;
	   bmp.GetBitmap(&bmpObj);

	   Images.Create(16, bmpObj.bmHeight, ILC_MASK|ILC_COLOR24, 0, 0);
	   Images.Add(&bmp, RGB(255, 0, 255));

	   TreeView.SetImageList(&Images, TVSIL_NORMAL);
   }

   void CProjectWnd::OnContextMenu(CWnd* pWnd, CPoint point)
   {
	   CTreeCtrl* pWndTree = (CTreeCtrl*) &TreeView;
	   ASSERT_VALID(pWndTree);

	   if (pWnd != pWndTree)
	   {
		   CDockablePane::OnContextMenu(pWnd, point);
		   return;
	   }

	   if (point != CPoint(-1, -1))
	   {
		   // Select clicked item:
		   CPoint ptTree = point;
		   pWndTree->ScreenToClient(&ptTree);

		   UINT flags = 0;
		   HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
		   if (hTreeItem != NULL)
		   {
			   pWndTree->SelectItem(hTreeItem);
		   }
	   }

	   pWndTree->SetFocus();
	   theApp.GetContextMenuManager()->ShowPopupMenu(IDM_PROJECT_POPUP, point.x, point.y, this, TRUE);
   }

   void CProjectWnd::OnFileOpen()
   {
	   // TODO: Add your command handler code here
   }

   void CProjectWnd::OnFileOpenWith()
   {
	   // TODO: Add your command handler code here
   }

   void CProjectWnd::OnDummyCompile()
   {
	   // TODO: Add your command handler code here
   }

   void CProjectWnd::OnEditCut()
   {
	   // TODO: Add your command handler code here
   }

   void CProjectWnd::OnEditCopy()
   {
	   // TODO: Add your command handler code here
   }

   void CProjectWnd::OnEditClear()
   {
	   // TODO: Add your command handler code here
   }

   void CProjectWnd::OnPaint()
   {
	   CPaintDC dc(this); // device context for painting

	   CRect rectTree;
	   TreeView.GetWindowRect(rectTree);
	   ScreenToClient(rectTree);

	   rectTree.InflateRect(1, 1);
	   dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
   }

   void CProjectWnd::OnProperties()
   {
	   AfxMessageBox(_T("Properties...."));

   }

   void CProjectWnd::OnSetFocus(CWnd* pOldWnd)
   {
	   CDockablePane::OnSetFocus(pOldWnd);

	   TreeView.SetFocus();
   }

   void CProjectWnd::OnSize(UINT nType, int cx, int cy)
   {
	   CDockablePane::OnSize(nType, cx, cy);
	   AdjustLayout();
   }

   // ------------------------------- PRIVATE METHODS ------------------------------

NAMESPACE_END2(GUI,Windows)

