// LanguageView.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "LanguageView.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN(GUI)

   // LanguageView

   IMPLEMENT_DYNCREATE(LanguageView, CFormView)

   LanguageView::LanguageView()
	   : CFormView(LanguageView::IDD)
   {

   }

   LanguageView::~LanguageView()
   {
   }

   void LanguageView::DoDataExchange(CDataExchange* pDX)
   {
	   CFormView::DoDataExchange(pDX);
   }

   BEGIN_MESSAGE_MAP(LanguageView, CFormView)
   END_MESSAGE_MAP()


   // LanguageView diagnostics

   #ifdef _DEBUG
   void LanguageView::AssertValid() const
   {
	   CFormView::AssertValid();
   }

   #ifndef _WIN32_WCE
   void LanguageView::Dump(CDumpContext& dc) const
   {
	   CFormView::Dump(dc);
   }
   #endif
   #endif //_DEBUG


   // LanguageView message handlers
   
NAMESPACE_END(GUI)

