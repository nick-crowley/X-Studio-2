#include "stdafx.h"
#include "BitmapStatic.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Controls)

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNCREATE(BitmapStatic, CStatic)

   BEGIN_MESSAGE_MAP(BitmapStatic, CStatic)
      ON_WM_SIZE()
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   BitmapStatic::BitmapStatic() : BitmapID(0)
   {}

   BitmapStatic::BitmapStatic(UINT id, bool alpha /*= false*/) : BitmapID(id), Alpha(alpha)
   {
      // Load RGB[A] bitmap
      Image.LoadFromResource(AfxGetResourceHandle(), BitmapID);
      Image.SetHasAlphaChannel(Alpha ? TRUE : FALSE);
   }

   BitmapStatic::~BitmapStatic()
   {}

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   /// <summary>Resize the static to fit the image.</summary>
   void BitmapStatic::ShinkToFit()
   {
      SetWindowPos(nullptr, -1, -1, Image.GetWidth(), Image.GetHeight(), SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOMOVE);
   }

   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Adjusts the layout.</summary>
   void  BitmapStatic::AdjustLayout()
   {
      // Destroyed/Minimised
	   if (!GetSafeHwnd() || theApp.IsMimized())
         return;
         
      ClientRect rc(this);

      // TODO: Layout code
   }
   
   /// <summary>Owner draw.</summary>
   /// <param name="lpDrawItemStruct">The draw item structure.</param>
   void BitmapStatic::DrawItem(LPDRAWITEMSTRUCT draw)
   {
      ClientRect rc(this);
      
      //dc.FillSolidRect(wnd, GetSysColor(COLOR_APPWORKSPACE));

      // Draw 
      if (Alpha)
         Image.AlphaBlend(SharedDC(draw->hDC), 0, 0, 0xff, AC_SRC_OVER); 
      else
         Image.Draw(draw->hDC, rc);
   }

   /// <summary>Called when resized.</summary>
   /// <param name="nType">Type of the resize.</param>
   /// <param name="cx">The cx.</param>
   /// <param name="cy">The cy.</param>
   void BitmapStatic::OnSize(UINT nType, int cx, int cy)
   {
      __super::OnSize(nType, cx, cy);
      AdjustLayout();
   }
   
   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
NAMESPACE_END2(GUI,Controls)


