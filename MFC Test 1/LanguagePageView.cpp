#include "stdafx.h"
#include "LanguagePageView.h"
#include "ListView.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Views)

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNCREATE(LanguagePageView, CListView)

   BEGIN_MESSAGE_MAP(LanguagePageView, CListView)
      ON_WM_SIZE()
      ON_NOTIFY_REFLECT(LVN_ITEMACTIVATE, &LanguagePageView::OnLvnItemActivate)
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
   
   // ------------------------------ PROTECTED METHODS -----------------------------
   
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

   

   void LanguagePageView::OnLvnItemActivate(NMHDR *pNMHDR, LRESULT *pResult)
   {
      LPNMITEMACTIVATE pItem = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
      
      Console << L"User activated language page " << pItem->iItem << ENDL;

      // Raise PAGE_CLICK
      PageClick.Raise(GetDocument()->Content.Pages.FindByIndex(pItem->iItem));

      // Return
      *pResult = 0;
   }

   void LanguagePageView::OnSize(UINT nType, int cx, int cy)
   {
      CListView::OnSize(nType, cx, cy);
      AdjustLayout();
   }
   
   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
NAMESPACE_END2(GUI,Views)



