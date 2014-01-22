// LanguageView.cpp : implementation file
//

#include "stdafx.h"
#include "LanguageView.h"

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

   void LanguageView::DoDataExchange(CDataExchange* pDX)
   {
	   CFormView::DoDataExchange(pDX);

   }
   // ------------------------------ PROTECTED METHODS -----------------------------

   // ------------------------------- PRIVATE METHODS ------------------------------

   
NAMESPACE_END(GUI)

