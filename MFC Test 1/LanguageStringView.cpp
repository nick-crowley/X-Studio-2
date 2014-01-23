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
   
   void LanguageStringView::OnSize(UINT nType, int cx, int cy)
   {
      CListView::OnSize(nType, cx, cy);
      AdjustLayout();
   }
   
   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
NAMESPACE_END2(GUI,Views)
