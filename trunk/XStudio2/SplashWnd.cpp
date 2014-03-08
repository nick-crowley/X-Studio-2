#include "stdafx.h"
#include "SplashWnd.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNCREATE(SplashWnd, CDialog)

   BEGIN_MESSAGE_MAP(SplashWnd, CDialog)
      ON_WM_SIZE()
      ON_WM_PAINT()
      ON_WM_ERASEBKGND()
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   SplashWnd::SplashWnd() : CDialog(IDD)
   {
   }

   SplashWnd::~SplashWnd()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   // ------------------------------ PROTECTED METHODS -----------------------------
   
   void  SplashWnd::AdjustLayout()
   {
      // Destroyed/Minimised
	   if (GetSafeHwnd() == nullptr || (AfxGetMainWnd() != nullptr && AfxGetMainWnd()->IsIconic()))
         return;
         
      CRect wnd;
      GetClientRect(wnd);

      // TODO: Layout code
   }

   BOOL SplashWnd::OnEraseBkgnd(CDC* pDC)
   {
      // TODO: Add your message handler code here and/or call default

      return __super::OnEraseBkgnd(pDC);
   }


   void SplashWnd::OnPaint()
   {
      CPaintDC dc(this); // device context for painting
      // TODO: Add your message handler code here
      // Do not call CDialog::OnPaint() for painting messages
   }


   
   void SplashWnd::OnSize(UINT nType, int cx, int cy)
   {
      __super::OnSize(nType, cx, cy);
      AdjustLayout();
   }
   
   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
   NAMESPACE_END2(GUI, Windows)

   
   