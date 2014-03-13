#include "stdafx.h"
#include "BackupWnd.h"
#include "MainWnd.h"
#include "ScriptDocument.h"
#include "ProjectDocument.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)

   // --------------------------------- APP WIZARD ---------------------------------

   BEGIN_MESSAGE_MAP(BackupWnd, CDockablePane)
	   ON_WM_CREATE()
	   ON_WM_SIZE()
      ON_WM_PAINT()
      ON_WM_SETFOCUS()
	   ON_WM_SETTINGCHANGE()
      ON_WM_DRAWITEM()
      ON_WM_MEASUREITEM()
      ON_WM_CONTEXTMENU()
      ON_COMMAND(ID_BACKUP_DIFF, &BackupWnd::OnCommandDiff)
      ON_COMMAND(ID_BACKUP_REVERT, &BackupWnd::OnCommandRevert)
      ON_COMMAND(ID_BACKUP_DELETE, &BackupWnd::OnCommandDelete)
      ON_UPDATE_COMMAND_UI(ID_BACKUP_DIFF, &BackupWnd::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_BACKUP_REVERT, &BackupWnd::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_BACKUP_DELETE, &BackupWnd::OnQueryCommand)
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   BackupWnd::BackupWnd() : fnDocumentSwitched(MainWnd::DocumentSwitched.Register(this, &BackupWnd::OnDocumentSwitched))
   {
   }

   BackupWnd::~BackupWnd()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   /// <summary>Creates the window.</summary>
   /// <param name="parent">The parent.</param>
   /// <exception cref="Logic::Win32Exception">Unable to create window</exception>
   void BackupWnd::Create(CWnd* parent)
   {
      DWORD style = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_RIGHT | CBRS_FLOAT_MULTI;

      // Create window
      if (!__super::Create(GuiString(IDR_BACKUP).c_str(), parent, MainWnd::DefaultSize, TRUE, IDR_BACKUP, style))
	      throw Win32Exception(HERE, L"Unable to create backup window");

      SetIcon(theApp.LoadIconW(IDR_BACKUP, ::GetSystemMetrics(SM_CXSMICON)), FALSE);

      // Dock left/right
      EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);
   }

   // ------------------------------ PROTECTED METHODS -----------------------------

   /// <summary>Adjusts the layout.</summary>
   void BackupWnd::AdjustLayout()
   {
      // Destroyed/Minimised
	   if (!GetSafeHwnd() || theApp.IsMimized())
         return;

	   ClientRect wnd(this);

      // Toolbar height
	   int barHeight = ToolBar.CalcFixedLayout(FALSE, TRUE).cy;

      // Snap toolbar to top, stretch list over remainder
	   ToolBar.SetWindowPos(nullptr, wnd.left, wnd.top, wnd.Width(), barHeight, SWP_NOACTIVATE | SWP_NOZORDER);
	   List.SetWindowPos(nullptr, wnd.left+1, wnd.top+barHeight+1, wnd.Width()-2, wnd.Height()-barHeight-2, SWP_NOACTIVATE | SWP_NOZORDER);
   }

   /// <summary>Create child controls</summary>
   /// <param name="lpCreateStruct">create params</param>
   /// <returns></returns>
   int BackupWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
   {
      try
      {
	      if (__super::OnCreate(lpCreateStruct) == -1)
		      throw Win32Exception(HERE, L"Failed to create dockable pane");

	      CRect rectDummy;
	      rectDummy.SetRectEmpty();
         
         // Toolbar
         ToolBar.Create(this, IDR_BACKUP, L"Backup");

         // Create List
         DWORD style = WS_VISIBLE|WS_CHILD|WS_VSCROLL|LBS_DISABLENOSCROLL|LBS_OWNERDRAWVARIABLE|LBS_NOTIFY;
	      if (!List.Create(style, rectDummy, this, IDC_BACKUP_LIST))
	         throw Win32Exception(HERE, L"Failed to create backup list");

         // Setup list

         // ImageList
         Images.Create(IDB_BACKUP_ICONS, 24, 3, RGB(255,0,255));
         
         // Layout
         UpdateFont();
	      AdjustLayout();
	      return 0;
      }
      catch (ExceptionBase& e) {
         Console.Log(HERE, e);
         return -1;
      }
   }
   
   /// <summary>Called when context menu.</summary>
   /// <param name="pWnd">The p WND.</param>
   /// <param name="point">The point in screen co-ordinates.</param>
   void BackupWnd::OnContextMenu(CWnd* pWnd, CPoint point)
   {
      // Ensure List was clicked
      if (pWnd && List.m_hWnd == pWnd->m_hWnd)
      {
         BOOL none = FALSE;
          
         // Select item beneath cursor
         int item = List.ItemFromPoint(CursorPoint(&List), none);
         List.SetCurSel(!none ? item : LB_ERR);

         // Popup
         theApp.GetContextMenuManager()->ShowPopupMenu(IDM_BACKUP_POPUP, point.x, point.y, this, TRUE);
      }
   }
   
   /// <summary>Display diff for selected revision</summary>
   void  BackupWnd::OnCommandDiff()
   {
      auto script = ScriptDocument::GetActive();
      
      // Require document/selection
      if (script && List.GetCurSel() != -1)
         theApp.OpenDiffDocument(*script, Content.Revisions.FindByIndex(List.GetCurSel()).Content);
   }

   /// <summary>Loads backups for the activate document</summary>
   void BackupWnd::OnDocumentSwitched()
   {
      Console << HERE << ENDL;

      // Clear previous
      List.ResetContent();
      //List.SendMessage(LB_SETCOUNT, 0);

      // Require project
      if (auto proj = ProjectDocument::GetActive())
      {
         Content = proj->GetAllBackups(DocumentBase::GetActive());
         Populate();
      }
   }
   

   void BackupWnd::OnDrawItem(int id, LPDRAWITEMSTRUCT draw)
   {
      try
      {
         SharedDC dc(draw->hDC);
         CRect    rc(draw->rcItem);

         // Empty: Draw focus only
         if (draw->itemID == -1)
            dc.DrawFocusRect(&draw->rcItem);
         else
         {
            // Prepare
            auto& item = Content.Revisions.FindByIndex(draw->itemID);
            bool Selected = (draw->itemState & ODS_SELECTED) != 0,   // List.GetCurSel() == draw->itemID,
                 Focused = (draw->itemState & ODS_FOCUS) != 0;   //List.GetCaretIndex() == draw->itemID;

            // Background:
            dc.FillSolidRect(rc, GetSysColor(Selected?COLOR_HIGHLIGHT:COLOR_WINDOW));
            dc.SetTextColor(GetSysColor(Selected?COLOR_HIGHLIGHTTEXT:COLOR_WINDOWTEXT));

            // Separator
            if (!Focused)
               dc.DrawEdge(rc, EDGE_ETCHED, BF_BOTTOM);
            rc.DeflateRect(GetSystemMetrics(SM_CXEDGE), 0);

            // Icon
            Images.Draw(dc, 0, CRect(rc.left, rc.top, rc.left+24, rc.bottom), CSize(24,24), Selected ? ODS_SELECTED : NULL);
            rc.left += 24 + GetSystemMetrics(SM_CXEDGE);

            // Date:
            auto font = dc.SelectObject(&BoldFont);
            dc.DrawText(item.Date.Format(L"%d %b '%y %I:%M%p"), rc, DT_RIGHT|DT_TOP);      // 5 Mar '14 14:20pm
            dc.SelectObject(font);

            // Title:
            rc.top += dc.GetTextExtent(L"ABC").cy;
            dc.DrawText(item.Title.c_str(), rc, DT_LEFT|DT_WORDBREAK);

            // Focus:
            if (Focused)
               dc.DrawFocusRect(&draw->rcItem);
         }
      }
      catch (ExceptionBase& e) {
         Console.Log(HERE, e);
      }

      __super::OnDrawItem(id, draw);
   }


   void BackupWnd::OnMeasureItem(int id, LPMEASUREITEMSTRUCT measure)
   {
      try
      {
         ClientRect rc(this);
         CClientDC  dc(this);

         Console << HERE << " item=" << measure->itemID << ENDL;

         // Lookup item
         auto& item = Content.Revisions.FindByIndex(measure->itemID);

         // Prepare
         auto font = dc.SelectObject(List.GetFont());
         rc.DeflateRect(GetSystemMetrics(SM_CXEDGE), 0);

         // Icon
         rc.left += 24 + GetSystemMetrics(SM_CXEDGE);
      
         // Measure Title/Date:
         int dateHeight = dc.GetTextExtent(L"ABC").cy;
         dc.DrawText(item.Title.c_str(), &rc, DT_LEFT|DT_TOP|DT_CALCRECT|DT_WORDBREAK);

         // Set width/height
         measure->itemHeight = rc.Height() + dateHeight + 3;
         measure->itemWidth = rc.Width();

         // Cleanup
         dc.SelectObject(font);
      }
      catch (ExceptionBase& e) {
         Console.Log(HERE, e);
      }

      __super::OnMeasureItem(id, measure);
   }

   /// <summary>Manually paints border around grid.</summary>
   void BackupWnd::OnPaint()
   {
	   CPaintDC dc(this); // device context for painting
      CtrlRect rc(this, &List);

      // Paint border
	   rc.InflateRect(1, 1);
	   dc.Draw3dRect(rc, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
   }
   
   /// <summary>Perform command</summary>
   /// <param name="nID">ID</param>
   void BackupWnd::OnPerformCommand(UINT nID)
   {
      switch (nID)
      {
      case ID_BACKUP_DELETE:  
         break;

      case ID_BACKUP_DIFF:    
         
         break;

      case ID_BACKUP_REVERT:  
         break;
      }
   }

   
   /// <summary>Query state of context menu command</summary>
   /// <param name="pCmdUI">UI object</param>
   void BackupWnd::OnQueryCommand(CCmdUI *pCmdUI)
   {
      BOOL state = TRUE, 
           check = FALSE;

      //
      switch (pCmdUI->m_nID)
      {
      case ID_BACKUP_DELETE:  
      case ID_BACKUP_DIFF:    
      case ID_BACKUP_REVERT:  
         state = (List.GetCurSel() != LB_ERR ? TRUE : FALSE);
         break;
      }

      // Set state
      pCmdUI->SetCheck(check);
      pCmdUI->Enable(state);
   }
   

   
   /// <summary>Set focus to properties grid</summary>
   /// <param name="pOldWnd">The p old WND.</param>
   void BackupWnd::OnSetFocus(CWnd* pOldWnd)
   {
      __super::OnSetFocus(pOldWnd);
	   List.SetFocus();
   }

   /// <summary>Update font when settings change.</summary>
   /// <param name="uFlags">The flags.</param>
   /// <param name="lpszSection">The section.</param>
   void BackupWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
   {
	   __super::OnSettingChange(uFlags, lpszSection);

      // Update font
	   UpdateFont();
      AdjustLayout();
   }

   /// <summary>Adjusts the layout</summary>
   /// <param name="nType">Type of the n.</param>
   /// <param name="cx">The width.</param>
   /// <param name="cy">The height.</param>
   void BackupWnd::OnSize(UINT nType, int cx, int cy)
   {
	   __super::OnSize(nType, cx, cy);
	   AdjustLayout();
   }
   
   /// <summary>Populates items from current backup file.</summary>
   void BackupWnd::Populate()
   {
      try
      {
         // Clear
         List.ResetContent();
         
         // Fill list with dummy items
         for (auto& rev : Content.Revisions)
            List.InsertString(-1, L"-");

         //List.SendMessage(LB_SETCOUNT, f.Revisions.Count);      // Doesn't work
      }
      catch (ExceptionBase& e) {
         List.ResetContent();
         theApp.ShowError(HERE, e);
      }
   }

   /// <summary>Updates the font.</summary>
   void BackupWnd::UpdateFont()
   {
	   List.SetFont(&theApp.ToolWindowFont);

      // Generate bold font
      LOGFONT lf;
      theApp.ToolWindowFont.GetLogFont(&lf);
      lf.lfWeight = FW_BOLD;

      // Create
      BoldFont.DeleteObject();
      BoldFont.CreateFontIndirectW(&lf);

      // Re-populate
      //Populate();
   }
   
   
   // ------------------------------- PRIVATE METHODS ------------------------------

   

NAMESPACE_END2(GUI,Windows)

