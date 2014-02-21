
#include "stdafx.h"
#include "MainWnd.h"
#include "ProjectWnd.h"
#include "Application.h"
//#include "ProjectDocument.h"

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

   CProjectWnd::CProjectWnd() : fnProjectChanged(ProjectDocument::Changed.Register(this, &CProjectWnd::OnProjectChanged))
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

   void CProjectWnd::InsertItem(ProjectItem* item, HTREEITEM parent)
   {
      int icon = -1;

      // Choose item
      switch (item->Type)
      {
      case ProjectItemType::Folder:    icon = 0;  break;
      case ProjectItemType::File:      icon = 1;  break;
      case ProjectItemType::Variable:  icon = 2;  break;
      }

      // Add item
      HTREEITEM node = TreeView.InsertItem(item->Name.c_str(), icon, icon, parent);

      // Fixed: Display in bold
      if (item->Fixed)
	      TreeView.SetItemState(node, TVIS_BOLD, TVIS_BOLD);

      // Folder: Display children
      for (auto child : item->Children)
         InsertItem(child, node);

      // Fixed: Expand
      TreeView.Expand(node, TVE_EXPAND);
   }

   void CProjectWnd::Populate()
   {
      // Clear
      TreeView.DeleteAllItems();
      
      // Repopulate
      if (auto doc = ProjectDocument::GetActive())
      {
         // Root: Project Name
         HTREEITEM root = TreeView.InsertItem(doc->GetTitle(), 0, 0);
	      TreeView.SetItemState(root, TVIS_BOLD, TVIS_BOLD);

         // Items: Populate recursively
         for (auto item : doc->Project.Items)
            InsertItem(item, root);

         // Expand root
         TreeView.Expand(root, TVE_EXPAND);
      }
   }

   int CProjectWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
   {
      try
      {
	      if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		      throw Win32Exception(HERE, L"Unable to create base pane");

	      CRect rectDummy;
	      rectDummy.SetRectEmpty();

	      // Create view:
	      const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_HASBUTTONS; //TVS_LINESATROOT | 

	      if (!TreeView.Create(dwViewStyle, rectDummy, this, 4))
            throw Win32Exception(HERE, L"Unable to create tree view");
	      
	      // ImageList:
	      Images.Create(IDB_PROJECT_ICONS, 16, 0, RGB(255, 0, 255));
	      TreeView.SetImageList(&Images, TVSIL_NORMAL);

         // Toolbar:
         Toolbar.Create(this, IDR_PROJECT);

	      // Populate
	      Populate();
	      AdjustLayout();

	      return 0;
      }
      catch (ExceptionBase& e) {
         Console.Log(HERE, e);
         return -1;
      }
   }

   void CProjectWnd::OnChangeVisualStyle()
   {
	   //Toolbar.CleanUpLockedImages();
	   //Toolbar.LoadBitmap(IDR_PROJECT, 0, 0, TRUE /* Locked */);

	   //Images.DeleteImageList();

	   //CBitmap bmp;
	   //if (!bmp.LoadBitmap(IDB_PROJECT_ICONS))
	   //{
		  // TRACE(_T("Can't load bitmap: %x\n"), IDB_PROJECT_ICONS);
		  // ASSERT(FALSE);
		  // return;
	   //}

	   //BITMAP bmpObj;
	   //bmp.GetBitmap(&bmpObj);

	   //Images.Create(16, bmpObj.bmHeight, ILC_MASK|ILC_COLOR24, 0, 0);
	   //Images.Add(&bmp, RGB(255, 0, 255));

	   //TreeView.SetImageList(&Images, TVSIL_NORMAL);
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

   void CProjectWnd::OnPaint()
   {
	   CPaintDC dc(this); // device context for painting

	   CRect rectTree;
	   TreeView.GetWindowRect(rectTree);
	   ScreenToClient(rectTree);

	   rectTree.InflateRect(1, 1);
	   dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
   }

   void CProjectWnd::OnProjectChanged()
   {
      Populate();
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

   
   void CProjectWnd::OnProperties()
   {
	   AfxMessageBox(_T("Properties...."));

   }

   // ------------------------------- PRIVATE METHODS ------------------------------

NAMESPACE_END2(GUI,Windows)

