// DiffFrame.cpp : implementation file
//

#include "stdafx.h"
#include "DiffFrame.h"
#include "DiffView.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Views)

   // --------------------------------- APP WIZARD ---------------------------------
  
   // -------------------------------- CONSTRUCTION --------------------------------

   DiffFrame::DiffFrame()
   {
   }

   DiffFrame::~DiffFrame()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   IMPLEMENT_DYNCREATE(DiffFrame, FrameWndBase)

   BEGIN_MESSAGE_MAP(DiffFrame, FrameWndBase)
   END_MESSAGE_MAP()

   BEGIN_MESSAGE_MAP(DiffSplitter, DualColumnSplitter)
   END_MESSAGE_MAP()

   // ------------------------------- PUBLIC METHODS -------------------------------

   /// <summary>Saves column widths on change</summary>
   void  DiffSplitter::RecalcLayout() 
   {
      __super::RecalcLayout();

      // Store column widths
      int width, min;
      GetColumnInfo(0, width, min);
      theApp.WriteSectionInt(L"DiffFrame", L"LeftWidth", width);
      GetColumnInfo(1, width, min);
      theApp.WriteSectionInt(L"DiffFrame", L"RightWidth", width);
   }

   // ------------------------------ PROTECTED METHODS -----------------------------
   

   BOOL DiffFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
   {
      try
      {
         int leftWidth = theApp.GetSectionInt(L"DiffFrame", L"LeftWidth", 400),
             rightWidth = theApp.GetSectionInt(L"DiffFrame", L"RightWidth", 400);

         ClientRect wnd(this);

         // Split into two columns:  x|y
         if (!Splitter.Create(this, AFX_IDW_PANE_FIRST))
            throw Win32Exception(HERE, L"Unable to create main splitter");

         // Define minimum sizes
         Splitter.SetColumnInfo(0, leftWidth, 80);
         Splitter.SetColumnInfo(1, rightWidth, 80);
         
         // LHS
         if (!Splitter.CreateView(0, 0, RUNTIME_CLASS(DiffView), CSize(leftWidth, wnd.Height()), pContext))
            throw Win32Exception(HERE, L"Unable to create left pane");

         // RHS 
         if (!Splitter.CreateView(0, 1, RUNTIME_CLASS(DiffView), CSize(rightWidth, wnd.Height()), pContext))
            throw Win32Exception(HERE, L"Unable to create right pane");

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


