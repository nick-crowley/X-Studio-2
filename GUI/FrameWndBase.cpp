#include "stdafx.h"
#include "FrameWndBase.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Views)

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNCREATE(FrameWndBase, CMDIChildWndEx)

   BEGIN_MESSAGE_MAP(FrameWndBase, CMDIChildWndEx)
      ON_WM_DROPFILES()
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   FrameWndBase::FrameWndBase()
   {
   }

   FrameWndBase::~FrameWndBase()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Adjusts the layout.</summary>
   void  FrameWndBase::AdjustLayout()
   {
      // Destroyed/Minimised
	   if (!GetSafeHwnd() || theApp.IsMimized())
         return;
         
      ClientRect rc(this);

      // TODO: Layout code
   }
   
   /// <summary>Open files dragged onto the window from windows explorer.</summary>
   /// <param name="hDropInfo">The drop information.</param>
   void FrameWndBase::OnDropFiles(HDROP hDropInfo)
   {
      theApp.OnDropFiles(hDropInfo);
   }

   /// <summary>Called when resized.</summary>
   /// <param name="nType">Type of the resize.</param>
   /// <param name="cx">The cx.</param>
   /// <param name="cy">The cy.</param>
   void FrameWndBase::OnSize(UINT nType, int cx, int cy)
   {
      __super::OnSize(nType, cx, cy);
      AdjustLayout();
   }
   
   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
NAMESPACE_END2(GUI,Views)

