
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
                   MainWnd::LoadSaveFeedback;

   // --------------------------------- APP WIZARD ---------------------------------

   IMPLEMENT_DYNAMIC(MainWnd, CMDIFrameWndEx)

   BEGIN_MESSAGE_MAP(MainWnd, CMDIFrameWndEx)
	   ON_WM_CREATE()
	   ON_COMMAND(ID_WINDOW_MANAGER, &MainWnd::OnWindowManager)
	   ON_COMMAND(ID_VIEW_CUSTOMIZE, &MainWnd::OnViewCustomize)
      ON_COMMAND(ID_VIEW_STRING_LIBRARY, &MainWnd::OnViewStringLibrary)
      ON_COMMAND(ID_TEST_RUN_ALL, &MainWnd::OnRunAllTests)
	   ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &MainWnd::OnToolbarCreateNew)
      ON_MESSAGE(WM_FEEDBACK, &MainWnd::OnWorkerFeedback)
	   ON_WM_SETTINGCHANGE()
   END_MESSAGE_MAP()

   // -------------------------------- CONSTRUCTION --------------------------------

   MainWnd::MainWnd() : fnGameDataFeedback(GameDataFeedback.Register(this, &MainWnd::onGameDataFeedback)),
                        fnCaretMoved(ScriptView::CaretMoved.Register(this, &MainWnd::onScriptViewCaretMoved))
   {
	   //theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2008);
   }

   MainWnd::~MainWnd()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------
   
   #ifdef _DEBUG
   void MainWnd::AssertValid() const
   {
	   CMDIFrameWndEx::AssertValid();
   }

   void MainWnd::Dump(CDumpContext& dc) const
   {
	   CMDIFrameWndEx::Dump(dc);
   }
   #endif //_DEBUG

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
   
   BOOL MainWnd::PreCreateWindow(CREATESTRUCT& cs)
   {
	   if( !CMDIFrameWndEx::PreCreateWindow(cs) )
		   return FALSE;
	   // TODO: Modify the Window class or styles here by modifying
	   //  the CREATESTRUCT cs

	   return TRUE;
   }

   // ------------------------------ PROTECTED METHODS -----------------------------

   int MainWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
   {
	   if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
		   return -1;

      try
      {
         // Document Tabs
	      CMDITabInfo mdiTabParams;
	      mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D_ONENOTE; // other styles available...
	      mdiTabParams.m_bActiveTabCloseButton = TRUE;      // set to FALSE to place close button at right of tab area
	      mdiTabParams.m_bTabIcons = TRUE;    // set to TRUE to enable document icons on MDI taba
	      mdiTabParams.m_bAutoColor = TRUE;    // set to FALSE to disable auto-coloring of MDI tabs
	      mdiTabParams.m_bDocumentMenu = TRUE; // enable the document menu at the right edge of the tab area
	      EnableMDITabbedGroups(TRUE, mdiTabParams);


         // MainMenu:
	      if (!m_wndMenuBar.Create(this))
            throw Win32Exception(HERE, L"Unable to create MainWnd menu");

	      m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	      // prevent the menu bar from taking the focus on activation
	      CMFCPopupMenu::SetForceMenuFocus(FALSE);


         // ToolBar:
	      if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		       !m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
            throw Win32Exception(HERE, L"Unable to create MainWnd toolbar");
	      
	      m_wndToolBar.SetWindowText(GuiString(IDS_TOOLBAR_STANDARD).c_str());
	      m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, GuiString(IDS_TOOLBAR_CUSTOMIZE).c_str());

	      // Allow user-defined toolbars operations:
	      InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);


         // StatusBar:
	      if (!m_wndStatusBar.Create(this))
            throw Win32Exception(HERE, L"Unable to create MainWnd statusBar");
	      m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));
         onScriptViewCaretMoved(POINT {0,0});


	      // Make Toolbar/MenuBar dockable
	      m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	      m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	      EnableDocking(CBRS_ALIGN_ANY);
	      DockPane(&m_wndMenuBar);
	      DockPane(&m_wndToolBar);


	      // enable Visual Studio 2005 style docking + AutoHide
	      CDockingManager::SetDockingMode(DT_SMART);
	      EnableAutoHidePanes(CBRS_ALIGN_ANY);

	      // Load menu item image (not placed on any standard toolbars):
	      CMFCToolBar::AddToolBarForImageCollection(IDR_CUSTOM, IDR_CUSTOM);


	      // Project Window:
	      if (!m_wndProject.Create(GuiString(IDR_PROJECT).c_str(), this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_PROJECT, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT| CBRS_FLOAT_MULTI))
            throw Win32Exception(HERE, L"Unable to create Project window");
	   
	      // Output Window:
	      if (!m_wndOutput.Create(GuiString(IDR_OUTPUT).c_str(), this, CRect(0, 0, 100, 100), TRUE, ID_VIEW_OUTPUT, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	         throw Win32Exception(HERE, L"Unable to create Output window");

	      // Properties Window:
	      if (!m_wndProperties.Create(GuiString(IDR_PROPERTIES).c_str(), this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_PROPERTIES, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	         throw Win32Exception(HERE, L"Unable to create Properties window");

         // Commands Window:
	      if (!m_wndCommands.Create(GuiString(L"Commands").c_str(), this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_COMMANDS, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	         throw Win32Exception(HERE, L"Unable to create Commands window");

         // Game objects window:
         if (!m_wndGameObjects.Create(GuiString(L"Game Objects").c_str(), this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_GAME_OBJECTS, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	         throw Win32Exception(HERE, L"Unable to create Game objects window");

         // Game objects window:
         if (!m_wndScriptObjects.Create(GuiString(L"Script Objects").c_str(), this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_SCRIPT_OBJECTS, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	         throw Win32Exception(HERE, L"Unable to create Script objects window");
         
         
         // Set icons
         m_wndProject.SetIcon(theApp.LoadIconW(IDR_PROJECT, ::GetSystemMetrics(SM_CXSMICON)), FALSE);
         m_wndOutput.SetIcon(theApp.LoadIconW(IDR_OUTPUT, ::GetSystemMetrics(SM_CXSMICON)), FALSE);
         m_wndProperties.SetIcon(theApp.LoadIconW(IDR_PROPERTIES, ::GetSystemMetrics(SM_CXSMICON)), FALSE);
         m_wndCommands.SetIcon(theApp.LoadIconW(IDR_PROPERTIES, ::GetSystemMetrics(SM_CXSMICON)), FALSE);
         m_wndGameObjects.SetIcon(theApp.LoadIconW(IDR_PROPERTIES, ::GetSystemMetrics(SM_CXSMICON)), FALSE);
         m_wndScriptObjects.SetIcon(theApp.LoadIconW(IDR_PROPERTIES, ::GetSystemMetrics(SM_CXSMICON)), FALSE);
	      UpdateMDITabbedBarsIcons();

         // Dock each window
	      m_wndProject.EnableDocking(CBRS_ALIGN_ANY);
         DockPane(&m_wndProject);
	   
	      m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
	      DockPane(&m_wndOutput);

	      m_wndProperties.EnableDocking(CBRS_ALIGN_ANY);
	      DockPane(&m_wndProperties);

         m_wndCommands.EnableDocking(CBRS_ALIGN_ANY);
         DockPane(&m_wndCommands);

         m_wndGameObjects.EnableDocking(CBRS_ALIGN_ANY);
         DockPane(&m_wndGameObjects);

         m_wndScriptObjects.EnableDocking(CBRS_ALIGN_ANY);
         DockPane(&m_wndScriptObjects);


	      // set the visual manager and style 
	      CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		   CDockingManager::SetDockingMode(DT_SMART);
	      m_wndOutput.UpdateFonts();
	      RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);


	      // Enable windows management dialog
	      EnableWindowsDialog(ID_WINDOW_MANAGER, ID_WINDOW_MANAGER, TRUE);

	      // Enable 'customize' command in menu 
	      EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, GuiString(IDS_TOOLBAR_CUSTOMIZE).c_str(), ID_VIEW_CUSTOMIZE);

	      // enable quick (Alt+drag) toolbar customization
	      CMFCToolBar::EnableQuickCustomization();

	      if (CMFCToolBar::GetUserImages() == NULL)
	      {
		      // load user-defined toolbar images
		      if (m_UserImages.Load(_T(".\\UserImages.bmp")))
		      {
			      CMFCToolBar::SetUserImages(&m_UserImages);
		      }
	      }

	      // enable menu personalization (most-recently used commands)
	      // TODO: define your own basic commands, ensuring that each pulldown menu has at least one basic command.
	      CList<UINT, UINT> lstBasicCommands;

	      lstBasicCommands.AddTail(ID_FILE_NEW);
	      lstBasicCommands.AddTail(ID_FILE_OPEN);
	      lstBasicCommands.AddTail(ID_FILE_SAVE);
	      lstBasicCommands.AddTail(ID_FILE_PRINT);
	      lstBasicCommands.AddTail(ID_APP_EXIT);
	      lstBasicCommands.AddTail(ID_EDIT_CUT);
	      lstBasicCommands.AddTail(ID_EDIT_PASTE);
	      lstBasicCommands.AddTail(ID_EDIT_UNDO);
	      lstBasicCommands.AddTail(ID_APP_ABOUT);
	      lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
	      lstBasicCommands.AddTail(ID_VIEW_CUSTOMIZE);

	      CMFCToolBar::SetBasicCommands(lstBasicCommands);

	      // Switch the order of document name and application name on the window title bar. This
	      // improves the usability of the taskbar because the document name is visible with the thumbnail.
	      ModifyStyle(0, FWS_PREFIXTITLE);


         // Load game data
         GameDataThread.Start(new GameDataWorkerData(L"D:\\X3 Albion Prelude", GameVersion::AlbionPrelude));
	      return 0;
      }
      catch (ExceptionBase& e)
      {
         Console.Log(HERE, e);
         return -1;
      }
   }

   void MainWnd::onGameDataFeedback(const WorkerProgress& wp)
   {
      // Success: Change app state
      if (wp.Type == ProgressType::Succcess)
      {
         theApp.State = AppState::GameDataPresent;

         // Parse command line for standard shell commands, DDE, file open
	      CCommandLineInfo cmdInfo;
	      theApp.ParseCommandLine(cmdInfo);

	      // Dispatch commands specified on the command line.
         if (cmdInfo.m_nShellCommand != CCommandLineInfo::FileNew)
	         theApp.ProcessShellCommand(cmdInfo);
      }
   }
   
   void MainWnd::OnRunAllTests()
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

   /// <summary>Display caret co-ordinates in status bar</summary>
   /// <param name="pt">The caret position</param>
   void MainWnd::onScriptViewCaretMoved(POINT pt)
   {
      m_wndStatusBar.SetPaneText(1, GuiString(L"Line %d  Ch %d", pt.y, pt.x).c_str());
   }

   void MainWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
   {
	   CMDIFrameWndEx::OnSettingChange(uFlags, lpszSection);
	   m_wndOutput.UpdateFonts();
   }


   LRESULT MainWnd::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
   {
	   LRESULT lres = CMDIFrameWndEx::OnToolbarCreateNew(wp,lp);
	   if (lres == 0)
	      return 0;

	   CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	   ASSERT_VALID(pUserToolbar);

	   pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, GuiString(IDS_TOOLBAR_CUSTOMIZE).c_str());
	   return lres;
   }


   void MainWnd::OnViewCustomize()
   {
	   CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* scan menus */);
	   pDlgCust->EnableUserDefinedToolbars();
	   pDlgCust->Create();
   }

   void MainWnd::OnViewStringLibrary()
   {
      theApp.OpenStringLibrary();
   }

   void MainWnd::OnWindowManager()
   {
	   ShowWindowsDialog();
   }

   
   LRESULT MainWnd::OnWorkerFeedback(WPARAM wParam, LPARAM lParam)
   {
      WorkerProgress* p = reinterpret_cast<WorkerProgress*>(lParam);

      // Raise appropriate event
      switch (p->Operation)
      {
      case Operation::LoadGameData:       GameDataFeedback.Raise(*p);   break;
      case Operation::LoadSaveDocument:   LoadSaveFeedback.Raise(*p);   break;
      // TODO: Add more operations
      }
      
      // Delete data
      delete p;
      return 0;
   }


   // ------------------------------- PRIVATE METHODS ------------------------------

NAMESPACE_END2(GUI,Windows)

