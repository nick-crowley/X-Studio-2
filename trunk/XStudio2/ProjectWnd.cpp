
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
	   ON_COMMAND(ID_PROJECT_ADD_FOLDER, OnCommand_CreateFolder)
	   ON_COMMAND(ID_PROJECT_ADD_FILE, OnCommand_AddExisting)
      ON_COMMAND(ID_PROJECT_OPEN, OnCommand_OpenItem)
	   ON_COMMAND(ID_PROJECT_RENAME, OnCommand_RenameItem)
	   ON_COMMAND(ID_PROJECT_REMOVE, OnCommand_RemoveItem)
	   ON_COMMAND(ID_PROJECT_DELETE, OnCommand_DeleteItem)
	   ON_COMMAND(ID_PROJECT_PROPERTIES, OnCommand_ViewProperties)
      ON_UPDATE_COMMAND_UI_RANGE(ID_PROJECT_OPEN, ID_PROJECT_PROPERTIES, OnQueryCommand)
      ON_NOTIFY(NM_DBLCLK, IDC_PROJECT_TREE, OnTreeView_DoubleClick)
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

   /// <summary>Creates the window.</summary>
   /// <param name="parent">The parent.</param>
   /// <exception cref="Logic::Win32Exception">Unable to create window</exception>
   void CProjectWnd::Create(CWnd* parent)
   {
      DWORD style = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_RIGHT | CBRS_FLOAT_MULTI;

      // Create window
      if (!__super::Create(L"Project", parent, MainWnd::DefaultSize, TRUE, IDR_PROJECTVIEW, style))
         throw Win32Exception(HERE, L"Unable to create Project window");

      // Icon
      SetIcon(theApp.LoadIconW(IDR_PROJECTVIEW, ::GetSystemMetrics(SM_CXSMICON)), FALSE);

      // Dock left/right
      EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);
   }
   

   /// <summary>Translates custom accelerators for this window.</summary>
   /// <param name="pMsg">The MSG.</param>
   /// <returns></returns>
   BOOL CProjectWnd::PreTranslateMessage(MSG* pMsg)
   {
      if (Accelerators != nullptr && pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST)
	      return ::TranslateAccelerator(m_hWnd, Accelerators, pMsg);

      return CDockablePane::PreTranslateMessage(pMsg);
   }

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

   /// <summary>Determines whether window/treeview has focus.</summary>
   /// <returns></returns>
   /// <remarks>this was an experiment, it's no longer used</remarks>
   bool  CProjectWnd::HasFocus() const
   {
      return GetFocus() && (*GetFocus() == *this || *GetFocus() == TreeView);
   }


   /// <summary>Called when created.</summary>
   /// <param name="lpCreateStruct">The lp create structure.</param>
   /// <returns></returns>
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

	      if (!TreeView.Create(dwViewStyle, rectDummy, this, IDC_PROJECT_TREE))
            throw Win32Exception(HERE, L"Unable to create project window tree view");
	      
	      // ImageList:
	      if (!Images.Create(IDB_PROJECT_ICONS, 16, 0, RGB(255, 0, 255)))
            throw Win32Exception(HERE, L"Unable to create project window imageList");
	      TreeView.SetImageList(&Images, TVSIL_NORMAL);

         // Toolbar:
         if (!Toolbar.Create(this, IDR_PROJECTVIEW, L"Project"))
            throw Win32Exception(HERE, L"Unable to create project window toolbar");

         // Custom accelerators
         Accelerators = ::LoadAccelerators(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_PROJECTVIEW));

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


   /// <summary>App-wizard generated.</summary>
   void CProjectWnd::OnChangeVisualStyle()
   {
	   //Toolbar.CleanUpLockedImages();
	   //Toolbar.LoadBitmap(IDR_PROJECTVIEW, 0, 0, TRUE /* Locked */);

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
		   // Get position
		   CPoint ptTree = point;
		   pWndTree->ScreenToClient(&ptTree);

         // Select item
		   UINT flags = TVHT_ONITEM;
		   if (auto item = pWndTree->HitTest(ptTree, &flags))
            pWndTree->SelectItem(item);
	   }

	   pWndTree->SetFocus();
	   theApp.GetContextMenuManager()->ShowPopupMenu(IDM_PROJECT_POPUP, point.x, point.y, this, TRUE);
   }

   
   /// <summary>Display common dialog and insert file as child of selected item.</summary>
   void CProjectWnd::OnCommand_AddExisting()
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
   
   /// <summary>Insert new folder as child of selected item</summary>
   void CProjectWnd::OnCommand_CreateFolder()
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

   /// <summary>Removes and deletes the selected item.</summary>
   void CProjectWnd::OnCommand_DeleteItem()
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

   /// <summary>Load and display the selected item.</summary>
   void CProjectWnd::OnCommand_OpenItem()
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
   void CProjectWnd::OnCommand_RenameItem()
   {
	   // Rename selected item
      if (auto item = TreeView.GetSelectedItem())
         TreeView.EditLabel(item);
   }

   /// <summary>Removes the selected item.</summary>
   void CProjectWnd::OnCommand_RemoveItem()
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

   /// <summary>TODO: Display properties</summary>
   void CProjectWnd::OnCommand_ViewProperties()
   {
	   AfxMessageBox(_T("Properties...."));

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


   /// <summary>Set command states</summary>
   void CProjectWnd::OnQueryCommand(CCmdUI* pCmd)
   {
      bool State = true;

      // Require selection
      if (!TreeView.GetSelectedItem())
         State = false;
      
      else 
      {
         auto item = TreeView.SelectedItem;
         switch (pCmd->m_nID)
         {
         // Open: Require file
         case ID_PROJECT_OPEN:   
            State = item && item->Type == ProjectItemType::File; 
            break;

         // AddFile/Folder: Require folder
         case ID_PROJECT_ADD_FILE:    
         case ID_PROJECT_ADD_FOLDER:  
            State = item && item->Type == ProjectItemType::Folder;  
            break;

         // Rename: Not fixed folder
         case ID_PROJECT_RENAME:
            State = !item || !item->Fixed;
            break;

         // Remove: Not project + not fixed folder
         case ID_PROJECT_REMOVE:
            State = item && !item->Fixed;  
            break;

         // Delete: Require file   
         case ID_PROJECT_DELETE:   
            State = item && item->Type == ProjectItemType::File;    
            break;

         // Properties: Not folder
         case ID_PROJECT_PROPERTIES:  
            State = !item || item->Type != ProjectItemType::Folder;    
            break;
         }
      }

      // Enable
      pCmd->Enable(State ? TRUE : FALSE);
   }


   /// <summary>Set focus to the tree</summary>
   /// <param name="pOldWnd">The old WND.</param>
   void CProjectWnd::OnSetFocus(CWnd* pOldWnd)
   {
	   CDockablePane::OnSetFocus(pOldWnd);

	   TreeView.SetFocus();
   }


   /// <summary>Adjust layout.</summary>
   /// <param name="nType">Type of the n.</param>
   /// <param name="cx">The width.</param>
   /// <param name="cy">The height.</param>
   void CProjectWnd::OnSize(UINT nType, int cx, int cy)
   {
	   CDockablePane::OnSize(nType, cx, cy);
	   AdjustLayout();
   }


   /// <summary>Open the selected item on double click</summary>
   /// <param name="pNMHDR">The NMHDR.</param>
   /// <param name="pResult">The result.</param>
   void CProjectWnd::OnTreeView_DoubleClick(NMHDR* pNMHDR, LRESULT* pResult)
   {
      OnCommand_OpenItem();
      *pResult = 0;
   }
   

   // ------------------------------- PRIVATE METHODS ------------------------------

NAMESPACE_END2(GUI,Windows)

