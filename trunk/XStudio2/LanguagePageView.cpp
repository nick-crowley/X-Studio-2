#include "stdafx.h"
#include "LanguagePageView.h"
#include "LanguageEditView.h"
#include "PropertiesWnd.h"
#include "Helpers.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Views)

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNCREATE(LanguagePageView, CListView)

   BEGIN_MESSAGE_MAP(LanguagePageView, CListView)
      ON_WM_CONTEXTMENU()
      ON_WM_SIZE()
      ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, &LanguagePageView::OnItemStateChanged)
      ON_COMMAND(ID_EDIT_CUT, &LanguagePageView::OnCommandEditCut)
      ON_COMMAND(ID_EDIT_COPY, &LanguagePageView::OnCommandEditCopy)
      ON_COMMAND(ID_EDIT_PASTE, &LanguagePageView::OnCommandEditPaste)
      ON_COMMAND(ID_EDIT_CLEAR, &LanguagePageView::OnCommandEditClear)
      ON_COMMAND(ID_EDIT_INSERT, &LanguagePageView::OnCommandEditInsert)
      ON_COMMAND(ID_EDIT_SELECT_ALL, &LanguagePageView::OnCommandEditSelectAll)
      ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, &LanguagePageView::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, &LanguagePageView::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, &LanguagePageView::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, &LanguagePageView::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_EDIT_FIND, &LanguagePageView::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_EDIT_INSERT, &LanguagePageView::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, &LanguagePageView::OnQueryCommand)
      // Toolbar Commands
      ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_SOURCE, ID_VIEW_DISPLAY, &LanguagePageView::OnQueryMode)
      ON_UPDATE_COMMAND_UI_RANGE(ID_EDIT_BOLD, ID_EDIT_UNDERLINE, &LanguagePageView::OnQueryCommand)
      ON_UPDATE_COMMAND_UI_RANGE(ID_EDIT_LEFT, ID_EDIT_JUSTIFY, &LanguagePageView::OnQueryCommand)
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   LanguagePageView::LanguagePageView() : Accelerators(nullptr)
   {
   }

   LanguagePageView::~LanguagePageView()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   /// <summary>Gets the document.</summary>
   /// <returns></returns>
   LanguageDocument* LanguagePageView::GetDocument() const 
   {
#ifdef _DEBUG
	   return dynamic_cast<LanguageDocument*>(m_pDocument);
#else
      return reinterpret_cast<LanguageDocument*>(m_pDocument);
#endif
   }

   /// <summary>Gets the currently selected page.</summary>
   /// <returns>Selected page if any, otherwise nullptr</returns>
   /// <exception cref="Logic::IndexOutOfRangeException">Selected item index is invalid</exception>
   LanguagePage*   LanguagePageView::GetSelected() const
   {
      int item = GetListCtrl().GetNextItem(-1, LVNI_SELECTED);
      return item != -1 ? &GetDocument()->Content.FindByIndex(item) : nullptr;
   }
   

   /// <summary>Inserts a page item.</summary>
   /// <param name="index">Zero-based index.</param>
   /// <param name="page">The page.</param>
   /// <param name="display">Whether to ensure visible after insert</param>
   void LanguagePageView::InsertPage(UINT index, LanguagePageRef page, bool display)
   {
      // Define item
      LVItem item(index, GuiString(L"%d", page.ID), (UINT)page.GetGroup(), LVIF_TEXT | LVIF_GROUPID | LVIF_IMAGE);
      item.iImage = page.Voiced ? 1 : 0;

      // Add item.  Set title
      GetListCtrl().InsertItem(&item);
      GetListCtrl().SetItemText(item.iItem, 1, page.Title.c_str());

      // Ensure visible
      if (display)
      {
         GetListCtrl().EnsureVisible(index, FALSE);
         GetListCtrl().SetItemState(index, LVIS_SELECTED, LVIS_SELECTED);
      }
   }
   
   /// <summary>Translates custom accelerators for this window.</summary>
   /// <param name="pMsg">The MSG.</param>
   /// <returns></returns>
   BOOL LanguagePageView::PreTranslateMessage(MSG* pMsg)
   {
      if (Accelerators != nullptr && pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST)
	      return ::TranslateAccelerator(m_hWnd, Accelerators, pMsg);

      return __super::PreTranslateMessage(pMsg);
   }
   
   /// <summary>Removes a page.</summary>
   /// <param name="index">The index.</param>
   void LanguagePageView::RemovePage(UINT index)
   {
      GetListCtrl().EnsureVisible(index, FALSE);
      GetListCtrl().DeleteItem(index);
   }

   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Adjusts the layout.</summary>
   void  LanguagePageView::AdjustLayout()
   {
      // Destroyed/Minimised
	   if (!GetSafeHwnd() || theApp.IsMimized())
         return;
         
      CRect wnd;
      GetClientRect(wnd);

      // Stretch 2nd column over view
      GetListCtrl().SetColumnWidth(1, wnd.Width()-GetListCtrl().GetColumnWidth(0));
   }
   
   /// <summary>Called when activate view.</summary>
   /// <param name="bActivate">activated.</param>
   /// <param name="pActivateView">The activate view.</param>
   /// <param name="pDeactiveView">The deactive view.</param>
   void LanguagePageView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
   {
      // Show page properties (if any), otherwise document properties
      if (bActivate)
         CPropertiesWnd::Connect(GetDocument(), true);

      __super::OnActivateView(bActivate, pActivateView, pDeactiveView);
   }
   
   /// <summary>Called when context menu.</summary>
   /// <param name="pWnd">The WND.</param>
   /// <param name="point">The point.</param>
   void LanguagePageView::OnContextMenu(CWnd* pWnd, CPoint point)
   {
      CMenu menu;
	   menu.LoadMenu(IDM_STRINGVIEW_POPUP);

      // Create MFC custom menu
	   CMFCPopupMenu* customMenu = new CMFCPopupMenu;
		if (!menu.GetSubMenu(0) || !customMenu->Create(this, point.x, point.y, menu.GetSubMenu(0)->m_hMenu, FALSE, TRUE))
			return;

      // Show menu:
		((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(customMenu);
		UpdateDialogControls(this, FALSE);
	   SetFocus();
   }

   /// <summary>Initialise listView and populate pages</summary>
   void LanguagePageView::OnInitialUpdate()
   {
      CListView::OnInitialUpdate();

      try
      {
         // Icons
         Images.Create(IDB_LANGUAGE_ICONS, 16, 6, RGB(255,0,255));

         // Setup listView
         GetListCtrl().ModifyStyle(WS_BORDER, LVS_SHOWSELALWAYS|LVS_SINGLESEL);
         GetListCtrl().SetView(LV_VIEW_DETAILS);
         GetListCtrl().InsertColumn(0, L"ID", LVCFMT_LEFT, 60, 0);
         GetListCtrl().InsertColumn(1, L"Title", LVCFMT_LEFT, 100, 1);
         GetListCtrl().SetExtendedStyle(LVS_EX_FULLROWSELECT);
         GetListCtrl().EnableGroupView(TRUE);
         GetListCtrl().SetImageList(&Images, LVSIL_SMALL);

         // Define groups
         for (UINT i = (UINT)PageGroup::DATA; i <= (UINT)PageGroup::USER; ++i)
         {
            LVGroup g(i, GuiString(i+IDS_FIRST_LANGUAGE_GROUP));
            GetListCtrl().InsertGroup(i, &g);
         }

         // Custom accelerators
         Accelerators = ::LoadAccelerators(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_STRINGVIEW));

         // Listen for 'LIBRARY REBUILT'
         fnLibraryRebuilt = GetDocument()->LibraryRebuilt.Register(this, &LanguagePageView::OnLibraryRebuilt);
         fnPageUpdated = GetDocument()->PageUpdated.Register(this, &LanguagePageView::OnPageUpdated);

         // Populate pages
         Populate();
      }
      catch (ExceptionBase& e) {
         Console.Log(HERE, e);
      }
   }

   /// <summary>Populates pages.</summary>
   void LanguagePageView::Populate()
   {
      // Freeze window
      GetListCtrl().SetRedraw(FALSE);

      // Clear previous, if any
      GetListCtrl().DeleteAllItems();

      // Populate pages
      int index = 0;
      for (const auto& pair : GetDocument()->Content) 
         InsertPage(index++, pair.second, false);

      // Refresh
      GetListCtrl().SetRedraw(TRUE);
      GetListCtrl().UpdateWindow();
   }

   /// <summary>Re-populate in response to library being rebuilt</summary>
   void LanguagePageView::OnLibraryRebuilt()
   {
      Populate();
   }
   

   /// <summary>Raise the PAGE SELECTION CHANGED event</summary>
   /// <param name="pNMHDR">Item data</param>
   /// <param name="pResult">Notify result.</param>
   void LanguagePageView::OnItemStateChanged(NMHDR *pNMHDR, LRESULT *pResult)
   {
      LPNMLISTVIEW pItem = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
      
      try
      {
         // Selection (not focus) has changed
         if ((pItem->uOldState | pItem->uNewState) & LVIS_SELECTED)
            // Update document
            GetDocument()->SelectedPage = GetSelected();
      }
      catch (ExceptionBase& e) {
         Console.Log(HERE, e);
      }

      *pResult = 0;
   }

   /// <summary>Refreshes the currently selected item</summary>
   void LanguagePageView::OnPageUpdated()
   {
      try 
      {  
         REQUIRED(GetDocument()->SelectedPage);  

         // Update page
         UpdatePage(GetListCtrl().GetNextItem(-1, LVNI_SELECTED), *GetDocument()->SelectedPage);
      }
      catch (ExceptionBase& e) { 
         Console.Log(HERE, e);
      }
   }
   
   /// <summary>Performs a menu command</summary>
   /// <param name="nID">Command identifier.</param>
   void LanguagePageView::OnPerformCommand(UINT nID)
   {
      try 
      {
         switch (nID)
         {
         // Cut/Copy/Paste/Delete
         case ID_EDIT_CUT:    GetDocument()->Execute(new CutSelectedPage(*GetDocument()));       break;
         case ID_EDIT_COPY:   GetDocument()->Execute(new CopySelectedPage(*GetDocument()));      break;
         case ID_EDIT_PASTE:  GetDocument()->Execute(new PastePage(*GetDocument()));             break;
         case ID_EDIT_CLEAR:  GetDocument()->Execute(new DeleteSelectedPage(*GetDocument()));    break;
         case ID_EDIT_INSERT: GetDocument()->Execute(new InsertNewPage(*GetDocument()));         break;

         // Select All
         //case ID_EDIT_SELECT_ALL:  GetListCtrl().SetItemState(-1, LVIS_SELECTED, LVIS_SELECTED);   break;
         }
      }
      catch (ExceptionBase& e) {
         theApp.ShowError(HERE, e);
      }
   }
   

   /// <summary>Queries the state of a menu command.</summary>
   /// <param name="pCmdUI">The command UI.</param>
   void LanguagePageView::OnQueryCommand(CCmdUI* pCmdUI)
   {
      bool state = false;

      // Query relevant states
      bool selection = GetDocument()->SelectedPage != nullptr,
           library   = GetDocument()->Virtual,
           hasFocus  = ::GetFocus() == GetListCtrl();

      // Disable-all if tool window is focused
      if (hasFocus)
         switch (pCmdUI->m_nID)
         {
         // Non-modifying
         case ID_EDIT_COPY:        state = selection;    break;
         case ID_EDIT_SELECT_ALL:  state = true;         break;
         case ID_EDIT_FIND:        state = false;        break;

         // Modifying
         case ID_EDIT_CLEAR:  
         case ID_EDIT_CUT:         state = !library && selection;                        break;
         case ID_EDIT_PASTE:       state = !library && theClipboard.HasLanguagePage();   break;
         case ID_EDIT_INSERT:      state = !library;                                     break;    

         // Formatting: Unchecked/Disabled
         case ID_EDIT_BOLD:       
         case ID_EDIT_ITALIC:     
         case ID_EDIT_UNDERLINE:  
         // Alignment: Unchecked/Disabled
         case ID_EDIT_LEFT:       
         case ID_EDIT_RIGHT:      
         case ID_EDIT_CENTRE:     
         case ID_EDIT_JUSTIFY:    
            break;
         }

      // Set state
      pCmdUI->Enable(state ? TRUE : FALSE);
      pCmdUI->SetCheck(FALSE);
   }

   /// <summary>Queries the state of the editor mode toolbar commands.</summary>
   /// <param name="pCmdUI">The command UI.</param>
   void LanguagePageView::OnQueryMode(CCmdUI* pCmdUI)
   {
      // Enable mode switch, Delegate to Edit View
      GetDocument()->GetView<LanguageEditView>()->OnQueryMode(pCmdUI);
   }

   /// <summary>Adjusts layout</summary>
   /// <param name="nType">Type of the resize</param>
   /// <param name="cx">The new width</param>
   /// <param name="cy">The new height</param>
   void LanguagePageView::OnSize(UINT nType, int cx, int cy)
   {
      __super::OnSize(nType, cx, cy);
      AdjustLayout();
   }
   

   /// <summary>Updates a page.</summary>
   /// <param name="index">The index.</param>
   /// <param name="page">Page data.</param>
   void LanguagePageView::UpdatePage(UINT index, LanguagePageRef page)
   {
      // Validate index
      if (index >= (UINT)GetListCtrl().GetItemCount())
         throw IndexOutOfRangeException(HERE, index, GetListCtrl().GetItemCount());

      // Define 1st sub-item
      LVItem item(index, GuiString(L"%d", page.ID), (UINT)page.GetGroup(), LVIF_TEXT | LVIF_GROUPID | LVIF_IMAGE);
      item.iImage = page.Voiced ? 1 : 0;

      // Set ID/Icon/Text
      GetListCtrl().SetItem(&item);
      GetListCtrl().SetItemText(index, 1, page.Title.c_str());
   }

   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
NAMESPACE_END2(GUI,Views)



