
// MainFrm.h : interface of the MainWnd class
//

#pragma once
#include "ProjectWnd.h"
#include "OutputWnd.h"
#include "BackupWnd.h"
#include "PropertiesWnd.h"
#include "CommandWnd.h"
#include "GameObjectWnd.h"
#include "ScriptObjectWnd.h"
#include "ScriptView.h"
#include "Logic/GameDataWorker.h"
#include "Logic/Event.h"
#include "ToolBarEx.h"
#include "FindDialog.h"
#include "SplashWnd.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)

   // ------------------------ CLASSES ------------------------

   /// <summary>Main window</summary>
   class MainWnd : public CMDIFrameWndEx
   {
      // ------------------------ TYPES --------------------------
   protected:
      /// <summary>Defines document tab styles</summary>
      class DefaultTabStyle : public CMDITabInfo
      {
      public:
         DefaultTabStyle() 
         {
	         m_style = CMFCTabCtrl::STYLE_3D_VS2005; 
	         m_bActiveTabCloseButton = TRUE;      // Document close button
	         m_bAutoColor = TRUE;                 // Coloured tabs
	         m_bDocumentMenu = TRUE;              // Document selection context menu
            m_bTabIcons = TRUE;                  // Document icons
            m_bEnableTabSwap = TRUE;             // Swap tabs
            m_bTabCustomTooltips = TRUE;         // Tooltips
         }
      };
      // --------------------- CONSTRUCTION ----------------------
   public:
	   MainWnd();
      virtual ~MainWnd();

      // ------------------------ STATIC -------------------------
      DECLARE_DYNAMIC(MainWnd)
      DECLARE_MESSAGE_MAP()

   public:
      static FeedbackEvent    GameDataFeedback,
                              FindReplaceFeedback,
                              LoadSaveFeedback;

      const static CRect DefaultSize;

      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
   public:

      // ----------------------- MUTATORS ------------------------
   public:
      void         ActivateOutputPane(Operation pane, bool show);
      void         CreateToolBars();
      void         CreateToolWindows();
      ScriptView*  GetActiveScriptView();
      BOOL         LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL) override;
	   BOOL         PreCreateWindow(CREATESTRUCT& cs) override;

   protected:
      afx_msg void    OnClose();
      afx_msg void    OnCommandConsole()   { OnCommandShowWindow(ID_VIEW_CONSOLE); }
      afx_msg void    OnCommandFindText();
      afx_msg void    OnCommandCustomizeToolbar();
      afx_msg void    OnCommandPreferences();
      afx_msg void    OnCommandRunTests();
      afx_msg void    OnCommandShowWindow(UINT nID);
      afx_msg void    OnCommandStringLibrary();
      afx_msg void    OnCommandWindowManager();
	   afx_msg int     OnCreate(LPCREATESTRUCT lpCreateStruct);
      afx_msg LRESULT OnDocumentSwitched(WPARAM wParam, LPARAM lParam);
      handler void    OnGameDataFeedback(const WorkerProgress& wp);
      handler void    OnInitialUpdate();
      afx_msg void    OnQueryCommand(CCmdUI *pCmdUI);
	   afx_msg void    OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
      handler void    OnScriptCaretMoved(POINT pt);
      afx_msg void    OnShowWindow(BOOL bShow, UINT nStatus);
      afx_msg LRESULT OnCreateNewToolbar(WPARAM wp, LPARAM lp);
      afx_msg LRESULT OnRequestTabTooltip(WPARAM wp, LPARAM lp);
      afx_msg LRESULT OnWorkerFeedback(WPARAM wParam, LPARAM lParam);

      // -------------------- REPRESENTATION ---------------------
   protected: 
      FindDialog        m_dlgFind;
      SplashWnd         m_dlgSplash;
	   CMFCMenuBar       m_wndMenuBar;
	   ToolBarEx         m_wndFileToolBar,
                        m_wndEditToolBar,
                        m_wndViewToolBar;
	   CMFCStatusBar     m_wndStatusBar;
	   CMFCToolBarImages m_UserImages;
	   CProjectWnd       m_wndProject;
	   COutputWnd        m_wndOutput;
      CCommandWnd       m_wndCommands;
      CGameObjectWnd    m_wndGameObjects;
      CScriptObjectWnd  m_wndScriptObjects;
	   CPropertiesWnd    m_wndProperties; 
      BackupWnd         m_wndBackups;

      GameDataWorker    GameDataThread;

      FeedbackHandler   fnGameDataFeedback;
      CaretMovedHandler fnCaretMoved;
   
   private:
      bool              FirstShow;
   };



NAMESPACE_END2(GUI,Windows)



