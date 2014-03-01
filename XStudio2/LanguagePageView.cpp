#include "stdafx.h"
#include "LanguagePageView.h"
#include "PropertiesWnd.h"
#include "Helpers.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Views)

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNCREATE(LanguagePageView, CListView)

   BEGIN_MESSAGE_MAP(LanguagePageView, CListView)
      ON_WM_SIZE()
      ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, &LanguagePageView::OnItemStateChanged)
      ON_COMMAND_RANGE(ID_EDIT_CLEAR, ID_EDIT_CLEAR, &LanguagePageView::OnPerformCommand)
      ON_COMMAND_RANGE(ID_EDIT_CUT, ID_EDIT_CUT, &LanguagePageView::OnPerformCommand)
      ON_COMMAND_RANGE(ID_EDIT_COPY, ID_EDIT_COPY, &LanguagePageView::OnPerformCommand)
      ON_COMMAND_RANGE(ID_EDIT_PASTE, ID_EDIT_PASTE, &LanguagePageView::OnPerformCommand)
      ON_COMMAND_RANGE(ID_EDIT_SELECT_ALL, ID_EDIT_SELECT_ALL, &LanguagePageView::OnPerformCommand)
      ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, &LanguagePageView::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, &LanguagePageView::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, &LanguagePageView::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, &LanguagePageView::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, &LanguagePageView::OnQueryCommand)
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   LanguagePageView::LanguagePageView()
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
      return item != -1 ? &GetDocument()->GetContent().FindByIndex(item) : nullptr;
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
   
   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Adjusts the layout.</summary>
   void  LanguagePageView::AdjustLayout()
   {
      // Destroyed/Minimised
	   if (GetSafeHwnd() == nullptr || (AfxGetMainWnd() != nullptr && AfxGetMainWnd()->IsIconic()))
         return;
         
      CRect wnd;
      GetClientRect(wnd);

      // Stretch 2nd column over view
      GetListCtrl().SetColumnWidth(1, wnd.Width()-GetListCtrl().GetColumnWidth(0));
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

      // Populate pages
      int index = 0;
      for (const auto& pair : GetDocument()->GetContent()) 
      {
         const LanguagePage& p = pair.second;
         
         // Define item
         LVItem item(index++, GuiString(L"%d", p.ID), (UINT)p.GetGroup(), LVIF_TEXT | LVIF_GROUPID | LVIF_IMAGE);
         item.iImage = p.Voiced ? 1 : 0;

         // Add item.  Set title
         GetListCtrl().InsertItem(&item);
         GetListCtrl().SetItemText(item.iItem, 1, p.Title.c_str());
      }

      // Refresh
      GetListCtrl().SetRedraw(TRUE);
      GetListCtrl().UpdateWindow();
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

   /// <summary>Performs a menu command</summary>
   /// <param name="nID">Command identifier.</param>
   void LanguagePageView::OnPerformCommand(UINT nID)
   {
      AfxMessageBox(L"LanguagePageView::OnPerformCommand");

      try 
      {
         switch (nID)
         {
         // TODO:
         case ID_EDIT_COPY:   
         case ID_EDIT_CUT:    
         case ID_EDIT_PASTE:  
         case ID_EDIT_SELECT_ALL:
            break;

         // TODO: Remove selected
         case ID_EDIT_CLEAR: 
            //GetDocument()->Execute(new RemoveSelectedString(*this, *GetDocument()));
            break;
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

      switch (pCmdUI->m_nID)
      {
      // Require selection
      case ID_EDIT_CLEAR: 
      case ID_EDIT_COPY:   
      case ID_EDIT_CUT:    
         state = (GetDocument()->SelectedPage != nullptr);  
         break;
     
      // Always enabled
      case ID_EDIT_PASTE:  
      case ID_EDIT_SELECT_ALL:
         state = true;  
         break;
      }

      // Set state
      pCmdUI->Enable(state ? TRUE : FALSE);
      pCmdUI->SetCheck(FALSE);
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
   
   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
NAMESPACE_END2(GUI,Views)



