#include "stdafx.h"
#include "LanguagePageView.h"
#include "ListView.h"

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
      return item != -1 ? &GetDocument()->Content.Pages.FindByIndex(item) : nullptr;
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

      // TODO: Layout code
      //GetListCtrl().SetColumnWidth(1, wnd.Width()-GetListCtrl().GetColumnWidth(0));
      GetListCtrl().SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
   }
   
   /// <summary>Initialise listView and populate pages</summary>
   void LanguagePageView::OnInitialUpdate()
   {
      CListView::OnInitialUpdate();

      // Setup listView
      GetListCtrl().SetView(LV_VIEW_DETAILS);
      GetListCtrl().InsertColumn(0, L"ID", LVCFMT_LEFT, 60, 0);
      GetListCtrl().InsertColumn(1, L"Title", LVCFMT_LEFT, 100, 1);
      GetListCtrl().SetExtendedStyle(LVS_EX_FULLROWSELECT);
      //GetListCtrl().EnableGroupView(TRUE);

      // Populate pages
      UINT item = 0;
      for (const auto& pair : GetDocument()->Content.Pages)
      {
         const LanguagePage& p = pair.second;
         
         // Add item {ID,Title}
         item = GetListCtrl().InsertItem(item++, GuiString(L"%d", p.ID).c_str(), 0);
         GetListCtrl().SetItem(item, 1, LVIF_TEXT, p.Title.c_str(), 0, NULL, NULL, NULL);
      }
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



