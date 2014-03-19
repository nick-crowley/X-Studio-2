#include "stdafx.h"
#include "DialogBase.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN(GUI)

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNCREATE(DialogBase, CDialogEx)

   BEGIN_MESSAGE_MAP(DialogBase, CDialogEx)
      ON_WM_SIZE()
      ON_WM_ERASEBKGND()
      ON_WM_CTLCOLOR()
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   DialogBase::DialogBase()
   {}

   /// <summary>Initializes a new instance of the <see cref="DialogBase"/> class.</summary>
   /// <param name="dlg">Dialog template.</param>
   /// <param name="parent">parent window.</param>
   /// <param name="bitmap">bitmap resource id.</param>
   /// <param name="alpha">Whether RGBA bitmap.</param>
   DialogBase::DialogBase(UINT dlg, CWnd* parent /*= nullptr*/, UINT bitmap /*= 0*/, bool alpha /*= false*/)
      : CDialogEx(dlg, parent), Image(bitmap, alpha)
   {
   }

   DialogBase::~DialogBase()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------
   
   /// <summary>Shrinks the image static to fit the image.</summary>
   /// <returns></returns>
   BOOL DialogBase::OnInitDialog()
   {
      __super::OnInitDialog();

      // Autosize image (if any)
      if (Image.GetSafeHwnd())
         Image.ShinkToFit();
      return TRUE;
   }


   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Performs data exchange.</summary>
   /// <param name="pDX">The dx.</param>
   void DialogBase::DoDataExchange(CDataExchange* pDX)
   {
	   __super::DoDataExchange(pDX);
      
      // Add OwnerDraw style to Image/Title/Heading(s) if present
      if (GetDlgItem(IDC_IMAGE_STATIC))
         DDX_OwnerDrawStatic(pDX, IDC_IMAGE_STATIC, Image);

      if (GetDlgItem(IDC_TITLE_STATIC))
         DDX_OwnerDrawStatic(pDX, IDC_TITLE_STATIC, Title);

      if (GetDlgItem(IDC_HEADING1_STATIC))
         DDX_OwnerDrawStatic(pDX, IDC_HEADING1_STATIC, Heading1);

      if (GetDlgItem(IDC_HEADING2_STATIC))
         DDX_OwnerDrawStatic(pDX, IDC_HEADING2_STATIC, Heading2);

      if (GetDlgItem(IDC_HEADING3_STATIC))
         DDX_OwnerDrawStatic(pDX, IDC_HEADING3_STATIC, Heading3);
   }

   /// <summary>Adjusts the layout.</summary>
   void  DialogBase::AdjustLayout()
   {
      // Destroyed/Minimised
	   if (!GetSafeHwnd() || theApp.IsMimized())
         return;
         
      ClientRect rc(this);

      // TODO: Layout code
   }
   
   
   /// <summary>Set the correct background colour for child statics.</summary>
   /// <param name="pDC">The dc.</param>
   /// <param name="pWnd">The WND.</param>
   /// <param name="nCtlColor">control type.</param>
   /// <returns></returns>
   HBRUSH DialogBase::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
   {
      // Customize appearance of Dialog/Statics
      switch (nCtlColor)
      {
      case CTLCOLOR_DLG:
      case CTLCOLOR_STATIC:
         pDC->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
         pDC->SetBkColor(GetSysColor(COLOR_WINDOW));
         return GetSysColorBrush(COLOR_WINDOW);
      }

      // Do default
      return __super::OnCtlColor(pDC, pWnd, nCtlColor);
   }

   /// <summary>Draws white dialog background</summary>
   /// <param name="pDC">The dc.</param>
   /// <returns></returns>
   BOOL DialogBase::OnEraseBkgnd(CDC* pDC)
   {
      // Requires OK button
      if (!GetDlgItem(IDOK))
         return __super::OnEraseBkgnd(pDC);

      // Prepare
      ClientRect wnd(this);
      CtrlRect   ok(this, GetDlgItem(IDOK));

      // Calculate gutter area that vcentres the OK button
      CRect gutter(wnd);
      gutter.top = ok.top - (wnd.bottom - ok.bottom);

      // Background: White
      pDC->FillSolidRect(wnd, GetSysColor(COLOR_WINDOW));

      // Gutter: grey + etched border 
      pDC->DrawEdge(gutter, EDGE_ETCHED, BF_ADJUST | BF_TOP);
      pDC->FillSolidRect(gutter, GetSysColor(COLOR_BTNFACE));

      return TRUE;
   }

   /// <summary>Called when resized.</summary>
   /// <param name="nType">Type of the resize.</param>
   /// <param name="cx">The cx.</param>
   /// <param name="cy">The cy.</param>
   void DialogBase::OnSize(UINT nType, int cx, int cy)
   {
      __super::OnSize(nType, cx, cy);
      AdjustLayout();
   }
   
   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
NAMESPACE_END(GUI)



