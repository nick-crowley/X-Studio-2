#include "stdafx.h"
#include "SplashWnd.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNCREATE(SplashWnd, CWnd)

   BEGIN_MESSAGE_MAP(SplashWnd, CWnd)
      ON_WM_SIZE()
      ON_WM_PAINT()
      ON_WM_ERASEBKGND()
      ON_WM_CREATE()
      ON_WM_TIMER()
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   SplashWnd::SplashWnd() 
   {
   }

   SplashWnd::~SplashWnd()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   void  SplashWnd::Create(CWnd* parent, UINT nID)
   {
      CRect rc(0,0,410,380);
      DWORD dwStyle = WS_OVERLAPPED|WS_VISIBLE;

      // Create window
      if (!__super::Create(/*WS_EX_LAYERED,*/ AfxRegisterWndClass(CS_OWNDC), L"Splash", dwStyle, rc, parent, nID))
         throw Win32Exception(HERE, L"Unable to create splash window");

      //SetLayeredWindowAttributes(0, 255, LWA_ALPHA );
      SetWindowPos(nullptr, -1, -1, 410, 380, SWP_NOMOVE|SWP_NOZORDER);
      SetForegroundWindow();
      ShowWindow(SW_SHOW);

      //SetTimer(10, 100, nullptr);
   }

   // ------------------------------ PROTECTED METHODS -----------------------------
   
   void  SplashWnd::AdjustLayout()
   {
      // Destroyed/Minimised
	   if (!GetSafeHwnd() || theApp.IsMimized())
         return;
         
      CRect wnd;
      GetClientRect(wnd);

      // TODO: Layout code
   }
   

   int SplashWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
   {
      if (CWnd::OnCreate(lpCreateStruct) == -1)
         return -1;

      CenterWindow();

      return 0;
   }

   BOOL SplashWnd::OnEraseBkgnd(CDC* pDC)
   {
      /*ClientRect rc(this);
      pDC->FillSolidRect(rc, RGB(0,0,255));*/
      
      // Complete
      return TRUE; 
   }


   void SplashWnd::OnPaint()
   {
      CPaintDC dc(this); 
      ClientRect wnd(this);

      CImage img;
      img.LoadFromResource(AfxGetResourceHandle(), IDB_SPLASH);
      img.SetHasAlphaChannel(TRUE);

      dc.FillSolidRect(wnd, GetSysColor(COLOR_APPWORKSPACE));
      img.AlphaBlend(dc, 0, 0, 0xff, AC_SRC_OVER); 
      //img.BitBlt(dc, CPoint(0,0), SRCCOPY);

      //CDC      memDC;
      //CDrawingManager dm(dc);
      //
      //try
      //{
      //   CWindowDC screen(nullptr);

      //   //dc.FillSolidRect(ClientRect(this), RGB(255,0,255));

      //   // Create memory DC
      //   if (!memDC.CreateCompatibleDC(&screen))
      //      throw Win32Exception(HERE, L"Unable to create memory DC");
      //   auto prev = memDC.SelectObject(Image);

      //   // Request per-pixel alpha blending
      //   BLENDFUNCTION  oBlendData; 
      //   oBlendData.BlendOp             = AC_SRC_OVER;
      //   oBlendData.AlphaFormat         = AC_SRC_ALPHA;
      //   oBlendData.SourceConstantAlpha = 255;

      //   // Draw bitmap
      //   if (!dc.BitBlt(0, 0, 410, 380, &memDC, 0, 0, SRCCOPY))
      //      throw Win32Exception(HERE, L"Unable to draw splash window");

      //   // Failed:
      //   dm.DrawAlpha(&dc, CRect(0, 0, 410, 380), &memDC, CRect(0, 0, 410, 380));

      //   /// [LOGO] Paint alpha-blended logo
      //   CSize wnd(410,380);
      //   CPoint src(0,0);
      //   if (!UpdateLayeredWindow(&dc, &src, &wnd, &memDC, &src, 0, &oBlendData, ULW_ALPHA))
      //   //if (!::UpdateLayeredWindow(m_hWnd, NULL, NULL, &wnd, memDC.m_hDC, &src, 0, &oBlendData, ULW_ALPHA))
      //      throw Win32Exception(HERE, L"Unable to updated layered window");

      //   // Cleanup
      //   memDC.SelectObject(prev);

      //   /*CImage img;
      //   img.LoadFromResource(AfxGetResourceHandle(), IDB_SPLASH);
      //   img.SetHasAlphaChannel(TRUE);
      //   img.AlphaBlend(dc, 0, 0, 0xff, AC_SRC_OVER); */
      //}
      //catch (ExceptionBase& e) {
      //   Console.Log(HERE, e);
      //}
   }


   
   void SplashWnd::OnSize(UINT nType, int cx, int cy)
   {
      __super::OnSize(nType, cx, cy);
      AdjustLayout();
   }
   


   void SplashWnd::OnTimer(UINT_PTR nIDEvent)
   {
   
      //try
      //{
      //   CDC       memDC;
      //   CWindowDC screen(nullptr);

      //   CImage img;
      //   img.LoadFromResource(AfxGetResourceHandle(), IDB_SPLASH);
      //   img.SetHasAlphaChannel(TRUE);
      //   

      //   // Create memory DC
      //   if (!memDC.CreateCompatibleDC(&screen))
      //      throw Win32Exception(HERE, L"Unable to create memory DC");
      //   auto prev = memDC.SelectObject(Image);

      //   // Request per-pixel alpha blending
      //   BLENDFUNCTION  oBlendData; 
      //   oBlendData.BlendOp             = AC_SRC_OVER;
      //   oBlendData.AlphaFormat         = AC_SRC_ALPHA;
      //   oBlendData.SourceConstantAlpha = 255;
      //   oBlendData.BlendFlags          = 0;

      //   /// [LOGO] Paint alpha-blended logo
      //   CSize wnd(410,380);
      //   CPoint src(0,0);

      //   //if (!UpdateLayeredWindow(&dc, &src, &wnd, &memDC, &src, 0, &oBlendData, ULW_ALPHA))
      //   if (!::UpdateLayeredWindow(m_hWnd, NULL, NULL, &wnd, img.GetDC(), &src, 0, &oBlendData, ULW_ALPHA))
      //      throw Win32Exception(HERE, L"Unable to updated layered window");

      //   // Cleanup
      //   memDC.SelectObject(prev);
      //}
      //catch (ExceptionBase& e) {
      //   Console.Log(HERE, e);
      //}

      __super::OnTimer(nIDEvent);
   }

   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
   NAMESPACE_END2(GUI, Windows)

   


   