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
      ON_WM_SETTINGCHANGE()
      ON_EN_CHANGE(IDC_EDIT, &CGameDataWnd::OnSearchTermChanged)
      ON_NOTIFY(NM_DBLCLK, IDC_LISTVIEW, &CGameDataWnd::OnDoubleClickItem)
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
   
   /// <summary>Clears all items.</summary>
   void  CGameDataWnd::Clear()
   {
      ListView.RemoveAllGroups();
      ListView.DeleteAllItems();
   }

   /// <summary>Populate when application state changes.</summary>
   /// <param name="s">The s.</param>
   void CGameDataWnd::OnAppStateChanged(AppState s)
   {
      UpdateContent();
   }

   /// <summary>Called when [context menu].</summary>
   /// <param name="pWnd">The p WND.</param>
   /// <param name="point">The point.</param>
   void CGameDataWnd::OnContextMenu(CWnd* pWnd, CPoint point)
   {
   }

   
   /// <summary>Insert item into current script</summary>
   /// <param name="pNMHDR">The p NMHDR.</param>
   /// <param name="pResult">The p result.</param>
   void CGameDataWnd::OnDoubleClickItem(NMHDR* pNMHDR, LRESULT* pResult)
   {
      auto info = reinterpret_cast<NMITEMACTIVATE*>(pNMHDR);

      // Require script document
      if (ScriptDocument::GetActive())
      {
         // Ignore item number provided by system, somtimes invalid.
         auto txt = GetItemText( ListView.HitTest(CursorPoint(this)) );

         // Insert into current document
         if (!txt.empty())
            ScriptDocument::GetActive()->Replace(txt);
      }

      *pResult = 0;
   }
   

   /// <summary>Create child controls.</summary>
   /// <param name="lpCreateStruct">The lp create structure.</param>
   /// <returns></returns>
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
         ListView.SetFont(&theApp.ToolWindowFont);
         ListView.EnableGroupView(TRUE);
         
         // Setup ImageList:
         Images.Create(IDB_GAMEDATA_ICONS, 16, 8, RGB(255,0,255));
	      ListView.SetImageList(&Images, LVSIL_SMALL);

         // create Search edit
	      if (!Search.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_LEFT | ES_AUTOHSCROLL, rectDummy, this, IDC_EDIT))
            throw Win32Exception(HERE, L"Unable to create game data window edit control");
         Search.SetFont(&theApp.ToolWindowFont);
         Search.SetCueBanner(L"Enter search term...");

         // create Groups ComboBox
	      if (!Groups.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST, rectDummy, this, IDC_COMBO))
            throw Win32Exception(HERE, L"Unable to create game data window combo box");
         Groups.SetFont(&theApp.ToolWindowFont);

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

   /// <summary>(App-Wizard Generated) Draw border.</summary>
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

   /// <summary>set focus to the listView.</summary>
   /// <param name="pOldWnd">The p old WND.</param>
   void CGameDataWnd::OnSetFocus(CWnd* pOldWnd)
   {
	   CDockablePane::OnSetFocus(pOldWnd);

	   ListView.SetFocus();
   }

   /// <summary>Refresh items when search text changes.</summary>
   void CGameDataWnd::OnSearchTermChanged()
   {
      UpdateContent();
   }
   

   /// <summary>Change the window fonts when settings change.</summary>
   /// <param name="uFlags">The flags.</param>
   /// <param name="lpszSection">The section.</param>
   void CGameDataWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
   {
      CDockablePane::OnSettingChange(uFlags, lpszSection);

      // Update window fonts
      Groups.SetFont(&theApp.ToolWindowFont);
      Search.SetFont(&theApp.ToolWindowFont);
      ListView.SetFont(&theApp.ToolWindowFont);

      // Adjust layout
      AdjustLayout();
   }

   /// <summary>Adjusts layout on resize</summary>
   /// <param name="nType">Type of the n.</param>
   /// <param name="cx">The width.</param>
   /// <param name="cy">The height.</param>
   void CGameDataWnd::OnSize(UINT nType, int cx, int cy)
   {
	   CDockablePane::OnSize(nType, cx, cy);
	   AdjustLayout();
   }

   // ------------------------------- PRIVATE METHODS ------------------------------
   
   /// <summary>Adjusts the layout.</summary>
   void CGameDataWnd::AdjustLayout()
   {
      // Destroyed/Minimised
	   if (GetSafeHwnd() == NULL || (AfxGetMainWnd() != NULL && AfxGetMainWnd()->IsIconic()))
         return;

      ClientRect client(this);
      CtrlRect   combo(this, &Groups);

      // Anchor Edit to top
      Search.SetWindowPos(NULL, client.left, client.top, client.Width(), combo.Height(), SWP_NOACTIVATE | SWP_NOZORDER);

      // Anchor Group below Edit
      Groups.SetWindowPos(NULL, client.left, client.top+combo.Height(), client.Width(), combo.Height(), SWP_NOACTIVATE | SWP_NOZORDER);

      // Stretch ListView over remaining area
      UINT header = combo.Height()+combo.Height();
      ListView.SetWindowPos(NULL, client.left, client.top+header, client.Width(), client.Height()-header, SWP_NOACTIVATE | SWP_NOZORDER);

      // Stretch ListView column
      ListView.SetColumnWidth(0, client.Width()-GetSystemMetrics(SM_CXVSCROLL));
   }

   /// <summary>Updates the content.</summary>
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
