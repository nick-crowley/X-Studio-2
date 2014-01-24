#include "stdafx.h"
#include "LanguagePageView.h"
#include "ListView.h"
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

   #ifdef _DEBUG
   void LanguagePageView::AssertValid() const
   {
	   CListView::AssertValid();
   }
   
   void LanguagePageView::Dump(CDumpContext& dc) const
   {
	   CListView::Dump(dc);
   }

   LanguageDocument* LanguagePageView::GetDocument() const // non-debug version is inline
   {
	   ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(LanguageDocument)));
	   return (LanguageDocument*)m_pDocument;
   }
   #endif //_DEBUG

   /// <summary>Gets the currently selected page.</summary>
   /// <returns>Selected page if any, otherwise nullptr</returns>
   /// <exception cref="Logic::IndexOutOfRangeException">Selected item index is invalid</exception>
   LanguagePage*   LanguagePageView::GetSelectedPage() const
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
      // Iterate thru all pages in all library files
      for (auto& f : StringLib.Files)
         for (auto& pair : f.Pages)
         {
            auto& p = pair.second;

            // Create new page, if not already present
            if (!Library.Contains(p.ID))
               Library.insert(PageCollection::value_type(p.ID, LanguagePage(p.ID, p.Title, p.Description, p.Voiced)) );

            // Copy strings into page
            Library.Merge(const_cast<LanguagePage&>(p));
         }
   }

   /// <summary>Initialise listView and populate pages</summary>
   void LanguagePageView::OnInitialUpdate()
   {
      CListView::OnInitialUpdate();

      // Setup listView
      GetListCtrl().ModifyStyle(WS_BORDER, LVS_SINGLESEL);
      GetListCtrl().SetView(LV_VIEW_DETAILS);
      GetListCtrl().InsertColumn(0, L"ID", LVCFMT_LEFT, 60, 0);
      GetListCtrl().InsertColumn(1, L"Title", LVCFMT_LEFT, 100, 1);
      GetListCtrl().SetExtendedStyle(LVS_EX_FULLROWSELECT);
      GetListCtrl().EnableGroupView(TRUE);

      // Define groups
      for (UINT i = (UINT)PageGroup::DATA; i <= (UINT)PageGroup::USER; ++i)
      {
         LVGroup g(i, GuiString(i+IDS_FIRST_LANGUAGE_GROUP));
         GetListCtrl().InsertGroup(i, &g);
      }

      // Populate pages
      Populate();
   }


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
         
         // Add item {ID,Title}
         LVItem item(index++, GuiString(L"%d", p.ID), (UINT)p.GetGroup(), LVIF_TEXT | LVIF_GROUPID);
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
      
      // Raise SELECTION CHANGED
      SelectionChanged.Raise();
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



