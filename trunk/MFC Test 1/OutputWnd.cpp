
#include "stdafx.h"

#include "OutputWnd.h"
#include "MainWnd.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/// <summary>User interface</summary>
NAMESPACE_BEGIN(GUI)

   // --------------------------------- APP WIZARD ---------------------------------
   
   BEGIN_MESSAGE_MAP(COutputWnd, CDockablePane)
	   ON_WM_CREATE()
	   ON_WM_SIZE()
   END_MESSAGE_MAP()

   // -------------------------------- CONSTRUCTION --------------------------------

   COutputWnd::COutputWnd() : fnWorkerFeedback(MainWnd::LoadingFeedback.Register(this, &COutputWnd::onWorkerFeedback))
   {
      
   }

   COutputWnd::~COutputWnd()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   // ------------------------------ PROTECTED METHODS -----------------------------
   
   void COutputWnd::AdjustHorzScroll(CListBox& wndListBox)
   {
	   CClientDC dc(this);
	   CFont* pOldFont = dc.SelectObject(&afxGlobalData.fontRegular);

	   int cxExtentMax = 0;

	   for (int i = 0; i < wndListBox.GetCount(); i ++)
	   {
		   CString strItem;
		   wndListBox.GetText(i, strItem);

		   cxExtentMax = max(cxExtentMax, (int)dc.GetTextExtent(strItem).cx);
	   }

	   wndListBox.SetHorizontalExtent(cxExtentMax);
	   dc.SelectObject(pOldFont);
   }

   int COutputWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
   {
	   if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		   return -1;

	   CRect rectDummy;
	   rectDummy.SetRectEmpty();

	   // Create tabs window:
	   if (!m_wndTabs.Create(CMFCTabCtrl::STYLE_FLAT, rectDummy, this, 1))
	   {
		   TRACE0("Failed to create output tab window\n");
		   return -1;      // fail to create
	   }

	   // Create output panes:
	   const DWORD dwStyle = LBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL;

	   if (!m_wndOutputBuild.Create(dwStyle, rectDummy, &m_wndTabs, 2) ||
		   !m_wndOutputDebug.Create(dwStyle, rectDummy, &m_wndTabs, 3) ||
		   !m_wndOutputFind.Create(dwStyle, rectDummy, &m_wndTabs, 4))
	   {
		   TRACE0("Failed to create output windows\n");
		   return -1;      // fail to create
	   }

	   UpdateFonts();

	   // Attach list windows to tab:
	   m_wndTabs.AddTab(&m_wndOutputBuild, GuiString(IDS_BUILD_TAB).c_str(), (UINT)0);
	   m_wndTabs.AddTab(&m_wndOutputDebug, GuiString(IDS_DEBUG_TAB).c_str(), (UINT)1);
	   m_wndTabs.AddTab(&m_wndOutputFind, GuiString(IDS_FIND_TAB).c_str(), (UINT)2);
	   return 0;
   }
   
   void COutputWnd::onWorkerFeedback(WorkerProgress* progress)
   {
      m_wndOutputBuild.AddString(progress->Text.c_str());
   }


   void COutputWnd::OnSize(UINT nType, int cx, int cy)
   {
	   CDockablePane::OnSize(nType, cx, cy);

	   // Tab control should cover the whole client area:
	   m_wndTabs.SetWindowPos (NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
   }

   void COutputWnd::UpdateFonts()
   {
	   m_wndOutputBuild.SetFont(&afxGlobalData.fontRegular);
	   m_wndOutputDebug.SetFont(&afxGlobalData.fontRegular);
	   m_wndOutputFind.SetFont(&afxGlobalData.fontRegular);
   }

   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
   
   
   
   
   
   
   
   // --------------------------------- APP WIZARD ---------------------------------
   
   BEGIN_MESSAGE_MAP(COutputList, CListBox)
	   ON_WM_CONTEXTMENU()
	   ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	   ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	   ON_COMMAND(ID_VIEW_OUTPUTWND, OnViewOutput)
	   ON_WM_WINDOWPOSCHANGING()
   END_MESSAGE_MAP()

   // -------------------------------- CONSTRUCTION --------------------------------

   COutputList::COutputList()
   {
   }

   COutputList::~COutputList()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   // ------------------------------ PROTECTED METHODS -----------------------------

   void COutputList::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
   {
	   CMenu menu;
	   menu.LoadMenu(IDM_OUTPUT_POPUP);

	   CMenu* pSumMenu = menu.GetSubMenu(0);

	   if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	   {
		   CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

		   if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
			   return;

		   ((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
		   UpdateDialogControls(this, FALSE);
	   }

	   SetFocus();
   }

   void COutputList::OnEditCopy()
   {
	   MessageBox(_T("Copy output"));
   }

   void COutputList::OnEditClear()
   {
	   MessageBox(_T("Clear output"));
   }

   void COutputList::OnViewOutput()
   {
	   CDockablePane* pParentBar = DYNAMIC_DOWNCAST(CDockablePane, GetOwner());
	   CMDIFrameWndEx* pMainFrame = DYNAMIC_DOWNCAST(CMDIFrameWndEx, GetTopLevelFrame());

	   if (pMainFrame != NULL && pParentBar != NULL)
	   {
		   pMainFrame->SetFocus();
		   pMainFrame->ShowPane(pParentBar, FALSE, FALSE, FALSE);
		   pMainFrame->RecalcLayout();

	   }
   }

   // ------------------------------- PRIVATE METHODS ------------------------------

   

NAMESPACE_END(GUI)

