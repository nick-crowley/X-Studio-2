
// MainFrm.cpp : implementation of the MainWnd class
//

#include "stdafx.h"
#include "Application.h"
#include "MainWnd.h"
#include "Logic/DebugTests.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)

   // --------------------------------- CONSTANTS ---------------------------------

   const int  iMaxUserToolbars = 10;
   const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
   const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

   // --------------------------------- GLOBAL DATA --------------------------------

   static UINT indicators[] =
   {
	   ID_SEPARATOR,           // status line indicator
      IDS_CARET_STATUS,
      ID_INDICATOR_OVR,
	   ID_INDICATOR_CAPS,
	   ID_INDICATOR_NUM,
	   ID_INDICATOR_SCRL,
   };

   // ----------------------------------- EVENTS -----------------------------------

   FeedbackEvent   MainWnd::GameDataFeedback,
                   MainWnd::FindReplaceFeedback,
                   MainWnd::LoadSaveFeedback;

   // --------------------------------- APP WIZARD ---------------------------------

   IMPLEMENT_DYNAMIC(MainWnd, CMDIFrameWndEx)

   BEGIN_MESSAGE_MAP(MainWnd, CMDIFrameWndEx)
	   ON_WM_CREATE()
      ON_WM_SETTINGCHANGE()
      ON_COMMAND(ID_EDIT_FIND, &MainWnd::OnCommandFindText)
	   ON_COMMAND(ID_TEST_RUN_ALL, &MainWnd::OnCommandRunTests)
	   ON_COMMAND(ID_VIEW_CUSTOMIZE, &MainWnd::OnCommandCustomizeToolbar)
      ON_COMMAND(ID_VIEW_STRING_LIBRARY, &MainWnd::OnCommandStringLibrary)
      ON_COMMAND(ID_WINDOW_MANAGER, &MainWnd::OnCommandWindowManager)
      ON_MESSAGE(WM_FEEDBACK, &MainWnd::OnWorkerFeedback)
      ON_COMMAND_RANGE(ID_VIEW_PROJECT, ID_VIEW_PROPERTIES, &MainWnd::OnCommandShowWindow)
	   ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &MainWnd::OnToolbarCreateNew)
      ON_UPDATE_COMMAND_UI(ID_EDIT_FIND, &MainWnd::OnQueryFindText)
      ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_PROJECT, ID_VIEW_PROPERTIES, &MainWnd::OnQueryShowWindow)
      ON_WM_SHOWWINDOW()
   END_MESSAGE_MAP()

   // -------------------------------- CONSTRUCTION --------------------------------

   MainWnd::MainWnd() : fnGameDataFeedback(GameDataFeedback.Register(this, &MainWnd::onGameDataFeedback)),
                        fnCaretMoved(ScriptView::CaretMoved.Register(this, &MainWnd::onScriptCaretMoved)),
                        FirstShow(true)
   {
   }

   MainWnd::~MainWnd()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------
   
   /// <summary>Activates an output window pane.</summary>
   /// <param name="pane">The pane.</param>
   void  MainWnd::ActivateOutputPane(Operation pane)
   {
      m_wndOutput.ActivatePane(pane);
   }

   /// <summary>Gets the active script view.</summary>
   /// <returns></returns>
   ScriptView*  MainWnd::GetActiveScriptView()
   {
      return dynamic_cast<ScriptView*>(GetActiveView());
   }

   /// <summary>Loads the frame.</summary>
   /// <param name="nIDResource">The n identifier resource.</param>
   /// <param name="dwDefaultStyle">The dw default style.</param>
   /// <param name="pParentWnd">The p parent WND.</param>
   /// <param name="pContext">The p context.</param>
   /// <returns></returns>
   BOOL MainWnd::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext)
   {
	   // base class does the real work
	   if (!CMDIFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	      return FALSE;

	   // enable customization button for all user toolbars
	   GuiString szCustomize(IDS_TOOLBAR_CUSTOMIZE);

	   for (int i = 0; i < iMaxUserToolbars; i ++)
	   {
		   CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		   if (pUserToolbar != NULL)
		      pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, szCustomize.c_str());
	   }

	   return TRUE;
   }
   
   /// <summary>App-wizard generated</summary>
   /// <param name="cs">The cs.</param>
   /// <returns></returns>
   BOOL MainWnd::PreCreateWindow(CREATESTRUCT& cs)
   {
	   if( !CMDIFrameWndEx::PreCreateWindow(cs) )
		   return FALSE;
	   // TODO: Modify the Window class or styles here by modifying
	   //  the CREATESTRUCT cs

	   return TRUE;
   }

   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Creates the tool bars.</summary>
   void MainWnd::CreateToolBars()
   {
      // MainMenu:
	   if (!m_wndMenuBar.Create(this)) 
         throw Win32Exception(HERE, L"Unable to create MainWnd menu");
	   m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);
         
	      
      // File ToolBar:
	   if (!m_wndFileToolBar.Create(this, IDT_FILE, L"File")) 
         throw Win32Exception(HERE, L"Unable to create MainWnd file toolbar");
      m_wndFileToolBar.SetPaneStyle(m_wndFileToolBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_GRIPPER);
      //m_wndFileToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, IDS_TOOLBAR_CUSTOMIZE, FALSE);
	      

      // Edit ToolBar:
	   if (!m_wndEditToolBar.Create(this, IDT_EDIT, L"Edit")) 
         throw Win32Exception(HERE, L"Unable to create MainWnd Edit toolbar");
      m_wndEditToolBar.SetPaneStyle(m_wndEditToolBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_GRIPPER);
      //m_wndEditToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, IDS_TOOLBAR_CUSTOMIZE, FALSE);
	      

      // View ToolBar:
	   if (!m_wndViewToolBar.Create(this, IDT_VIEW, L"View")) 
         throw Win32Exception(HERE, L"Unable to create MainWnd View toolbar");
      m_wndViewToolBar.SetPaneStyle(m_wndViewToolBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_GRIPPER);
      //m_wndViewToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, IDS_TOOLBAR_CUSTOMIZE, FALSE);
	      

      // StatusBar:
	   if (!m_wndStatusBar.Create(this))
         throw Win32Exception(HERE, L"Unable to create MainWnd statusBar");
	   m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));
      // Init indicators
      onScriptCaretMoved(POINT {0,0});


      // Enable docking
      EnableDocking(CBRS_ALIGN_ANY);
      EnableAutoHidePanes(CBRS_ALIGN_ANY);

      // Dock toolbars *after* creation of StatusBar, otherwise StatusBar placed within docking area
      for (auto bar : vector<CMFCBaseToolBar*>({&m_wndMenuBar, &m_wndFileToolBar, &m_wndEditToolBar, &m_wndViewToolBar}))
      {
         bar->EnableDocking(CBRS_ALIGN_ANY);
         DockPane(bar);
      }
   }


   /// <summary>Creates the tool windows.</summary>
   void MainWnd::CreateToolWindows()
   {
      static CRect DefaultSize = CRect(0, 0, 300, 500);     // output=500 high, others=300 wide

      // Project Window:
	   if (!m_wndProject.Create(L"Project", this, DefaultSize, TRUE, ID_VIEW_PROJECT, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT| CBRS_FLOAT_MULTI))
         throw Win32Exception(HERE, L"Unable to create Project window");
      m_wndProject.SetIcon(theApp.LoadIconW(IDR_PROJECTVIEW, ::GetSystemMetrics(SM_CXSMICON)), FALSE);

      // Dock left
      m_wndProject.EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);
      DockPane(&m_wndProject, AFX_IDW_DOCKBAR_LEFT);
         

	   // Properties Window:
	   if (!m_wndProperties.Create(GuiString(IDR_PROPERTIES).c_str(), this, DefaultSize, TRUE, ID_VIEW_PROPERTIES, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	      throw Win32Exception(HERE, L"Unable to create Properties window");
      m_wndProperties.SetIcon(theApp.LoadIconW(IDR_PROPERTIES, ::GetSystemMetrics(SM_CXSMICON)), FALSE);

      // Dock left, Attach to LHS tab
      m_wndProperties.EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);
      m_wndProperties.AttachToTabWnd(&m_wndProject, AFX_DOCK_METHOD::DM_SHOW);


      // Commands Window:
	   if (!m_wndCommands.Create(GuiString(L"Commands").c_str(), this, DefaultSize, TRUE, ID_VIEW_COMMANDS, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	      throw Win32Exception(HERE, L"Unable to create Commands window");
      m_wndCommands.SetIcon(theApp.LoadIconW(IDR_COMMANDS, ::GetSystemMetrics(SM_CXSMICON)), FALSE);

      // Dock right
      m_wndCommands.EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);
      DockPane(&m_wndCommands, AFX_IDW_DOCKBAR_RIGHT);


      // Game objects window:
      if (!m_wndGameObjects.Create(GuiString(L"Game Objects").c_str(), this, DefaultSize, TRUE, ID_VIEW_GAME_OBJECTS, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	      throw Win32Exception(HERE, L"Unable to create Game objects window");
      m_wndGameObjects.SetIcon(theApp.LoadIconW(IDR_GAME_OBJECTS, ::GetSystemMetrics(SM_CXSMICON)), FALSE);

      // Dock right. Attach to RHS tab
      m_wndGameObjects.EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);
      m_wndGameObjects.AttachToTabWnd(&m_wndCommands, AFX_DOCK_METHOD::DM_SHOW);


      // Game objects window:
      if (!m_wndScriptObjects.Create(GuiString(L"Script Objects").c_str(), this, DefaultSize, TRUE, ID_VIEW_SCRIPT_OBJECTS, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	      throw Win32Exception(HERE, L"Unable to create Script objects window");
      m_wndScriptObjects.SetIcon(theApp.LoadIconW(IDR_SCRIPT_OBJECTS, ::GetSystemMetrics(SM_CXSMICON)), FALSE);

      // Dock right. Attach to RHS tab
      m_wndScriptObjects.EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);
      m_wndScriptObjects.AttachToTabWnd(&m_wndCommands, AFX_DOCK_METHOD::DM_SHOW);
         

	   // Output Window:
	   if (!m_wndOutput.Create(GuiString(IDR_OUTPUT).c_str(), this, DefaultSize, TRUE, ID_VIEW_OUTPUT, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	      throw Win32Exception(HERE, L"Unable to create Output window");
      m_wndOutput.SetIcon(theApp.LoadIconW(IDR_OUTPUT, ::GetSystemMetrics(SM_CXSMICON)), FALSE);

      // Dock bottom
      m_wndOutput.EnableDocking(CBRS_ALIGN_BOTTOM);
	   DockPane(&m_wndOutput, AFX_IDW_DOCKBAR_BOTTOM);
   }

   /// <summary>Displays the Find & Replace dialog.</summary>
   void MainWnd::OnCommandFindText()
   {
      m_dlgFind.ShowWindow(m_dlgFind.IsWindowVisible() ? SW_HIDE : SW_SHOW);
   }
   
   /// <summary>Display customize toolbar dialog.</summary>
   void MainWnd::OnCommandCustomizeToolbar()
   {
	   CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* scan menus */);
	   pDlgCust->EnableUserDefinedToolbars();
	   pDlgCust->Create();
   }
   
   /// <summary>Execute debugging tests.</summary>
   void MainWnd::OnCommandRunTests()
   {
      try
      {
         DebugTests::RunAll();
         //AfxMessageBox(L"Tests completed successfully");
         Console << ENDL << "Tests completed" << ENDL;
      }
      catch (ExceptionBase& e) {
         theApp.ShowError(HERE, e, L"Unhandled exception in debugging tests");
      }
   }

   /// <summary>Display tool window.</summary>
   void MainWnd::OnCommandShowWindow(UINT nID)
   {
      // Toggle display state of tool window
      switch (nID)
      {
      case ID_VIEW_PROJECT:         m_wndProject.ShowPane(m_wndProject.IsVisible(), FALSE, TRUE);              break;
      case ID_VIEW_SCRIPT_OBJECTS:  m_wndScriptObjects.ShowPane(m_wndScriptObjects.IsVisible(), FALSE, TRUE);  break;
      case ID_VIEW_GAME_OBJECTS:    m_wndGameObjects.ShowPane(m_wndGameObjects.IsVisible(), FALSE, TRUE);      break;
      case ID_VIEW_COMMANDS:        m_wndCommands.ShowPane(m_wndCommands.IsVisible(), FALSE, TRUE);            break;
      case ID_VIEW_OUTPUT:          m_wndOutput.ShowPane(m_wndOutput.IsVisible(), FALSE, TRUE);                break;
      case ID_VIEW_PROPERTIES:      m_wndProperties.ShowPane(m_wndProperties.IsVisible(), FALSE, TRUE);        break;
      }
   }

   /// <summary>Display string library.</summary>
   void MainWnd::OnCommandStringLibrary()
   {
      theApp.OpenStringLibrary();
   }

   /// <summary>Display window manager.</summary>
   void MainWnd::OnCommandWindowManager()
   {
	   ShowWindowsDialog();
   }

   /// <summary>Creates child windows and sets up MFC GUI classes</summary>
   /// <param name="lpCreateStruct">The create data.</param>
   /// <returns></returns>
   int MainWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
   {
      try
      {
         // Create base
         if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
		      throw Win32Exception(HERE, L"Unable to CMDIFrameWndEx window");

         // Document Tabs
	      CMDITabInfo mdiTabParams;
	      mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D_ONENOTE; // other styles available...
	      mdiTabParams.m_bActiveTabCloseButton = TRUE;      // set to FALSE to place close button at right of tab area
	      mdiTabParams.m_bTabIcons = TRUE;    // set to TRUE to enable document icons on MDI taba
	      mdiTabParams.m_bAutoColor = TRUE;    // set to FALSE to disable auto-coloring of MDI tabs
	      mdiTabParams.m_bDocumentMenu = TRUE; // enable the document menu at the right edge of the tab area
	      EnableMDITabbedGroups(TRUE, mdiTabParams);

         // prevent the menu bar from taking the focus on activation
	      CMFCPopupMenu::SetForceMenuFocus(FALSE);

         // Load menu item image (not placed on any standard toolbars):
         CMFCToolBar::AddToolBarForImageCollection(IDT_CUSTOM, IDT_CUSTOM);
         CMFCToolBar::m_dblLargeImageRatio = 1.2;

         // enable quick (Alt+drag) toolbar customization
	      CMFCToolBar::EnableQuickCustomization();

         // load user-defined toolbar images
	      /*if (!CMFCToolBar::GetUserImages() && m_UserImages.Load(IDB_USER_IMAGES))
            CMFCToolBar::SetUserImages(&m_UserImages);*/

         // enable Visual Studio 2008 style docking + AutoHide
         CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
	      CDockingManager::SetDockingMode(DT_SMART);
	      
         
         
         // Create Toolbars
         CreateToolBars();
         
         // Create Tool windows
	      CreateToolWindows();
         

         // Set document icons??
	      UpdateMDITabbedBarsIcons();

         // Redraw?
	      RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	      // Enable windows management dialog
	      EnableWindowsDialog(ID_WINDOW_MANAGER, ID_WINDOW_MANAGER, TRUE);

	      // Enable 'customize' command in menu 
	      //EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, GuiString(IDS_TOOLBAR_CUSTOMIZE).c_str(), ID_VIEW_CUSTOMIZE);

	      // Switch the order of document name and application name on the window title bar. This
	      // improves the usability of the taskbar because the document name is visible with the thumbnail.
	      ModifyStyle(0, FWS_PREFIXTITLE);
	      return 0;
      }
      catch (ExceptionBase& e) {
         theApp.ShowError(HERE, e);
         return -1;
      }
   }

   
   /// <summary>Changes app state once game data is loaded.</summary>
   /// <param name="wp">The wp.</param>
   void MainWnd::onGameDataFeedback(const WorkerProgress& wp)
   {
      // Success: Change app state
      if (wp.Type == ProgressType::Succcess)
      {
         // Change state + cleanup thread
         theApp.State = AppState::GameDataPresent;
         GameDataThread.Close(true);

         // Parse command line for standard shell commands, DDE, file open
	      CCommandLineInfo cmdInfo;
	      theApp.ParseCommandLine(cmdInfo);

	      // Dispatch commands specified on the command line.
         if (cmdInfo.m_nShellCommand != CCommandLineInfo::FileNew)
	         theApp.ProcessShellCommand(cmdInfo);
      }
   }
   
   /// <summary>Called on first display</summary>
   void MainWnd::OnInitialUpdate()
   {
      // Load game data
      GameDataThread.Start(new GameDataWorkerData(L"D:\\X3 Albion Prelude", GameVersion::AlbionPrelude));

      
      // Find & Replace dialog:
      m_dlgFind.Create(FindDialog::IDD, this);
   }


   /// <summary>Query state of 'find text'.</summary>
   /// <param name="pCmdUI">The command UI.</param>
   void MainWnd::OnQueryFindText(CCmdUI *pCmdUI)
   {
      // Require active script document
      pCmdUI->Enable(ScriptDocument::GetActive() != nullptr);
      // Check if visible
      pCmdUI->SetCheck(m_dlgFind.IsWindowVisible());
   }
   
   /// <summary>Query state of tool window command.</summary>
   void MainWnd::OnQueryShowWindow(CCmdUI* pCmdUI)
   {
      BOOL state = TRUE, 
           checked = FALSE;

      // Query windows
      switch (pCmdUI->m_nID)
      {
      case ID_VIEW_PROJECT:         checked = m_wndProject.IsVisible();        break;
      case ID_VIEW_SCRIPT_OBJECTS:  checked = m_wndScriptObjects.IsVisible();  break;
      case ID_VIEW_GAME_OBJECTS:    checked = m_wndGameObjects.IsVisible();    break;
      case ID_VIEW_COMMANDS:        checked = m_wndCommands.IsVisible();       break;
      case ID_VIEW_OUTPUT:          checked = m_wndOutput.IsVisible();         break;
      case ID_VIEW_PROPERTIES:      checked = m_wndProperties.IsVisible();     break;
      }

      // Set state
      pCmdUI->Enable(state);
      pCmdUI->SetCheck(checked);
   }

   /// <summary>Display caret co-ordinates in status bar</summary>
   /// <param name="pt">The caret position</param>
   void MainWnd::onScriptCaretMoved(POINT pt)
   {
      m_wndStatusBar.SetPaneText(1, GuiString(L"Line %d  Ch %d", pt.y, pt.x).c_str());
   }
   
   /// <summary>Called when shown.</summary>
   /// <param name="bShow">show.</param>
   /// <param name="nStatus">status.</param>
   void MainWnd::OnShowWindow(BOOL bShow, UINT nStatus)
   {
      CMDIFrameWndEx::OnShowWindow(bShow, nStatus);

      // Initial update
      if (FirstShow)
      {
         FirstShow = false;
         OnInitialUpdate();
      }
         
   }

   /// <summary>Called when settings change.</summary>
   /// <param name="uFlags">The flags.</param>
   /// <param name="lpszSection">The section.</param>
   void MainWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
   {
	   CMDIFrameWndEx::OnSettingChange(uFlags, lpszSection);
   }


   /// <summary>Enable customization on user created toolsbars.  (App-Wizard generated)</summary>
   /// <param name="wp">The wp.</param>
   /// <param name="lp">The lp.</param>
   /// <returns></returns>
   LRESULT MainWnd::OnToolbarCreateNew(WPARAM wp, LPARAM lp)
   {
	   LRESULT lres = CMDIFrameWndEx::OnToolbarCreateNew(wp,lp);
	   if (lres == 0)
	      return 0;

	   CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	   ASSERT_VALID(pUserToolbar);

	   pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, GuiString(IDS_TOOLBAR_CUSTOMIZE).c_str());
	   return lres;
   }


   /// <summary>Raise appropriate events in response to worker feedback.</summary>
   /// <param name="wParam">Not used.</param>
   /// <param name="lParam">Progress data.</param>
   /// <returns></returns>
   LRESULT MainWnd::OnWorkerFeedback(WPARAM wParam, LPARAM lParam)
   {
      WorkerProgress* p = reinterpret_cast<WorkerProgress*>(lParam);

      // Raise appropriate event
      switch (p->Operation)
      {
      case Operation::LoadGameData:       GameDataFeedback.Raise(*p);      break;
      case Operation::LoadSaveDocument:   LoadSaveFeedback.Raise(*p);      break;
      case Operation::FindAndReplace1:
      case Operation::FindAndReplace2:    FindReplaceFeedback.Raise(*p);   break;
      }
      
      // Delete data
      delete p;
      return 0;
   }


   // ------------------------------- PRIVATE METHODS ------------------------------

NAMESPACE_END2(GUI,Windows)

