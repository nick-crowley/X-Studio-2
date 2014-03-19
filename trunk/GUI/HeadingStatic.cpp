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
         
      ClientRect rc(this);

      // TODO: Layout code
   }

   /// <summary>Draw text in bold.</summary>
   /// <param name="draw">The draw item structure.</param>
   void HeadingStatic::DrawItem(LPDRAWITEMSTRUCT draw)
   {
      CString    str;
      ClientRect rc(this);
      SharedDC   dc(draw->hDC);

      //Console << HERE << ENDL;

      // Prepare
      GetWindowText(str);
      
      // Draw text in bold
      auto font = dc.SelectObject(&GetGlobalData()->fontBold);
      dc.DrawText(str, rc, DT_LEFT|DT_SINGLELINE|DT_VCENTER);
      dc.SelectObject(font);
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


