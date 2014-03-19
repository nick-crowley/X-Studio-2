#include "stdafx.h"
#include "TitleStatic.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Controls)

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNCREATE(TitleStatic, CStatic)

   BEGIN_MESSAGE_MAP(TitleStatic, CStatic)
      ON_WM_SIZE()
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   TitleStatic::TitleStatic()
   {
   }

   TitleStatic::~TitleStatic()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Adjusts the layout.</summary>
   void  TitleStatic::AdjustLayout()
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
   void TitleStatic::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
   {
   }

   /// <summary>Called when resized.</summary>
   /// <param name="nType">Type of the resize.</param>
   /// <param name="cx">The cx.</param>
   /// <param name="cy">The cy.</param>
   void TitleStatic::OnSize(UINT nType, int cx, int cy)
   {
      __super::OnSize(nType, cx, cy);
      AdjustLayout();
   }
   
   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
NAMESPACE_END2(GUI,Controls)


