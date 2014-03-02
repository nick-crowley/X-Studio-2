
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
	   ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &MainWnd::OnToolbarCreateNew)
      ON_UPDATE_COMMAND_UI(ID_EDIT_FIND, &MainWnd::OnQueryFindText)
   END_MESSAGE_MAP()

   // -------------------------------- CONSTRUCTION --------------------------------

   MainWnd::MainWnd() : fnGameDataFeedback(GameDataFeedback.Register(this, &MainWnd::onGameDataFeedback)),
                        fnCaretMoved(ScriptView::CaretMoved.Register(this, &MainWnd::onScriptCaretMoved))
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

         // MainMenu:
	      if (!m_wndMenuBar.Create(this)) 
            throw Win32Exception(HERE, L"Unable to create MainWnd menu");
	      m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);
         
         // Make dockable
	      m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	      EnableDocking(CBRS_ALIGN_ANY);
	      DockPane(&m_wndMenuBar);


	      
         // File ToolBar:
	      if (!m_wndFileToolBar.Create(this, IDT_FILE)) 
            throw Win32Exception(HERE, L"Unable to create MainWnd file toolbar");
         m_wndFileToolBar.SetPaneStyle(m_wndFileToolBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_GRIPPER);
	      
         // Allow user-defined toolbars operations:
	      m_wndFileToolBar.SetWindowText(L"File");
	      m_wndFileToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, IDS_TOOLBAR_CUSTOMIZE);
	      //InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);

         m_wndFileToolBar.EnableDocking(CBRS_ALIGN_ANY);
         DockPane(&m_wndFileToolBar);


         // Edit ToolBar:
	      if (!m_wndEditToolBar.Create(this, IDT_EDIT)) 
            throw Win32Exception(HERE, L"Unable to create MainWnd Edit toolbar");
         m_wndEditToolBar.SetPaneStyle(m_wndEditToolBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_GRIPPER);
	      
         // Allow user-defined toolbars operations:
	      m_wndEditToolBar.SetWindowText(L"Edit");
	      m_wndEditToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, IDS_TOOLBAR_CUSTOMIZE);
	      //InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);

         m_wndEditToolBar.EnableDocking(CBRS_ALIGN_ANY);
         DockPane(&m_wndEditToolBar);


         // View ToolBar:
	      if (!m_wndViewToolBar.Create(this, IDT_VIEW)) 
            throw Win32Exception(HERE, L"Unable to create MainWnd View toolbar");
         m_wndViewToolBar.SetPaneStyle(m_wndViewToolBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_GRIPPER);
	      
         // Allow user-defined toolbars operations:
	      m_wndViewToolBar.SetWindowText(L"View");
	      m_wndViewToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, IDS_TOOLBAR_CUSTOMIZE);
	      //InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);

         m_wndViewToolBar.EnableDocking(CBRS_ALIGN_ANY);
         DockPane(&m_wndViewToolBar);



         // StatusBar:
	      if (!m_wndStatusBar.Create(this))
            throw Win32Exception(HERE, L"Unable to create MainWnd statusBar");
	      m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));
         onScriptCaretMoved(POINT {0,0});


	      
	      


	      // enable Visual Studio 2005 style docking + AutoHide
	      CDockingManager::SetDockingMode(DT_SMART);
	      EnableAutoHidePanes(CBRS_ALIGN_ANY);

	      
         
         // Find & Replace dialog:
         m_dlgFind.Create(FindDialog::IDD, this);
         

	      // Project Window:
	      if (!m_wndProject.Create(L"Project", this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_PROJECT, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT| CBRS_FLOAT_MULTI))
            throw Win32Exception(HERE, L"Unable to create Project window");
	   
         m_wndProject.SetIcon(theApp.LoadIconW(IDR_PROJECTVIEW, ::GetSystemMetrics(SM_CXSMICON)), FALSE);
         m_wndProject.EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);
         DockPane(&m_wndProject);
         
	      // Output Window:
	      if (!m_wndOutput.Create(GuiString(IDR_OUTPUT).c_str(), this, CRect(0, 0, 100, 100), TRUE, ID_VIEW_OUTPUT, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	         throw Win32Exception(HERE, L"Unable to create Output window");

         m_wndOutput.EnableDocking(CBRS_ALIGN_BOTTOM);
	      DockPane(&m_wndOutput);
         m_wndOutput.SetIcon(theApp.LoadIconW(IDR_OUTPUT, ::GetSystemMetrics(SM_CXSMICON)), FALSE);

	      // Properties Window:
	      if (!m_wndProperties.Create(GuiString(IDR_PROPERTIES).c_str(), this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_PROPERTIES, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	         throw Win32Exception(HERE, L"Unable to create Properties window");

         m_wndProperties.SetIcon(theApp.LoadIconW(IDR_PROPERTIES, ::GetSystemMetrics(SM_CXSMICON)), FALSE);
         m_wndProperties.EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);
	      DockPane(&m_wndProperties);

         // Commands Window:
	      if (!m_wndCommands.Create(GuiString(L"Commands").c_str(), this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_COMMANDS, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	         throw Win32Exception(HERE, L"Unable to create Commands window");

         m_wndCommands.SetIcon(theApp.LoadIconW(IDR_COMMANDS, ::GetSystemMetrics(SM_CXSMICON)), FALSE);
         m_wndCommands.EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);
         DockPane(&m_wndCommands);

         // Game objects window:
         if (!m_wndGameObjects.Create(GuiString(L"Game Objects").c_str(), this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_GAME_OBJECTS, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	         throw Win32Exception(HERE, L"Unable to create Game objects window");

         m_wndGameObjects.SetIcon(theApp.LoadIconW(IDR_GAME_OBJECTS, ::GetSystemMetrics(SM_CXSMICON)), FALSE);
         m_wndGameObjects.EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);
         DockPane(&m_wndGameObjects);

         // Game objects window:
         if (!m_wndScriptObjects.Create(GuiString(L"Script Objects").c_str(), this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_SCRIPT_OBJECTS, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	         throw Win32Exception(HERE, L"Unable to create Script objects window");
         
         m_wndScriptObjects.SetIcon(theApp.LoadIconW(IDR_SCRIPT_OBJECTS, ::GetSystemMetrics(SM_CXSMICON)), FALSE);
         m_wndScriptObjects.EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);
         DockPane(&m_wndScriptObjects);
         
         
         // Set document icons??
	      UpdateMDITabbedBarsIcons();

                  
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


	      // Switch the order of document name and application name on the window title bar. This
	      // improves the usability of the taskbar because the document name is visible with the thumbnail.
	      ModifyStyle(0, FWS_PREFIXTITLE);


         // REM: Load game data
         GameDataThread.Start(new GameDataWorkerData(L"D:\\X3 Albion Prelude", GameVersion::AlbionPrelude));
	      return 0;
      }
      catch (ExceptionBase& e)
      {
         Console.Log(HERE, e);
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
   
   /// <summary>Query state of 'find text'.</summary>
   /// <param name="pCmdUI">The command UI.</param>
   void MainWnd::OnQueryFindText(CCmdUI *pCmdUI)
   {
      // Require active script document
      pCmdUI->Enable(ScriptDocument::GetActive() != nullptr);
      // Check if visible
      pCmdUI->SetCheck(m_dlgFind.IsWindowVisible());
   }
   
   /// <summary>Display caret co-ordinates in status bar</summary>
   /// <param name="pt">The caret position</param>
   void MainWnd::onScriptCaretMoved(POINT pt)
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



