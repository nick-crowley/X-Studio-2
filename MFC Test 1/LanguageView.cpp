// LanguageView.cpp : implementation file
//

#include "stdafx.h"
#include "LanguageView.h"
#include "ListView.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN(GUI)

   // --------------------------------- APP WIZARD ---------------------------------
  
   // -------------------------------- CONSTRUCTION --------------------------------

   LanguageView::LanguageView() : CFormView(LanguageView::IDD)
   {

   }

   LanguageView::~LanguageView()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   IMPLEMENT_DYNCREATE(LanguageView, CFormView)

   BEGIN_MESSAGE_MAP(LanguageView, CFormView)
      ON_WM_SIZE()
   END_MESSAGE_MAP()

   // ------------------------------- PUBLIC METHODS -------------------------------

   #ifdef _DEBUG
   void LanguageView::AssertValid() const
   {
	   CFormView::AssertValid();
   }

   
   void LanguageView::Dump(CDumpContext& dc) const
   {
	   CFormView::Dump(dc);
   }
   #endif //_DEBUG

   void  LanguageView::AdjustLayout()
   {
      // Destroyed/Minimised
	   if (StringEdit.GetSafeHwnd() == nullptr || (AfxGetMainWnd() != nullptr && AfxGetMainWnd()->IsIconic()))
         return;
         
      CRect view;
      GetClientRect(view);

      // Anchor page list on left
      CSize page(view.Width()/3, view.Height());
      PageList.SetWindowPos(nullptr, 0, 0, page.cx, page.cy, SWP_NOZORDER | SWP_NOACTIVATE);

      // Anchor string list on right. Split vertically 66 : 33
      CSize str(2*view.Width()/3, 2*view.Height()/3);
      StringList.SetWindowPos(nullptr, page.cx, 0, str.cx, str.cy, SWP_NOZORDER | SWP_NOACTIVATE);

      // Stretch RichEdit below. Split vertically 33 : 66
      CSize edit(str.cx, view.Height()/3);
      StringEdit.SetWindowPos(nullptr, page.cx, str.cy, edit.cx, edit.cy, SWP_NOZORDER | SWP_NOACTIVATE);
   }

   void LanguageView::DoDataExchange(CDataExchange* pDX)
   {
      CFormView::DoDataExchange(pDX);

      DDX_Control(pDX, IDC_PAGE_LIST, PageList);
      DDX_Control(pDX, IDC_STRING_LIST, StringList);
      DDX_Control(pDX, IDC_STRING_EDIT, StringEdit);
   }

   
   void LanguageView::OnInitialUpdate()
   {
      CFormView::OnInitialUpdate();

      // Setup lists
      PageList.InsertColumn(0, L"ID", LVCFMT_LEFT, 60, 0);
      PageList.InsertColumn(1, L"Title", LVCFMT_LEFT, 100, 1);
      PageList.SetExtendedStyle(LVS_EX_FULLROWSELECT);

      StringList.InsertColumn(0, L"ID", LVCFMT_LEFT, 60, 0);
      StringList.InsertColumn(1, L"Text", LVCFMT_LEFT, 240, 1);
      StringList.SetExtendedStyle(LVS_EX_FULLROWSELECT);

      // Populate pages
   }

   
   void LanguageView::OnSize(UINT nType, int cx, int cy)
   {
      CFormView::OnSize(nType, cx, cy);
      AdjustLayout();
   }

   // ------------------------------ PROTECTED METHODS -----------------------------

   // ------------------------------- PRIVATE METHODS ------------------------------

   
NAMESPACE_END(GUI)



