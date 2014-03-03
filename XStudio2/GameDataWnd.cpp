#include "stdafx.h"
#include "GameDataWnd.h"
#include "MainWnd.h"
#include "Helpers.h"
#include <strsafe.h>

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)

   const UINT IDC_LISTVIEW = 10,
              IDC_EDIT  = 20,
              IDC_COMBO = 30;

   // --------------------------------- APP WIZARD ---------------------------------
  
   BEGIN_MESSAGE_MAP(CGameDataWnd, CDockablePane)
	   ON_WM_CREATE()
	   ON_WM_SIZE()
	   ON_WM_CONTEXTMENU()
	   ON_WM_PAINT()
	   ON_WM_SETFOCUS()
      ON_EN_CHANGE(IDC_EDIT, &CGameDataWnd::OnSearchTermChanged)
   END_MESSAGE_MAP()

   // -------------------------------- CONSTRUCTION --------------------------------

   CGameDataWnd::CGameDataWnd() : fnAppStateChanged(theApp.StateChanged.Register(this, &CGameDataWnd::OnAppStateChanged))
   {
   }


   CGameDataWnd::~CGameDataWnd()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   /// <summary>Creates the window.</summary>
   /// <param name="parent">parent.</param>
   /// <param name="title">title.</param>
   /// <param name="nID">child id.</param>
   /// <param name="nIconID">Icon id.</param>
   /// <exception cref="Logic::Win32Exception">Unable to create window</exception>
   void CGameDataWnd::Create(CWnd* parent, wstring title, UINT nID, UINT nIconID)
   {
      DWORD style = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_RIGHT | CBRS_FLOAT_MULTI;

      // Create window
      if (!__super::Create(title.c_str(), parent, MainWnd::DefaultSize, TRUE, nID, style))
	      throw Win32Exception(HERE, GuiString(L"Unable to create %s window", title.c_str()));
      SetIcon(theApp.LoadIconW(nIconID, ::GetSystemMetrics(SM_CXSMICON)), FALSE);

      // Dock left/right
      EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);
   }

   // ------------------------------ PROTECTED METHODS -----------------------------
   
   void  CGameDataWnd::Clear()
   {
      ListView.RemoveAllGroups();
         ListView.DeleteAllItems();
   }

   void CGameDataWnd::OnAppStateChanged(AppState s)
   {
      UpdateContent();
   }

   void CGameDataWnd::OnContextMenu(CWnd* pWnd, CPoint point)
   {
	   //CTreeCtrl* pWndTree = (CTreeCtrl*) &TreeView;
	   //ASSERT_VALID(pWndTree);

	   //if (pWnd != pWndTree)
	   //{
		  // CDockablePane::OnContextMenu(pWnd, point);
		  // return;
	   //}

	   //if (point != CPoint(-1, -1))
	   //{
		  // // Select clicked item:
		  // CPoint ptTree = point;
		  // pWndTree->ScreenToClient(&ptTree);

		  // UINT flags = 0;
		  // HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
		  // if (hTreeItem != NULL)
		  // {
			 //  pWndTree->SelectItem(hTreeItem);
		  // }
	   //}

	   //pWndTree->SetFocus();
	   //theApp.GetContextMenuManager()->ShowPopupMenu(IDM_PROJECT_POPUP, point.x, point.y, this, TRUE);
   }


   int CGameDataWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
   {
      try
      {
         CRect rectDummy;
	      rectDummy.SetRectEmpty();

         // Base 
	      if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		      throw Win32Exception(HERE, L"Unable to create dockable pane");

	      // ListView
	      if (!ListView.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | LVS_REPORT |  LVS_ALIGNTOP | LVS_NOCOLUMNHEADER, rectDummy, this, IDC_LISTVIEW))
            throw Win32Exception(HERE, L"Unable to create game data window listview");

         // Insert columns
         ListView.InsertColumn(0, L"Item", LVCFMT_LEFT, 200, 0);
         ListView.SetExtendedStyle(LVS_EX_FULLROWSELECT);
         ListView.SetFont(&afxGlobalData.fontRegular);
         ListView.EnableGroupView(TRUE);
         
         // Setup ImageList:
         Images.Create(IDB_GAMEDATA_ICONS, 16, 8, RGB(255,0,255));
	      ListView.SetImageList(&Images, LVSIL_SMALL);

         // create Search edit
	      if (!Search.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_LEFT | ES_AUTOHSCROLL, rectDummy, this, IDC_EDIT))
            throw Win32Exception(HERE, L"Unable to create game data window edit control");
         Search.SetFont(&afxGlobalData.fontRegular);
         Search.SetCueBanner(L"Enter search term...");

         // create Groups ComboBox
	      if (!Groups.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST, rectDummy, this, IDC_COMBO))
            throw Win32Exception(HERE, L"Unable to create game data window combo box");
         Groups.SetFont(&afxGlobalData.fontRegular);

         // Populate groups
         PopulateGroupCombo();
         
	      // Layout controls + Populate
	      AdjustLayout();
         UpdateContent();
	      return 0;
      }
      catch (ExceptionBase& e)
      {
         Console.Log(HERE, e);
         return -1;
      }
   }

   void CGameDataWnd::OnPaint()
   {
	   CPaintDC dc(this); 

	   CRect rc;
	   ListView.GetWindowRect(rc);
	   ScreenToClient(rc);

      // Draw border
	   rc.InflateRect(1, 1);
	   dc.Draw3dRect(rc, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
   }

   void CGameDataWnd::OnSetFocus(CWnd* pOldWnd)
   {
	   CDockablePane::OnSetFocus(pOldWnd);

	   ListView.SetFocus();
   }

   void CGameDataWnd::OnSearchTermChanged()
   {
      UpdateContent();
   }

   void CGameDataWnd::OnSize(UINT nType, int cx, int cy)
   {
	   CDockablePane::OnSize(nType, cx, cy);
	   AdjustLayout();
   }

   // ------------------------------- PRIVATE METHODS ------------------------------
   
   void CGameDataWnd::AdjustLayout()
   {
      // Destroyed/Minimised
	   if (GetSafeHwnd() == NULL || (AfxGetMainWnd() != NULL && AfxGetMainWnd()->IsIconic()))
         return;

	   CRect client, edit, combo;
	   GetClientRect(client);
      Search.GetClientRect(edit);
      Groups.GetClientRect(combo);

      // Anchor Edit to top
      Search.SetWindowPos(NULL, client.left, client.top, client.Width(), combo.Height(), SWP_NOACTIVATE | SWP_NOZORDER);

      // Anchor Group below Edit
      Groups.SetWindowPos(NULL, client.left, client.top+edit.Height(), client.Width(), combo.Height(), SWP_NOACTIVATE | SWP_NOZORDER);

      // Stretch ListView over remaining area
      UINT header = edit.Height()+combo.Height();
      ListView.SetWindowPos(NULL, client.left, client.top+header, client.Width(), client.Height()-header, SWP_NOACTIVATE | SWP_NOZORDER);

      // Stretch ListView column
      ListView.SetColumnWidth(0, client.Width()-GetSystemMetrics(SM_CXVSCROLL));
   }

   void  CGameDataWnd::UpdateContent()
   {
      try
      {
         // Clear prev content
         Clear();

         // Display nothing if no game data
         if (theApp.State == AppState::GameDataPresent)
         {
            // Get search term
            CString searchTerm;
            Search.GetWindowTextW(searchTerm);

            // Populate items
            ListView.SetRedraw(FALSE);
            PopulateItems((const WCHAR*)searchTerm, Groups.GetCurSel());
            ListView.SetRedraw(TRUE);
            ListView.UpdateWindow();
         }
      }
      catch (ExceptionBase& e)
      {
         Console.Log(HERE, e);
      }
   }
   
NAMESPACE_END2(GUI,Windows)
