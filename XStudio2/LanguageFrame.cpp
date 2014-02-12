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

   BEGIN_MESSAGE_MAP(LanguagePageSplitter, CSplitterWnd)
   END_MESSAGE_MAP()

   BEGIN_MESSAGE_MAP(LanguageStringSplitter, CSplitterWnd)
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
         int pageWidth = theApp.GetSectionInt(L"LanguageFrame", L"PageWidth", 150),
             strWidth = theApp.GetSectionInt(L"LanguageFrame", L"StringWidth", 500),
             strHeight = theApp.GetSectionInt(L"LanguageFrame", L"StringHeight", 400),
             editHeight = theApp.GetSectionInt(L"LanguageFrame", L"EditHeight", 100);

         //theApp.WriteSectionInt(L"LanguageView", L"Width", 100);
         // Split horizontally:  x | y
         if (!Splitter.CreateStatic(this, 1, 2, WS_CHILD | WS_VISIBLE, AFX_IDW_PANE_FIRST))
            throw Win32Exception(HERE, L"Unable to create main splitter");

         // Page: LHS
         if (!Splitter.CreateView(0, 0, RUNTIME_CLASS(LanguagePageView), CSize(pageWidth, 0), pContext))
            throw Win32Exception(HERE, L"Unable to create page pane");

         // Split rhs vertically:  a/b
         if (!SubSplitter.CreateStatic(&Splitter, 2, 1, WS_CHILD | WS_VISIBLE, Splitter.IdFromRowCol(0,1)))
            throw Win32Exception(HERE, L"Unable to create child splitter");

         // StringPane: RHS top
         if (!SubSplitter.CreateView(0, 0, RUNTIME_CLASS(LanguageStringView), CSize(strWidth, strHeight), pContext))
            throw Win32Exception(HERE, L"Unable to create string pane");

         // EditPane: RHS bottom
         if (!SubSplitter.CreateView(1, 0, RUNTIME_CLASS(LanguageEditView), CSize(strWidth, editHeight), pContext))
            throw Win32Exception(HERE, L"Unable to create edit pane");

         // Don't call base
         return TRUE;
      }
      catch (ExceptionBase& e)
      {
         Console.Log(HERE, e);
         return FALSE;
      }
   }

   // ------------------------------- PRIVATE METHODS ------------------------------

   
NAMESPACE_END2(GUI,Views)


