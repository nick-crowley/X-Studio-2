#include "stdafx.h"
#include "HeadingStatic.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Controls)

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNCREATE(HeadingStatic, CStatic)

   BEGIN_MESSAGE_MAP(HeadingStatic, CStatic)
      /*ON_WM_DRAWITEM_REFLECT()
      ON_WM_ERASEBKGND()*/
      ON_WM_SIZE()
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   HeadingStatic::HeadingStatic()
   {
   }

   HeadingStatic::~HeadingStatic()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Adjusts the layout.</summary>
   void  HeadingStatic::AdjustLayout()
   {
      // Destroyed/Minimised
	   if (!GetSafeHwnd() || theApp.IsMimized())
         return;
         
      CRect wnd;
      GetClientRect(wnd);

      // TODO: Layout code
   }

   /// <summary>Owner draw.</summary>
   /// <param name="lpDrawItemStruct">The draw item structure.</param>
   void HeadingStatic::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
   {
   }

   
   /// <summary>Called when resized.</summary>
   /// <param name="nType">Type of the resize.</param>
   /// <param name="cx">The cx.</param>
   /// <param name="cy">The cy.</param>
   void HeadingStatic::OnSize(UINT nType, int cx, int cy)
   {
      __super::OnSize(nType, cx, cy);
      AdjustLayout();
   }
   
   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
NAMESPACE_END2(GUI,Controls)


