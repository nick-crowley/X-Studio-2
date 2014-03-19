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
      /*LOGFONT lf;
      GetGlobalData()->fontRegular.GetLogFont(&lf);*/
      
      // Create 14pt version of title font
      TitleFont.CreatePointFont(10*TEXT_SIZE, GetGlobalData()->bIsWindows7 ? L"Segoe UI Light" : L"Tahoma");
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
         
      ClientRect rc(this);

      // TODO: Layout code
   }
   
   /// <summary>Draw title in large blue text.</summary>
   /// <param name="lpDrawItemStruct">The draw item structure.</param>
   void TitleStatic::DrawItem(LPDRAWITEMSTRUCT draw)
   {
      CString    str;
      ClientRect rc(this);
      SharedDC   dc(draw->hDC);

      //Console << HERE << ENDL;

      // Prepare
      GetWindowText(str);

      // Draw large text in blue
      auto font = dc.SelectObject(&TitleFont);
      auto col = dc.SetTextColor(TEXT_COLOUR);
      dc.DrawText(str, rc, DT_CENTER|DT_SINGLELINE|DT_TOP);

      // Cleanup
      dc.SetTextColor(col);
      dc.SelectObject(font);
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


