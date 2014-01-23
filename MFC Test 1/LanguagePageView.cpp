#include "stdafx.h"
#include "LanguagePageView.h"
#include "ListView.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Views)

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNCREATE(LanguagePageView, CListView)

   BEGIN_MESSAGE_MAP(LanguagePageView, CListView)
      ON_WM_SIZE()
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
         item = GetListCtrl().InsertItem(item, GuiString(L"%d", p.ID).c_str(), 0);
         GetListCtrl().SetItem(item, 1, LVIF_TEXT, p.Title.c_str(), 0, NULL, NULL, NULL);
      }
   }

   void LanguagePageView::OnSize(UINT nType, int cx, int cy)
   {
      CListView::OnSize(nType, cx, cy);
      AdjustLayout();
   }
   
   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
NAMESPACE_END2(GUI,Views)


