
// MainFrm.cpp : implementation of the MainWnd class
//

#include "stdafx.h"
#include "Application.h"
#include "MainWnd.h"
#include "Logic/DebugTests.h"
#include "PreferencesDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)

   // --------------------------------- CONSTANTS ---------------------------------

   /// <summary>AppWizard generated stuff</summary>
   const int  iMaxUserToolbars = 10;
   const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
   const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

   /// <summary>Default Toolwindow size  (300 wide, 500 high)</summary>
   const CRect MainWnd::DefaultSize = CRect(0, 0, 300, 500); 

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
      ON_WM_SHOWWINDOW()
      ON_MESSAGE(WM_FEEDBACK, &MainWnd::OnWorkerFeedback)
      ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &MainWnd::OnCreateNewToolbar)
      ON_REGISTERED_MESSAGE(AFX_WM_ON_GET_TAB_TOOLTIP, &MainWnd::OnRequestTabTooltip)
      ON_COMMAND(ID_EDIT_FIND, &MainWnd::OnCommandFindText)
      ON_COMMAND(ID_EDIT_PREFERENCES, &MainWnd::OnCommandPreferences)
	   ON_COMMAND(ID_TEST_RUN_ALL, &MainWnd::OnCommandRunTests)
      ON_COMMAND(ID_VIEW_CONSOLE, &MainWnd::OnCommandConsole)
	   ON_COMMAND(ID_VIEW_CUSTOMIZE, &MainWnd::OnCommandCustomizeToolbar)
      ON_COMMAND(ID_VIEW_STRING_LIBRARY, &MainWnd::OnCommandStringLibrary)
      ON_COMMAND(ID_WINDOW_MANAGER, &MainWnd::OnCommandWindowManager)
      ON_COMMAND_RANGE(ID_VIEW_PROJECT, ID_VIEW_PROPERTIES, &MainWnd::OnCommandShowWindow)
      ON_UPDATE_COMMAND_UI(ID_EDIT_FIND, &MainWnd::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_EDIT_PREFERENCES, &MainWnd::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_VIEW_STRING_LIBRARY, &MainWnd::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_VIEW_CONSOLE, &MainWnd::OnQueryCommand)
      ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_PROJECT, ID_VIEW_PROPERTIES, &MainWnd::OnQueryCommand)
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
      // Project: Dock left
      m_wndProject.Create(this);
      DockPane(&m_wndProject, AFX_IDW_DOCKBAR_LEFT);
         
	   // Properties: Dock left, Attach to LHS tab
      m_wndProperties.Create(this);
      m_wndProperties.AttachToTabWnd(&m_wndProject, AFX_DOCK_METHOD::DM_SHOW);

      
      // Commands: Dock right
      m_wndCommands.Create(this);
      DockPane(&m_wndCommands, AFX_IDW_DOCKBAR_RIGHT);

      // Game objects: Dock right. Attach to RHS tab
      m_wndGameObjects.Create(this);
      m_wndGameObjects.AttachToTabWnd(&m_wndCommands, AFX_DOCK_METHOD::DM_SHOW);

      // Script objects: Dock right. Attach to RHS tab
      m_wndScriptObjects.Create(this);
      m_wndScriptObjects.AttachToTabWnd(&m_wndCommands, AFX_DOCK_METHOD::DM_SHOW);
         

	   // Output Window: Dock bottom
      m_wndOutput.Create(this);
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
   void MainWnd::OnCommandPreferences()
   {
      // Display dialog
      PreferencesDialog dlg(this);
      dlg.DoModal();

      // Raise 'SETTINGS CHANGED'
      if (dlg.Modified)
         theApp.OnPreferencesChanged();
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
      case ID_VIEW_CONSOLE:         Console.Visible = !Console.Visible;                                         break;
      case ID_VIEW_PROJECT:         m_wndProject.ShowPane(!m_wndProject.IsVisible(), FALSE, TRUE);              break;
      case ID_VIEW_SCRIPT_OBJECTS:  m_wndScriptObjects.ShowPane(!m_wndScriptObjects.IsVisible(), FALSE, TRUE);  break;
      case ID_VIEW_GAME_OBJECTS:    m_wndGameObjects.ShowPane(!m_wndGameObjects.IsVisible(), FALSE, TRUE);      break;
      case ID_VIEW_COMMANDS:        m_wndCommands.ShowPane(!m_wndCommands.IsVisible(), FALSE, TRUE);            break;
      case ID_VIEW_OUTPUT:          m_wndOutput.ShowPane(!m_wndOutput.IsVisible(), FALSE, TRUE);                break;
      case ID_VIEW_PROPERTIES:      m_wndProperties.ShowPane(!m_wndProperties.IsVisible(), FALSE, TRUE);        break;
      }
   }

   /// <summary>Display string library.</summary>
   void MainWnd::OnCommandStringLibrary()
   {
      try
      {
         theApp.OpenStringLibrary();   
      }
      catch (ExceptionBase& e) {
         theApp.ShowError(HERE, e);
      }
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

         // Setup Document Tabs
	      EnableMDITabbedGroups(TRUE, DefaultTabStyle());

         // prevent the menu bar from taking the focus on activation
	      CMFCPopupMenu::SetForceMenuFocus(FALSE);

         // Load menu item image (not placed on any standard toolbars):
         CMFCToolBar::AddToolBarForImageCollection(IDT_MENU_IMAGES, IDT_MENU_IMAGES);
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
   
   /// <summary>Enable customization on user created toolsbars.  (App-Wizard generated)</summary>
   /// <param name="wp">The wp.</param>
   /// <param name="lp">The lp.</param>
   /// <returns></returns>
   LRESULT MainWnd::OnCreateNewToolbar(WPARAM wp, LPARAM lp)
   {
	   LRESULT lres = CMDIFrameWndEx::OnToolbarCreateNew(wp,lp);
	   if (lres == 0)
	      return 0;

	   CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	   ASSERT_VALID(pUserToolbar);

	   pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, GuiString(IDS_TOOLBAR_CUSTOMIZE).c_str());
	   return lres;
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
      GameDataThread.Start(new GameDataWorkerData(PrefsLib.GameDataFolder, PrefsLib.GameDataVersion));

      
      // Find & Replace dialog:
      m_dlgFind.Create(FindDialog::IDD, this);
   }

   /// <summary>Query menu item state</summary>
   void MainWnd::OnQueryCommand(CCmdUI* pCmdUI)
   {
      BOOL state = TRUE,
           check = FALSE;
   
      // Query 
      switch (pCmdUI->m_nID)
      {
      // Always enabled
      case ID_EDIT_PREFERENCES:  
         state = TRUE;  
         break;

      // Query find dialog
      case ID_EDIT_FIND:
         state = (ScriptDocument::GetActive() ? TRUE : FALSE);
         check = m_dlgFind.IsWindowVisible();
         break;

      // Query appropriate window
      case ID_VIEW_CONSOLE:         check = Console.Visible ? TRUE : FALSE;  break;
      case ID_VIEW_PROJECT:         check = m_wndProject.IsVisible();        break;
      case ID_VIEW_SCRIPT_OBJECTS:  check = m_wndScriptObjects.IsVisible();  break;
      case ID_VIEW_GAME_OBJECTS:    check = m_wndGameObjects.IsVisible();    break;
      case ID_VIEW_COMMANDS:        check = m_wndCommands.IsVisible();       break;
      case ID_VIEW_OUTPUT:          check = m_wndOutput.IsVisible();         break;
      case ID_VIEW_PROPERTIES:      check = m_wndProperties.IsVisible();     break;

      // Query string library document
      case ID_VIEW_STRING_LIBRARY:
         state = (theApp.State == AppState::GameDataPresent ? TRUE : FALSE);
         check = theApp.IsDocumentOpen(L"String Library") ? TRUE : FALSE;
         break;
      }

      // Set state
      pCmdUI->Enable(state);
      pCmdUI->SetCheck(check);
   }
   

   /// <summary>Called to request tab tooltip.</summary>
   /// <param name="wp">The wp.</param>
   /// <param name="lp">The lp.</param>
   /// <returns></returns>
   LRESULT  MainWnd::OnRequestTabTooltip(WPARAM wp, LPARAM lp)
   {
      auto info = reinterpret_cast<CMFCTabToolTipInfo*>(lp);
    
      // TODO: Display document path
      info->m_strText = L"TODO: Display document path";
      return 0;
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

