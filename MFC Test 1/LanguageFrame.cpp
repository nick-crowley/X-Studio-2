// LanguageEditView.cpp : implementation file
//

#include "stdafx.h"
#include "LanguageFrame.h"
#include "LanguageEditView.h"
#include "LanguagePageView.h"
#include "LanguageStringView.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Views)

   // --------------------------------- APP WIZARD ---------------------------------
  
   // -------------------------------- CONSTRUCTION --------------------------------

   LanguageFrame::LanguageFrame()
   {
   }

   LanguageFrame::~LanguageFrame()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   IMPLEMENT_DYNCREATE(LanguageFrame, CMDIChildWndEx)

   BEGIN_MESSAGE_MAP(LanguageFrame, CMDIChildWndEx)
   END_MESSAGE_MAP()

   // ------------------------------- PUBLIC METHODS -------------------------------

   #ifdef _DEBUG
   void LanguageFrame::AssertValid() const
   {
	   CMDIChildWndEx::AssertValid();
   }

   
   void LanguageFrame::Dump(CDumpContext& dc) const
   {
	   CMDIChildWndEx::Dump(dc);
   }
   #endif //_DEBUG


   // ------------------------------ PROTECTED METHODS -----------------------------
   

   BOOL LanguageFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
   {
      try
      {
         if (!Splitter.CreateStatic(this, 1, 2, WS_CHILD | WS_VISIBLE, AFX_IDW_PANE_FIRST))
            throw Win32Exception(HERE, L"Unable to create languageView splitter");

         if (!Splitter.CreateView(0, 0, RUNTIME_CLASS(LanguagePageView), CSize(0,0), pContext))
            throw Win32Exception(HERE, L"Unable to create languageView page pane");

         if (!Splitter.CreateView(0, 1, RUNTIME_CLASS(LanguageStringView), CSize(0,0), pContext))
            throw Win32Exception(HERE, L"Unable to create languageView string pane");

         //return CMDIChildWndEx::OnCreateClient(lpcs, pContext);
         return TRUE;
      }
      catch (ExceptionBase& e)
      {
         Console << e;
         return FALSE;
      }
   }

   // ------------------------------- PRIVATE METHODS ------------------------------

   
NAMESPACE_END2(GUI,Views)



