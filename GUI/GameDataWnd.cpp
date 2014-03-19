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
      //ON_WM_CTLCOLOR()
	   ON_WM_CREATE()
	   ON_WM_SIZE()
	   ON_WM_CONTEXTMENU()
	   ON_WM_PAINT()
	   ON_WM_SETFOCUS()
      ON_WM_SETTINGCHANGE()
      ON_WM_SHOWWINDOW()
      ON_CBN_SELCHANGE(IDC_COMBO, &CGameDataWnd::OnSearchGroupChanged)
      ON_EN_CHANGE(IDC_EDIT, &CGameDataWnd::OnSearchTermChanged)
      ON_NOTIFY(NM_SETFOCUS, IDC_LISTVIEW, &CGameDataWnd::OnSetFocusCtrl)
      ON_NOTIFY(NM_DBLCLK, IDC_LISTVIEW, &CGameDataWnd::OnDoubleClickItem)
   END_MESSAGE_MAP()

   // -------------------------------- CONSTRUCTION --------------------------------

   CGameDataWnd::CGameDataWnd() 
      : fnAppStateChanged(theApp.StateChanged.Register(this, &CGameDataWnd::OnAppStateChanged)),
        fnShowTooltip(Tooltip.RequestData.Register(this, &CGameDataWnd::OnRequestTooltip)),
        fnDocumentSwitched(MainWnd::DocumentSwitched.Register(this, &CGameDataWnd::OnDocumentSwitched))
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
	      throw Win32Exception(HERE, VString(L"Unable to create %s window", title.c_str()));
      SetIcon(theApp.LoadIconW(nIconID, ::GetSystemMetrics(SM_CXSMICON)), FALSE);

      // Dock left/right
      EnableDocking(CBRS_ORIENT_VERT|CBRS_FLOAT_MULTI);
   }
   
   /// <summary>Relay mouse events to tooltip</summary>
   /// <param name="pMsg">MSG.</param>
   /// <returns></returns>
   BOOL CGameDataWnd::PreTranslateMessage(MSG* pMsg)
   {
      // Relay mouse messages to tooltip
      if(pMsg->message >= WM_MOUSEFIRST && pMsg->message <= WM_MOUSELAST)
         Tooltip.RelayEvent(pMsg);

      return __super::PreTranslateMessage(pMsg);
   }

   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Clears all items.</summary>
   void  CGameDataWnd::Clear()
   {
      ListView.RemoveAllGroups();
      ListView.DeleteAllItems();
   }
   
   /// <summary>Enables/disables the list view.</summary>
   /// <param name="enable">new state</param>
   void CGameDataWnd::EnableList(bool enable)
   {
      /*ListView.SetBkColor(GetSysColor(enable ? COLOR_WINDOW : COLOR_BTNFACE));
      ListView.RedrawItems(0, ListView.GetItemCount()-1);
      ListView.UpdateWindow();*/

      // Set state
      ListView.EnableWindow(enable ? TRUE : FALSE);
   }
   
   /// <summary>Gets the index of the item beneath the cursor.</summary>
   /// <returns></returns>
   int  CGameDataWnd::GetHotItemIndex() const
   {
      return ListView.HitTest(CursorPoint(&ListView));
   }

   /// <summary>Gets the search term.</summary>
   /// <returns></returns>
   wstring  CGameDataWnd::GetSearchTerm() const
   {
      CString str;
      Search.GetWindowTextW(str);
      return (LPCWSTR)str;
   }

   /// <summary>Populate when application state changes.</summary>
   /// <param name="s">The s.</param>
   void CGameDataWnd::OnAppStateChanged(AppState s)
   {
      // Set listView state
      EnableList(s == AppState::GameDataPresent);

      // Refresh items
      UpdateContent();
   }

   /// <summary>Called when [context menu].</summary>
   /// <param name="pWnd">The p WND.</param>
   /// <param name="point">The point.</param>
   void CGameDataWnd::OnContextMenu(CWnd* pWnd, CPoint point)
   {
      // Display pane pop-up menu for toolbar
	   if (pWnd->GetSafeHwnd() != ListView.m_hWnd)
	   {
		   CDockablePane::OnContextMenu(pWnd, point);
		   return;
	   }

      // Select item
	   if (point != CPoint(-1, -1))
	   {
         UINT flags = LVHT_ONITEM;
		   // Select item
		   if (auto item = ListView.HitTest(CursorPoint(&ListView), &flags))
            ListView.SetItemState(item, LVIS_SELECTED, LVIS_SELECTED);
	   }

      // Popup
	   ListView.SetFocus();
	   theApp.GetContextMenuManager()->ShowPopupMenu(IDM_GAMEDATA_POPUP, point.x, point.y, this, TRUE);
   }
   
   /// <summary>Abortive attempt to solve listView item background not matching listView background</summary>
   /// <param name="pDC">The dc.</param>
   /// <param name="pWnd">The WND.</param>
   /// <param name="nCtlColor">control type.</param>
   /// <returns></returns>
   /*HBRUSH CGameDataWnd::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
   {
      if (theApp.State == AppState::NoGameData || !ScriptDocument::GetActive())
         return GetSysColorBrush(COLOR_BTNFACE);
   
      return __super::OnCtlColor(pDC, pWnd, nCtlColor);
   }*/

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

         // Set listView background (disabled)
         EnableList(false);
         
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
         
         // Create tooltip
         Tooltip.Create(this, &ListView);
         Tooltip.Activate(TRUE);
         Tooltip.SetTiming(3*1000, 30*1000);
         
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

   /// <summary>Grey the listView for non-script documents</summary>
   void CGameDataWnd::OnDocumentSwitched()
   {
      //EnableList( ScriptDocument::GetActive() != nullptr );
   }
   
   /// <summary>Insert item into current script</summary>
   /// <param name="pNMHDR">The p NMHDR.</param>
   /// <param name="pResult">The p result.</param>
   void CGameDataWnd::OnDoubleClickItem(NMHDR* pNMHDR, LRESULT* pResult)
   {
      auto info = reinterpret_cast<NMITEMACTIVATE*>(pNMHDR);

      // Require script document
      if (auto doc = ScriptDocument::GetActive())
      {
         // Ignore item number provided by system, somtimes invalid.
         auto txt = GetItemText( ListView.GetNextItem(-1, LVNI_SELECTED) ); 

         // Insert into current document
         if (!txt.empty())
            doc->Replace(txt);
      }

      *pResult = 0;
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

   /// <summary>Base implementation cancels tooltip</summary>
   /// <param name="data">data.</param>
   void CGameDataWnd::OnRequestTooltip(CustomTooltip::TooltipData* data)
   {
      data->Cancel();
   }

   /// <summary>set focus to the listView.</summary>
   /// <param name="pOldWnd">The p old WND.</param>
   void CGameDataWnd::OnSetFocus(CWnd* pOldWnd)
   {
	   CDockablePane::OnSetFocus(pOldWnd);

      // Focus listview
	   ListView.SetFocus();
      Tooltip.Reset();
   }
   
   /// <summary>Reset the tooltip when listView becomes focused</summary>
   /// <param name="pNMHDR">The NMHDR.</param>
   /// <param name="pResult">The result.</param>
   void CGameDataWnd::OnSetFocusCtrl(NMHDR* pNMHDR, LRESULT* pResult)
   {
      // Reset tooltip
      Tooltip.Reset();

      *pResult = 0;
   }
   
   /// <summary>Refresh items when group/filter changes.</summary>
   void CGameDataWnd::OnSearchGroupChanged()
   {
      UpdateContent();
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

   /// <summary>Resets tooltip on show</summary>
   /// <param name="bShow">The show.</param>
   /// <param name="nStatus">The status.</param>
   void CGameDataWnd::OnShowWindow(BOOL bShow, UINT nStatus)
   {
      CDockablePane::OnShowWindow(bShow, nStatus);

      // Reset tooltip
      Tooltip.Reset();
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
	   if (!GetSafeHwnd() || theApp.IsMimized())
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
            // Suspend
            ListView.SetRedraw(FALSE);

            // Populate items
            PopulateItems(GetSearchTerm(), Groups.GetCurSel());

            // Redraw
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

