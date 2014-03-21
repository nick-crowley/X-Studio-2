
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
#include "../Logic/GameDataWorker.h"
#include "../Logic/Event.h"
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
	         //m_bDocumentMenu = TRUE;              // Document selection context menu
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
      static SimpleEvent      DocumentSwitched;
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
      void         ActivateProjectPane();
      void         ActivatePropertiesPane();
      void         ClearOutputPane(Operation pane, bool show);
      
      ScriptView*  GetActiveScriptView();
      BOOL         LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL) override;
      BOOL         OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) override;
	   BOOL         PreCreateWindow(CREATESTRUCT& cs) override;

   protected:
      void  CreateToolBars();
      void  CreateToolWindows();
      void  LoadGameData();

      afx_msg void    OnClose();
      afx_msg void    OnCommand_Console()            { OnPerformCommand(ID_VIEW_CONSOLE);         }
      afx_msg void    OnCommand_ExportProject(); //             { OnPerformCommand(ID_FILE_EXPORT);          }
      afx_msg void    OnCommand_FindText()           { OnPerformCommand(ID_EDIT_FIND);            }
      afx_msg void    OnCommand_CustomizeToolbar()   { OnPerformCommand(ID_VIEW_CUSTOMIZE);       }
      afx_msg void    OnCommand_Preferences();
      afx_msg void    OnCommand_Reload();
      afx_msg void    OnCommand_RunTests()           { OnPerformCommand(ID_TEST_RUN_ALL);         }
      afx_msg void    OnCommand_StringLibrary()      { OnPerformCommand(ID_VIEW_STRING_LIBRARY);  }
      afx_msg void    OnCommand_WindowManager()      { OnPerformCommand(ID_WINDOW_MANAGER);       }
	   afx_msg int     OnCreate(LPCREATESTRUCT lpCreateStruct);
      afx_msg LRESULT OnDocumentSwitched(WPARAM wParam, LPARAM lParam);
      handler void    OnGameDataFeedback(const WorkerProgress& wp);
      handler void    OnInitialUpdate();
      handler void    OnOpenWorkspace();
      afx_msg void    OnPerformCommand(UINT nID);
      afx_msg void    OnQueryCommand(CCmdUI *pCmdUI);
	   afx_msg void    OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
      handler void    OnScriptCaretMoved(POINT pt);
      afx_msg void    OnShowWindow(BOOL bShow, UINT nStatus);
      afx_msg LRESULT OnCreateNewToolbar(WPARAM wp, LPARAM lp);
      afx_msg LRESULT OnRequestTabTooltip(WPARAM wp, LPARAM lp);
      afx_msg LRESULT OnWorkerFeedback(WPARAM wParam, LPARAM lParam);

      // -------------------- REPRESENTATION ---------------------
   public:

   protected: 
      FindDialog        m_dlgFind;
      SplashWnd         m_dlgSplash;
	   CMFCMenuBar       m_wndMenuBar;
	   ToolBarEx         m_wndFileToolBar,
                        m_wndEditToolBar,
                        m_wndGameDataToolBar,
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
      DocumentBase*     ActiveDocument;
      bool              FirstShow;
   };



NAMESPACE_END2(GUI,Windows)



