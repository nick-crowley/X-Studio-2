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

   IMPLEMENT_DYNCREATE(LanguageFrame, FrameWndBase)

   BEGIN_MESSAGE_MAP(LanguageFrame, FrameWndBase)
   END_MESSAGE_MAP()

   BEGIN_MESSAGE_MAP(DualRowSplitter, CSplitterWnd)
   END_MESSAGE_MAP()

   BEGIN_MESSAGE_MAP(DualColumnSplitter, CSplitterWnd)
   END_MESSAGE_MAP()

   // ------------------------------- PUBLIC METHODS -------------------------------

   /// <summary>Creates splitter.</summary>
   /// <param name="pParentWnd">The parent WND.</param>
   /// <param name="nID">The identifier.</param>
   /// <returns></returns>
   BOOL  DualColumnSplitter::Create(CWnd* pParentWnd, UINT nID /*= AFX_IDW_PANE_FIRST*/)
   {
      return __super::CreateStatic(pParentWnd, 1, 2, WS_CHILD | WS_VISIBLE, nID);
   }

   /// <summary>Saves column widths on change</summary>
   void  DualColumnSplitter::RecalcLayout() 
   {
      CSplitterWnd::RecalcLayout();

      // Store column widths
      int width, min;
      GetColumnInfo(0, width, min);
      theApp.WriteSectionInt(L"LanguageFrame", L"PageWidth", width);
      GetColumnInfo(1, width, min);
      theApp.WriteSectionInt(L"LanguageFrame", L"StringWidth", width);
   }

   /// <summary>Creates splitter.</summary>
   /// <param name="pParentWnd">The parent WND.</param>
   /// <param name="nID">The identifier.</param>
   /// <returns></returns>
   BOOL  DualRowSplitter::Create(CWnd* pParentWnd, UINT nID /*= AFX_IDW_PANE_FIRST*/)
   {
      return __super::CreateStatic(pParentWnd, 2, 1, WS_CHILD | WS_VISIBLE, nID);
   }

   /// <summary>Saves row heights on change</summary>
   void  DualRowSplitter::RecalcLayout() 
   {
      CSplitterWnd::RecalcLayout();

      // Store row heights
      int height, min;
      GetRowInfo(0, height, min);
      theApp.WriteSectionInt(L"LanguageFrame", L"TopHeight", height);
      GetRowInfo(1, height, min);
      theApp.WriteSectionInt(L"LanguageFrame", L"BottomHeight", height);
   }

   // ------------------------------ PROTECTED METHODS -----------------------------
   

   BOOL LanguageFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
   {
      try
      {
         int pageWidth = theApp.GetSectionInt(L"LanguageFrame", L"PageWidth", 200),
             strWidth = theApp.GetSectionInt(L"LanguageFrame", L"StringWidth", 600),
             topHeight = theApp.GetSectionInt(L"LanguageFrame", L"TopHeight", 400),
             btmHeight = theApp.GetSectionInt(L"LanguageFrame", L"BottomHeight", 200);

         ClientRect wnd(this);

         // Split into two rows:  x/y
         if (!RowSplitter.Create(this, AFX_IDW_PANE_FIRST))
            throw Win32Exception(HERE, L"Unable to create main splitter");

         // Define minimum sizes
         RowSplitter.SetRowInfo(0, topHeight, 80);
         RowSplitter.SetRowInfo(1, btmHeight, 80);
         
         // Split top row into two columns:  a | b
         if (!ColumnSplitter.Create(&RowSplitter, RowSplitter.IdFromRowCol(0,0)))
            throw Win32Exception(HERE, L"Unable to create child splitter");

         // Define minimum sizes
         ColumnSplitter.SetColumnInfo(0, pageWidth, 80);
         ColumnSplitter.SetColumnInfo(1, strWidth, 80);

         // Page: LHS
         if (!ColumnSplitter.CreateView(0, 0, RUNTIME_CLASS(LanguagePageView), CSize(pageWidth, topHeight), pContext))
            throw Win32Exception(HERE, L"Unable to create page pane");

         // String: RHS 
         if (!ColumnSplitter.CreateView(0, 1, RUNTIME_CLASS(LanguageStringView), CSize(strWidth, topHeight), pContext))
            throw Win32Exception(HERE, L"Unable to create string pane");

         // EditPane: bottom
         if (!RowSplitter.CreateView(1, 0, RUNTIME_CLASS(LanguageEditView), CSize(wnd.Width(), btmHeight), pContext))
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


