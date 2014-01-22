#include "stdafx.h"
#include "GameDataWnd.h"
#include <strsafe.h>
#include "ListView.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN(GUI)

   // --------------------------------- APP WIZARD ---------------------------------
  
   BEGIN_MESSAGE_MAP(CGameDataWnd, CDockablePane)
	   ON_WM_CREATE()
	   ON_WM_SIZE()
	   ON_WM_CONTEXTMENU()
	   ON_WM_PAINT()
	   ON_WM_SETFOCUS()
      ON_NOTIFY(LVN_GETDISPINFO,1,&CGameDataWnd::onRequestItem)
   END_MESSAGE_MAP()

   // -------------------------------- CONSTRUCTION --------------------------------

   CGameDataWnd::CGameDataWnd() : fnAppStateChanged(theApp.StateChanged.Register(this, &CGameDataWnd::onAppStateChanged))
   {
   }


   CGameDataWnd::~CGameDataWnd()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   // ------------------------------ PROTECTED METHODS -----------------------------

   void CGameDataWnd::onAppStateChanged(AppState s)
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
	      if (!ListView.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | LVS_REPORT |  LVS_ALIGNTOP |/*LVS_OWNERDATA |*/ LVS_NOCOLUMNHEADER, rectDummy, this, 1))
            throw Win32Exception(HERE, L"Unable to create game data window listview");

         // Insert columns
         ListView.InsertColumn(0, L"Item", LVCFMT_LEFT, 200, 0);
         ListView.InsertColumn(1, L"Group", LVCFMT_LEFT, 100, 1);
         ListView.SetExtendedStyle(LVS_EX_FULLROWSELECT);
         ListView.SetFont(&afxGlobalData.fontRegular);
         ListView.EnableGroupView(TRUE);
         
         // ListView ImageList:
	      /*Images.Create(IDB_FILE_VIEW, 16, 0, RGB(255, 0, 255));
	      ListView.SetImageList(&Images, LVSIL_NORMAL);*/

         // Edit
	      if (!Search.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_LEFT | ES_AUTOHSCROLL, rectDummy, this, 2))
            throw Win32Exception(HERE, L"Unable to create game data window edit control");
         Search.SetFont(&afxGlobalData.fontRegular);

         // Combobox
	      if (!Groups.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST, rectDummy, this, 3))
            throw Win32Exception(HERE, L"Unable to create game data window combo box");
         Groups.SetFont(&afxGlobalData.fontRegular);

         // Populate groups
         for (UINT g = (UINT)CommandGroup::ARRAY; g < (UINT)CommandGroup::HIDDEN; ++g)
            Groups.AddString(GuiString(IDS_FIRST_COMMAND_GROUP + g).c_str());

	      // Layout controls + Populate
	      AdjustLayout();
         UpdateContent();
	      return 0;
      }
      catch (ExceptionBase& e)
      {
         Console << e << ENDL;
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

   void CGameDataWnd::OnSize(UINT nType, int cx, int cy)
   {
	   CDockablePane::OnSize(nType, cx, cy);
	   AdjustLayout();
   }


   void CGameDataWnd::onRequestItem(NMHDR* pNMHDR, LRESULT* pResult)
   {
      LVITEM& item = reinterpret_cast<NMLVDISPINFO*>(pNMHDR)->item;
      auto syntax = Content[item.iItem];
      
      // Text
      if (item.mask & LVIF_TEXT)
      {
         if (item.iSubItem == 0)
            StringCchCopy(item.pszText, item.cchTextMax, syntax->Text.c_str());
      }

      *pResult = TRUE;
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
      ListView.SetColumnWidth(0, client.Width());
   }

   void  CGameDataWnd::UpdateContent()
   {
      try
      {
         // Clear prev content
         Content.clear();

         if (theApp.State == AppState::GameDataPresent)
         {
            // TODO: Get search text

            // Lookup matches
            Content = SyntaxLib.Query(L"", GameVersion::TerranConflict);

            // Populate group names
            for (auto pair : SyntaxLib.GetGroups())
            {
               LVGroup grp((UINT)pair.first, pair.second);
               if (ListView.InsertGroup(grp.iGroupId, (LVGROUP*)&grp) != grp.iGroupId)
                  throw Win32Exception(HERE, GuiString(L"Unable to insert command group ") + pair.second);
            }

            // Display results
            ListView.SetItemCount(Content.size());
            for (UINT i = 0; i < Content.size(); ++i)
            {
               LVITEM item;
               item.mask = LVIF_TEXT | LVIF_GROUPID;
               item.iGroupId = (UINT)Content[i]->Group;
               item.pszText = (WCHAR*)Content[i]->Text.c_str();
               item.iItem = i;
            
               //ListView.InsertItem(i, Content[i]->Text.c_str(), 0);
               if (ListView.InsertItem(&item) == -1)
                  throw Win32Exception(HERE, GuiString(L"Unable to insert command '%s' (item %d, group %d)", item.pszText, i, item.iGroupId));
            }
         }
         else
            ListView.SetItemCount(0);
      }
      catch (ExceptionBase& e)
      {
         Console << e << ENDL;
      }
   }
   
NAMESPACE_END(GUI)
