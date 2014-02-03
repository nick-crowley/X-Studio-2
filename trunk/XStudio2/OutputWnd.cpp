
#include "stdafx.h"

#include "OutputWnd.h"
#include "MainWnd.h"
#include "Helpers.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)

   // --------------------------------- APP WIZARD ---------------------------------
   
   BEGIN_MESSAGE_MAP(COutputList, CListCtrl)
      ON_WM_CREATE()
	   ON_WM_CONTEXTMENU()
	   ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	   ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	   ON_COMMAND(ID_VIEW_OUTPUTWND, OnViewOutput)
	   ON_WM_WINDOWPOSCHANGING()
      ON_WM_SIZE()
   END_MESSAGE_MAP()

   BEGIN_MESSAGE_MAP(COutputWnd, CDockablePane)
	   ON_WM_CREATE()
	   ON_WM_SIZE()
   END_MESSAGE_MAP()

   // -------------------------------- CONSTRUCTION --------------------------------

   COutputList::COutputList()
   {
   }

   COutputList::~COutputList()
   {
   }

   COutputWnd::COutputWnd() : fnGameDataFeedback(MainWnd::GameDataFeedback.Register(this, &COutputWnd::onGameDataFeedback)),
                              fnLoadSaveFeedback(MainWnd::LoadSaveFeedback.Register(this, &COutputWnd::onLoadSaveFeedback))
   {
      
   }

   COutputWnd::~COutputWnd()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   // ------------------------------ PROTECTED METHODS -----------------------------
   
   int COutputList::OnCreate(LPCREATESTRUCT lpCreateStruct)
   {
	   if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		   return -1;

      // Set font
      SetFont(&afxGlobalData.fontRegular);

      // Insert column
      InsertColumn(0, L"Text");
      
      SetExtendedStyle(LVS_EX_FULLROWSELECT);

      // Load view images:
	   /*Images.Create(IDB_OUTPUT_ICONS, 16, 0, RGB(255, 0, 255));
	   m_wndOutputBuild.SetImageList(&Images, LVSIL_NORMAL);*/
	   return 0;
   }

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

   
   void COutputList::OnSize(UINT nType, int cx, int cy)
   {
      CListCtrl::OnSize(nType, cx, cy);

      // Adjust column
      CRect  rc;
      GetClientRect(&rc);
      SetColumnWidth(0, rc.Width());
   }



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
      try
      {
	      if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		      throw Win32Exception(HERE, L"Unable to create output window dockable pane");

	      CRect rectDummy;
	      rectDummy.SetRectEmpty();

	      // Create tabs window:
	      if (!m_wndTabs.Create(CMFCTabCtrl::STYLE_FLAT, rectDummy, this, 1))
            throw Win32Exception(HERE, L"Unable to create output window tab control");
	      
	      // Create output panes:
	      const DWORD dwStyle = LVS_REPORT | LVS_NOCOLUMNHEADER | LVS_SHAREIMAGELISTS | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL;

	      if (!m_wndOutputBuild.CreateEx(LVS_EX_FULLROWSELECT, dwStyle, rectDummy, &m_wndTabs, 2) ||
		      !m_wndOutputDebug.CreateEx(LVS_EX_FULLROWSELECT, dwStyle, rectDummy, &m_wndTabs, 3) ||
		      !m_wndOutputFind.CreateEx(LVS_EX_FULLROWSELECT, dwStyle, rectDummy, &m_wndTabs, 4))
            throw Win32Exception(HERE, L"Unable to create output window listview");
	      
         // Setup ImageList:
         Images.Create(IDB_OUTPUT_ICONS, 16, 6, RGB(255,0,255));
	      m_wndOutputBuild.SetImageList(&Images, LVSIL_SMALL);
         m_wndOutputDebug.SetImageList(&Images, LVSIL_SMALL);
         m_wndOutputFind.SetImageList(&Images, LVSIL_SMALL);

	      // Attach list windows to tab:
	      m_wndTabs.AddTab(&m_wndOutputBuild, GuiString(IDS_BUILD_TAB).c_str(), (UINT)0);
	      m_wndTabs.AddTab(&m_wndOutputDebug, GuiString(IDS_DEBUG_TAB).c_str(), (UINT)1);
	      m_wndTabs.AddTab(&m_wndOutputFind, GuiString(IDS_FIND_TAB).c_str(), (UINT)2);
	      return 0;
      }
      catch (ExceptionBase& e)
      {
         Console.Log(HERE, e);
         return -1;
      }
   }
   
   void COutputWnd::onGameDataFeedback(const WorkerProgress& wp)
   {
      // Create item
      LVItem item(m_wndOutputBuild.GetItemCount(), wp.Text, NULL, LVIF_TEXT | LVIF_IMAGE | LVIF_INDENT);
      item.iImage = (UINT)wp.Type;
      item.iIndent = wp.Indent;

      // Insert/display
      m_wndOutputBuild.InsertItem(&item);
      m_wndOutputBuild.EnsureVisible(m_wndOutputBuild.GetItemCount()-1, FALSE);
   }

   void COutputWnd::onLoadSaveFeedback(const WorkerProgress& wp)
   {
      // Create item
      LVItem item(m_wndOutputDebug.GetItemCount(), wp.Text, NULL, LVIF_TEXT | LVIF_IMAGE | LVIF_INDENT);
      item.iImage = (UINT)wp.Type;
      item.iIndent = wp.Indent;

      // Insert/display
      m_wndOutputDebug.InsertItem(&item);
      m_wndOutputDebug.EnsureVisible(m_wndOutputDebug.GetItemCount()-1, FALSE);
   }

   void COutputWnd::OnSize(UINT nType, int cx, int cy)
   {
	   CDockablePane::OnSize(nType, cx, cy);

	   // Tab control should cover the whole client area:
	   m_wndTabs.SetWindowPos (NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
   }

   void COutputWnd::UpdateFonts()
   {
	   
   }

   // ------------------------------- PRIVATE METHODS ------------------------------
   
   

NAMESPACE_END2(GUI,Windows)


