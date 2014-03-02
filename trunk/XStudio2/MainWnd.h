
// MainFrm.h : interface of the MainWnd class
//

#pragma once
#include "ProjectWnd.h"
#include "OutputWnd.h"
#include "PropertiesWnd.h"
#include "CommandWnd.h"
#include "GameObjectWnd.h"
#include "ScriptObjectWnd.h"
#include "ScriptView.h"
#include "Logic/GameDataWorker.h"
#include "Logic/Event.h"
#include "ToolBarEx.h"
#include "FindDialog.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)

   // ------------------------ CLASSES ------------------------

   /// <summary>Main window</summary>
   class MainWnd : public CMDIFrameWndEx
   {
      // ------------------------ TYPES --------------------------

      // --------------------- CONSTRUCTION ----------------------
	   
   public:
	   MainWnd();
      virtual ~MainWnd();

      // ------------------------ STATIC -------------------------
   public:
      DECLARE_DYNAMIC(MainWnd)
   protected:
      DECLARE_MESSAGE_MAP()

   public:
      static FeedbackEvent    GameDataFeedback,
                              FindReplaceFeedback,
                              LoadSaveFeedback;

      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
   public:
   #ifdef _DEBUG
	   virtual void AssertValid() const;
	   virtual void Dump(CDumpContext& dc) const;
   #endif 

      // ----------------------- MUTATORS ------------------------
   
   public:
      void         ActivateOutputPane(Operation pane);
      ScriptView*  GetActiveScriptView();
      BOOL         LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL) override;
	   BOOL         PreCreateWindow(CREATESTRUCT& cs) override;

   protected:
      afx_msg void    OnCommandFindText();
      afx_msg void    OnCommandCustomizeToolbar();
      afx_msg void    OnCommandRunTests();
      afx_msg void    OnCommandShowWindow(UINT nID);
      afx_msg void    OnCommandStringLibrary();
      afx_msg void    OnCommandWindowManager();
	   afx_msg int     OnCreate(LPCREATESTRUCT lpCreateStruct);
      handler void    onGameDataFeedback(const WorkerProgress& wp);
      afx_msg void    OnQueryFindText(CCmdUI *pCmdUI);
      afx_msg void    OnQueryShowWindow(CCmdUI *pCmdUI);
	   afx_msg void    OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
      handler void    onScriptCaretMoved(POINT pt);
      afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
      afx_msg LRESULT OnWorkerFeedback(WPARAM wParam, LPARAM lParam);

      // -------------------- REPRESENTATION ---------------------

   protected: 
      FindDialog        m_dlgFind;
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

      GameDataWorker    GameDataThread;

      FeedbackHandler   fnGameDataFeedback;
      CaretMovedHandler fnCaretMoved;
   };



NAMESPACE_END2(GUI,Windows)



