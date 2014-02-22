
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
	   ON_COMMAND(ID_PROJECT_ADD_FOLDER, OnCreateFolder)
	   ON_COMMAND(ID_PROJECT_ADD_FILE, OnAddExisting)
      ON_COMMAND(ID_PROJECT_OPEN, OnOpenItem)
	   ON_COMMAND(ID_PROJECT_RENAME, OnRenameItem)
	   ON_COMMAND(ID_PROJECT_REMOVE, OnRemoveItem)
	   ON_COMMAND(ID_PROJECT_DELETE, OnDeleteItem)
	   ON_COMMAND(ID_PROJECT_PROPERTIES, OnViewProperties)
	   ON_WM_PAINT()
	   ON_WM_SETFOCUS()
   END_MESSAGE_MAP()

   // -------------------------------- CONSTRUCTION --------------------------------

   CProjectWnd::CProjectWnd() : fnProjectLoaded(ProjectDocument::Loaded.Register(this, &CProjectWnd::OnProjectLoaded))
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


   int CProjectWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
   {
      try
      {
	      if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		      throw Win32Exception(HERE, L"Unable to create project window base pane");

	      CRect rectDummy;
	      rectDummy.SetRectEmpty();

	      // Create view:
	      const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_EDITLABELS | TVS_HASLINES | TVS_HASBUTTONS; //TVS_LINESATROOT | 

	      if (!TreeView.Create(dwViewStyle, rectDummy, this, 4))
            throw Win32Exception(HERE, L"Unable to create project window tree view");
	      
	      // ImageList:
	      if (!Images.Create(IDB_PROJECT_ICONS, 16, 0, RGB(255, 0, 255)))
            throw Win32Exception(HERE, L"Unable to create project window imageList");
	      TreeView.SetImageList(&Images, TVSIL_NORMAL);

         // Toolbar:
         if (!Toolbar.Create(this, IDR_PROJECT))
            throw Win32Exception(HERE, L"Unable to create project window toolbar");

	      // Populate
	      TreeView.Populate();
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

   /// <summary>Called when project loaded or unloaded.</summary>
   void CProjectWnd::OnProjectLoaded()
   {
      TreeView.Populate();
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



   




   /// <summary>Insert new folder as child of selected item</summary>
   void CProjectWnd::OnCreateFolder()
   {
      try
      {
	      // Get selected folder, if any
         auto folder = dynamic_cast<ProjectFolderItem*>(TreeView.SelectedItem);
         if (!folder)
            return;

         // Feedback
         Console << Cons::UserAction << "Adding new folder to project item " << Cons::Yellow << folder->Name << ENDL;

         // Insert new folder
         ProjectDocument::GetActive()->AddFolder(L"New Folder", folder);
      }
      catch (ExceptionBase& e) {
         theApp.ShowError(HERE, e);
      }
   }

   /// <summary>Display common dialog and insert file as child of selected item.</summary>
   void CProjectWnd::OnAddExisting()
   {
      auto filter = L"Uncompressed Files (*.xml)|*.xml|" 
                    L"Compressed Files (*.pck)|*.pck|" 
                    L"All files (*.*)|*.*||";

      try
      {
         // Get selected folder
         auto folder = dynamic_cast<ProjectFolderItem*>(TreeView.SelectedItem);
         if (!folder)
            return;

         // Query for file
	      CFileDialog dlg(TRUE, L".xml", L"", OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST, filter, this, 0, TRUE);
         if (dlg.DoModal() == IDOK)
         {
            IO::Path path = (LPCWSTR)dlg.GetPathName();

            // Feedback
            Console << Cons::UserAction << "Adding " << path << " to project item " << Cons::Yellow << folder->Name << "...";

            // Attempt to insert
            bool added = ProjectDocument::GetActive()->AddFile(path, folder);

            // Feedback
            Console << (added ? Cons::Success : Cons::Failure) << ENDL;
         }
      }
      catch (ExceptionBase& e) {
         theApp.ShowError(HERE, e);
      }
   }

   /// <summary>Load and display the selected item.</summary>
   void CProjectWnd::OnOpenItem()
   {
      try
      {
         // Get selected file, if any
         auto file = dynamic_cast<ProjectFileItem*>(TreeView.SelectedItem);
         if (!file)
            return;

         // Feedback
         Console << Cons::UserAction << "Opening project item: " << Cons::Yellow << file->Name << ENDL;

         // Open document
         theApp.OpenDocumentFile(file->FullPath.c_str(), TRUE);
      }
      catch (ExceptionBase& e) {
         theApp.ShowError(HERE, e);
      }
   }

   /// <summary>Rename the selected item.</summary>
   void CProjectWnd::OnRenameItem()
   {
	   // Rename selected item
      TreeView.EditLabel(TreeView.GetSelectedItem());
   }

   /// <summary>Removes the selected item.</summary>
   void CProjectWnd::OnRemoveItem()
   {
      try
      {
         // Get selected unfixed file/folder
         auto item = TreeView.SelectedItem;
         if (!item || item->Fixed)
            return;

         // Feedback
         Console << Cons::UserAction << "Removing project item: " << Cons::Yellow << item->Name << ENDL;

         // Remove item
         ProjectDocument::GetActive()->RemoveItem(item);
      }
      catch (ExceptionBase& e) {
         theApp.ShowError(HERE, e);
      }
   }

   /// <summary>Removes and deletes the selected item.</summary>
   void CProjectWnd::OnDeleteItem()
   {
      try
      {
         // Get selected unfixed file/folder
         auto item = TreeView.SelectedItem;
         if (!item || item->Fixed)
            return;

         // Feedback
         Console << Cons::UserAction << "Deleting project item: " << Cons::Yellow << item->Name << ENDL;

         // Remove item
         auto ptr = ProjectDocument::GetActive()->RemoveItem(item);

         // File: Ensure path exists, then delete
         if (auto file = dynamic_cast<ProjectFileItem*>(item))
            if (file->FullPath.Exists() && !DeleteFile(file->FullPath.c_str()))
               throw Win32Exception(HERE, GuiString(L"Unable to delete '%s'", file->FullPath.c_str()));
      }
      catch (ExceptionBase& e) {
         theApp.ShowError(HERE, e);
      }
   }

   /// <summary>TODO: Display properties</summary>
   void CProjectWnd::OnViewProperties()
   {
	   AfxMessageBox(_T("Properties...."));

   }

   // ------------------------------- PRIVATE METHODS ------------------------------

NAMESPACE_END2(GUI,Windows)

