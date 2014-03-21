
// MainFrm.cpp : implementation of the MainWnd class
//

#include "stdafx.h"
#include "Application.h"
#include "MainWnd.h"
#include "../Testing/LogicTests.h"
#include "PreferencesDialog.h"
#include "ExportProjectDialog.h"

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

   /// <summary>Active Document changed</summary>
   SimpleEvent    MainWnd::DocumentSwitched;

   /// <summary>Feedback events</summary>
   FeedbackEvent   MainWnd::GameDataFeedback,
                   MainWnd::FindReplaceFeedback,
                   MainWnd::LoadSaveFeedback;

   // --------------------------------- APP WIZARD ---------------------------------

   IMPLEMENT_DYNAMIC(MainWnd, CMDIFrameWndEx)

   BEGIN_MESSAGE_MAP(MainWnd, CMDIFrameWndEx)
	   ON_WM_CREATE()
      ON_WM_CLOSE()
      ON_WM_SETTINGCHANGE()
      ON_WM_SHOWWINDOW()
      ON_MESSAGE(WM_FEEDBACK, &MainWnd::OnWorkerFeedback)
      ON_REGISTERED_MESSAGE(AFX_WM_CHANGE_ACTIVE_TAB, &MainWnd::OnDocumentSwitched)
      ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &MainWnd::OnCreateNewToolbar)
      ON_REGISTERED_MESSAGE(AFX_WM_ON_GET_TAB_TOOLTIP, &MainWnd::OnRequestTabTooltip)
      ON_COMMAND(ID_FILE_EXPORT, &MainWnd::OnCommandExportProject)
      ON_COMMAND(ID_EDIT_FIND, &MainWnd::OnCommandFindText)
      ON_COMMAND(ID_EDIT_PREFERENCES, &MainWnd::OnCommandEditPreferences)
	   ON_COMMAND(ID_TEST_RUN_ALL, &MainWnd::OnCommandRunTests)
      ON_COMMAND(ID_VIEW_CONSOLE, &MainWnd::OnCommandConsole)
	   ON_COMMAND(ID_VIEW_CUSTOMIZE, &MainWnd::OnCommandCustomizeToolbar)
      ON_COMMAND(ID_VIEW_STRING_LIBRARY, &MainWnd::OnCommandStringLibrary)
      ON_COMMAND(ID_WINDOW_MANAGER, &MainWnd::OnCommandWindowManager)
      ON_COMMAND_RANGE(ID_VIEW_PROJECT, ID_VIEW_PROPERTIES, &MainWnd::OnPerformCommand)
      ON_UPDATE_COMMAND_UI(ID_FILE_EXPORT, &MainWnd::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_EDIT_FIND, &MainWnd::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_EDIT_PREFERENCES, &MainWnd::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_VIEW_STRING_LIBRARY, &MainWnd::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_VIEW_CONSOLE, &MainWnd::OnQueryCommand)
      ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_PROJECT, ID_VIEW_PROPERTIES, &MainWnd::OnQueryCommand)
   END_MESSAGE_MAP()

   // -------------------------------- CONSTRUCTION --------------------------------

   MainWnd::MainWnd() : fnGameDataFeedback(GameDataFeedback.Register(this, &MainWnd::OnGameDataFeedback)),
                        fnCaretMoved(ScriptView::CaretMoved.Register(this, &MainWnd::OnScriptCaretMoved)),
                        ActiveDocument(nullptr), FirstShow(true)
   {
   }

   MainWnd::~MainWnd()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------
   
   /// <summary>Set the current output window tab.</summary>
   /// <param name="pane">Operation type.</param>
   /// <param name="show">Whether to pop-up output window.</param>
   void  MainWnd::ActivateOutputPane(Operation pane, bool show)
   {
      m_wndOutput.ActivatePane(pane);

      if (show)
         m_wndOutput.ShowPane(TRUE, FALSE, TRUE);
   }
   
   /// <summary>Activates the project window.</summary>
   void  MainWnd::ActivateProjectPane()
   {
      m_wndProject.ShowPane(TRUE, FALSE, TRUE);
   }
   
   /// <summary>Activates the properties window.</summary>
   void  MainWnd::ActivatePropertiesPane()
   {
      m_wndProperties.ShowPane(TRUE, FALSE, TRUE);
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

   /// <summary>Extends MFC command routing to the project document, which has no attached view.</summary>
   /// <param name="nID">The identifier.</param>
   /// <param name="nCode">The code.</param>
   /// <param name="pExtra">UI handler.</param>
   /// <param name="pHandlerInfo">The handler information.</param>
   /// <returns></returns>
   BOOL MainWnd::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
   {
      // Delegate to project
      if (auto proj = ProjectDocument::GetActive())
         if (proj->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
             return TRUE;

      // Default
      return __super::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
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
	      
      
      // GameData ToolBar:
	   if (!m_wndGameDataToolBar.Create(this, IDT_GAMEDATA, L"Game Data")) 
         throw Win32Exception(HERE, L"Unable to create MainWnd GameData toolbar");
      m_wndGameDataToolBar.SetPaneStyle(m_wndGameDataToolBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_GRIPPER);
      //m_wndGameDataToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, IDS_TOOLBAR_CUSTOMIZE, FALSE);
	      

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
      OnScriptCaretMoved(POINT {0,0});


      // Enable docking
      EnableDocking(CBRS_ALIGN_ANY);
      EnableAutoHidePanes(CBRS_ALIGN_ANY);

      // Dock toolbars *after* creation of StatusBar, otherwise StatusBar placed within docking area
      for (auto bar : vector<CMFCBaseToolBar*>({&m_wndMenuBar, &m_wndFileToolBar, &m_wndEditToolBar, &m_wndGameDataToolBar, &m_wndViewToolBar}))
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

      // Backup: Dock left, Attach to LHS tab
      m_wndBackups.Create(this);
      m_wndBackups.AttachToTabWnd(&m_wndProject, AFX_DOCK_METHOD::DM_SHOW);
      

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
   
   /// <summary>Execute debugging tests.</summary>
   void MainWnd::OnCommandEditPreferences()
   {
      // Display dialog
      PreferencesDialog dlg(this);
      dlg.DoModal();

      // Raise 'SETTINGS CHANGED'
      if (dlg.Modified)
         theApp.OnPreferencesChanged();
   }
   
   /// <summary>Execute debugging tests.</summary>
   void MainWnd::OnCommandExportProject()
   {
      // Require project
      if (!ProjectDocument::GetActive())
         return;

      // Display dialog
      ExportProjectDialog dlg(this);
      dlg.DoModal();
   }

   /// <summary>Stores current workspace.</summary>
   void MainWnd::OnClose()
   {
      list<wstring> docs;

      // Get path of non-virtual files
      for (auto& doc : theApp.GetOpenDocuments())
         if (!doc->Virtual)
            docs.push_back(doc->FullPath.c_str());

      // Save current open docs to preferences
      PrefsLib.WorkspaceDocuments = docs;

      // Close 
      CMDIFrameWndEx::OnClose();
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
         CMFCToolBar::AddToolBarForImageCollection(IDT_FILE, IDT_FILE);
         CMFCToolBar::AddToolBarForImageCollection(IDT_EDIT, IDT_EDIT);
         CMFCToolBar::AddToolBarForImageCollection(IDT_GAMEDATA, IDT_GAMEDATA);
         CMFCToolBar::AddToolBarForImageCollection(IDT_VIEW, IDT_VIEW);
         CMFCToolBar::AddToolBarForImageCollection(IDR_EDITOR, IDR_EDITOR);
         CMFCToolBar::AddToolBarForImageCollection(IDR_PROJECTVIEW, IDR_PROJECTVIEW);
         CMFCToolBar::AddToolBarForImageCollection(IDR_BACKUP, IDR_BACKUP);
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

   
   /// <summary>Called when document switched.</summary>
   /// <param name="wParam">Tab Index</param>
   /// <param name="lParam">TabCtrl pointer.</param>
   /// <returns></returns>
   LRESULT  MainWnd::OnDocumentSwitched(WPARAM wParam, LPARAM lParam)
   {
      UINT Index = wParam;
      auto tab = reinterpret_cast<CMFCTabCtrl*>(lParam);

      // Get active
      auto doc = DocumentBase::GetActive();

      // Raise 'DOCUMENT SWITCHED' 
      if (doc != ActiveDocument)    // Filter the excessive AFX_WM_CHANGE_ACTIVE_TAB messages by recording the last active document 
      {
         ActiveDocument = doc;       
         DocumentSwitched.Raise();
      }

      /*GetMDITabs().SetActiveTabColor(RGB(255,0,0));
      GetMDITabs().SetTabBkColor(0, RGB(255,0,0));*/

      return 0;
   }

   
   /// <summary>Changes app state once game data is loaded.</summary>
   /// <param name="wp">The wp.</param>
   void MainWnd::OnGameDataFeedback(const WorkerProgress& wp)
   {
      // Exit 'splash screen' mode
      EnableWindow(TRUE);

      // Close worker
      GameDataThread.Close();

      // Success: 
      if (wp.Type == ProgressType::Succcess)
      {
         // Change app state 
         theApp.State = AppState::GameDataPresent;
         
         // Hide splash screen
         m_dlgSplash.DestroyWindow();
         
         // Parse command line 
	      CCommandLineInfo cmdInfo;
	      theApp.ParseCommandLine(cmdInfo);

         // CommandLine: Open file
         if (cmdInfo.m_nShellCommand != CCommandLineInfo::FileNew)
	         theApp.ProcessShellCommand(cmdInfo);
         else
            OnOpenWorkspace();
      }
   }

   /// <summary>Called on first display</summary>
   void MainWnd::OnInitialUpdate()
   {
      try
      {
         if (!PrefsLib.GameDataFolder.Empty())
         {
            // Show splash screen
            m_dlgSplash.Create(this, 0);
            m_dlgSplash.ShowWindow(SW_SHOW);
            EnableWindow(FALSE);

            // Load game data
            GameDataThread.Start();
         }
         
         // Find & Replace dialog:
         m_dlgFind.Create(FindDialog::IDD, this);
      }
      catch (ExceptionBase& e) {
         Console.Log(HERE, e);
      }
   }
   
   /// <summary>Called when opening workspace.</summary>
   void MainWnd::OnOpenWorkspace()
   {
      auto workspace = PrefsLib.WorkspaceDocuments;

      if (!workspace.empty())
      {
         // Feedback:
         Console << Cons::UserAction << "Restoring previous workspace documents" << ENDL;

         // Default: Restore previous workspace
         for (auto& doc : workspace)
         {
            if (Path(doc).Exists())
               theApp.OpenDocumentFile(doc.c_str(), FALSE);
            else
            {
               Console << Cons::Warning << "Cannot find previously open document: " << Path(doc) << ENDL;
               theApp.ShowMessage(L"Cannot find previously open document: "+doc, MB_OK|MB_ICONWARNING);
            }
         }

         // Feedback:
         Console << Cons::UserAction << "Successfully restored previous workspace documents" << ENDL;
      }
   }
   
   /// <summary>Display tool window.</summary>
   void MainWnd::OnPerformCommand(UINT nID)
   {
      CMFCToolBarsCustomizeDialog* pDlgCust;

      try
      {
         switch (nID)
         {
         // Show/Hide window
         case ID_WINDOW_MANAGER:       ShowWindowsDialog();                                                        break;
         case ID_VIEW_CONSOLE:         Console.Visible = !Console.Visible;                                         break;
         case ID_VIEW_STRING_LIBRARY:  theApp.OpenStringLibrary();                                                 break;
         case ID_VIEW_PROJECT:         m_wndProject.ShowPane(!m_wndProject.IsVisible(), FALSE, TRUE);              break;
         case ID_VIEW_SCRIPT_OBJECTS:  m_wndScriptObjects.ShowPane(!m_wndScriptObjects.IsVisible(), FALSE, TRUE);  break;
         case ID_VIEW_GAME_OBJECTS:    m_wndGameObjects.ShowPane(!m_wndGameObjects.IsVisible(), FALSE, TRUE);      break;
         case ID_VIEW_COMMANDS:        m_wndCommands.ShowPane(!m_wndCommands.IsVisible(), FALSE, TRUE);            break;
         case ID_VIEW_OUTPUT:          m_wndOutput.ShowPane(!m_wndOutput.IsVisible(), FALSE, TRUE);                break;
         case ID_VIEW_BACKUP:          m_wndBackups.ShowPane(!m_wndBackups.IsVisible(), FALSE, TRUE);              break;
         case ID_VIEW_PROPERTIES:      m_wndProperties.ShowPane(!m_wndProperties.IsVisible(), FALSE, TRUE);        break;
         case ID_EDIT_FIND:            m_dlgFind.ShowWindow(m_dlgFind.IsWindowVisible() ? SW_HIDE : SW_SHOW);      break;

         // Customize Toolbar
         case ID_VIEW_CUSTOMIZE: 
            pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* scan menus */);
	         pDlgCust->EnableUserDefinedToolbars();
	         pDlgCust->Create();
            break;

         // Run Test
         case ID_TEST_RUN_ALL: 
            Testing::LogicTests::RunAll();   
            break;
         }
      }
      catch (ExceptionBase& e) {
         theApp.ShowError(HERE, e);
      }
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
      case ID_VIEW_BACKUP:          check = m_wndBackups.IsVisible();        break;
      case ID_VIEW_SCRIPT_OBJECTS:  check = m_wndScriptObjects.IsVisible();  break;
      case ID_VIEW_GAME_OBJECTS:    check = m_wndGameObjects.IsVisible();    break;
      case ID_VIEW_COMMANDS:        check = m_wndCommands.IsVisible();       break;
      case ID_VIEW_OUTPUT:          check = m_wndOutput.IsVisible();         break;
      case ID_VIEW_PROPERTIES:      check = m_wndProperties.IsVisible();     break;

      // Project Export/Close/Save/SaveAs: Require project
      case ID_FILE_EXPORT:
      case ID_FILE_PROJECT_SAVE:
      case ID_FILE_PROJECT_CLOSE: 
      case ID_FILE_PROJECT_SAVE_AS: 
         state = ProjectDocument::GetActive() ? TRUE : FALSE;  
         break;

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
   void MainWnd::OnScriptCaretMoved(POINT pt)
   {
      m_wndStatusBar.SetPaneText(1, VString(L"Line %d  Ch %d", pt.y, pt.x).c_str());
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

      // Initial Feedback: Activate appropriate pane
      if (p->Type == ProgressType::Operation)
         ActivateOutputPane(p->Operation, false);

      // Raise appropriate event
      switch (p->Operation)
      {
      case Operation::LoadGameData:       GameDataFeedback.Raise(*p);      break;
      case Operation::ImportProject:
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

