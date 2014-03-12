#include "stdafx.h"
#include "BackupWnd.h"
#include "MainWnd.h"
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
	      if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		      throw Win32Exception(HERE, L"Failed to create dockable pane");

	      CRect rectDummy;
	      rectDummy.SetRectEmpty();
         
         // Toolbar
         ToolBar.Create(this, IDR_BACKUP, L"Backup");

         // Create List
	      if (!List.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, IDC_BACKUP_LIST))
	         throw Win32Exception(HERE, L"Failed to create backup list");

         // Setup list
         
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

   /// <summary>Loads backups for the activate document</summary>
   void BackupWnd::OnDocumentSwitched()
   {
      Console << HERE << ENDL;

      // Clear previous
      List.ResetContent();

      // Require project
      if (auto proj = ProjectDocument::GetActive())
         Populate( proj->GetAllBackups(DocumentBase::GetActive()) );
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

   /// <summary>Query state of custom toolbar command</summary>
   /// <param name="pCmdUI">The p command UI.</param>
   void BackupWnd::OnQueryCustomCommand(CCmdUI* pCmdUI)
   {
      
   }

   
   /// <summary>Called when query state of command.</summary>
   /// <param name="pCmdUI">The p command UI.</param>
   void BackupWnd::OnQueryCommand(CCmdUI* pCmdUI)
   {
      
   }

   
   /// <summary>Set focus to properties grid</summary>
   /// <param name="pOldWnd">The p old WND.</param>
   void BackupWnd::OnSetFocus(CWnd* pOldWnd)
   {
      CDockablePane::OnSetFocus(pOldWnd);
	   List.SetFocus();
   }

   /// <summary>Update font when settings change.</summary>
   /// <param name="uFlags">The flags.</param>
   /// <param name="lpszSection">The section.</param>
   void BackupWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
   {
	   CDockablePane::OnSettingChange(uFlags, lpszSection);

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
	   CDockablePane::OnSize(nType, cx, cy);
	   AdjustLayout();
   }
   
   /// <summary>Populates from a backup file.</summary>
   /// <param name="f">backup file.</param>
   void BackupWnd::Populate(BackupFile& f)
   {
      try
      {
         Console << HERE << " size=" << f.Revisions.Count << ENDL;

         for (auto& rev : f.Revisions)
            List.AddString(rev.Title.c_str());
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
   }
   
   
   // ------------------------------- PRIVATE METHODS ------------------------------

   

NAMESPACE_END2(GUI,Windows)

