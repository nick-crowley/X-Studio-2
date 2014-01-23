#include "stdafx.h"
#include "LanguageStringView.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Views)

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNCREATE(LanguageStringView, CListView)

   BEGIN_MESSAGE_MAP(LanguageStringView, CListView)
      ON_WM_SIZE()
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   LanguageStringView::LanguageStringView()
   {
   }

   LanguageStringView::~LanguageStringView()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   #ifdef _DEBUG
   void LanguageStringView::AssertValid() const
   {
	   CListView::AssertValid();
   }
   
   void LanguageStringView::Dump(CDumpContext& dc) const
   {
	   CListView::Dump(dc);
   }

   LanguageDocument* LanguageStringView::GetDocument() const // non-debug version is inline
   {
	   ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(LanguageDocument)));
	   return (LanguageDocument*)m_pDocument;
   }
   #endif //_DEBUG
   
   // ------------------------------ PROTECTED METHODS -----------------------------
   
   void  LanguageStringView::AdjustLayout()
   {
      // Destroyed/Minimised
	   if (GetSafeHwnd() == nullptr || (AfxGetMainWnd() != nullptr && AfxGetMainWnd()->IsIconic()))
         return;
         
      CRect wnd;
      GetClientRect(wnd);

      // TODO: Layout code
   }

   
   void LanguageStringView::OnInitialUpdate()
   {
      CListView::OnInitialUpdate();

      // Setup listView
      GetListCtrl().SetView(LV_VIEW_DETAILS);
      GetListCtrl().InsertColumn(0, L"ID", LVCFMT_LEFT, 60, 0);
      GetListCtrl().InsertColumn(1, L"Text", LVCFMT_LEFT, 240, 1);
      GetListCtrl().SetExtendedStyle(LVS_EX_FULLROWSELECT);
   }

   
   void LanguageStringView::OnSize(UINT nType, int cx, int cy)
   {
      CListView::OnSize(nType, cx, cy);
      AdjustLayout();
   }
   

   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
NAMESPACE_END2(GUI,Views)

