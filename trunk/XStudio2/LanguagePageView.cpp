#include "stdafx.h"
#include "LanguagePageView.h"
#include "Helpers.h"
#include "Logic/StringLibrary.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Views)

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNCREATE(LanguagePageView, CListView)

   BEGIN_MESSAGE_MAP(LanguagePageView, CListView)
      ON_WM_SIZE()
      ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, &LanguagePageView::OnItemStateChanged)
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
      return item != -1 ? &GetDataSource().FindByIndex(item) : nullptr;
   }
   
   /// <summary>Retrieves page collection, either from document language file or static copy of library</summary>
   /// <returns></returns>
   LanguagePageView::PageCollection&   LanguagePageView::GetDataSource() const
   {
      return GetDocument()->Virtual ? const_cast<PageCollection&>(Library) : GetDocument()->Content.Pages;
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
   
   /// <summary>Builds a static copy of the string library.</summary>
   void LanguagePageView::GenerateLibrary()
   {
      // Copy each page (and strings) into from string library into static snapshot
      for (auto& file : StringLib.Files)
         for (auto& page : file)
            Library.Add(page);
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


   /// <summary>Populates this instance.</summary>
   void LanguagePageView::Populate()
   {
      // Generate static copy of string library
      if (GetDocument()->Virtual)
         GenerateLibrary();

      // Populate pages
      int index = 0;
      GetListCtrl().SetRedraw(FALSE);
      for (const auto& pair : GetDataSource()) 
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
         {
            // Update document
            GetDocument()->SelectedPage = GetSelected();

            // Raise SELECTION CHANGED
            SelectionChanged.Raise();
         }
      }
      catch (ExceptionBase& e) {
         Console.Log(HERE, e);
      }

      *pResult = 0;
   }

   /// <summary>Adjusts layout</summary>
   /// <param name="nType">Type of the resize</param>
   /// <param name="cx">The new width</param>
   /// <param name="cy">The new height</param>
   void LanguagePageView::OnSize(UINT nType, int cx, int cy)
   {
      CListView::OnSize(nType, cx, cy);
      AdjustLayout();
   }
   
   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
NAMESPACE_END2(GUI,Views)



