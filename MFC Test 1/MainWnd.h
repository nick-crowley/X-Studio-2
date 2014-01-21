
// MainFrm.h : interface of the MainWnd class
//

#pragma once
#include "ProjectWnd.h"
#include "OutputWnd.h"
#include "PropertiesWnd.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN(GUI)

   class MainWnd : public CMDIFrameWndEx
   {
	   DECLARE_DYNAMIC(MainWnd)
   public:
	   MainWnd();

   // Attributes
   public:

   // Operations
   public:

   // Overrides
   public:
	   virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	   virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);

   // Implementation
   public:
	   virtual ~MainWnd();
   #ifdef _DEBUG
	   virtual void AssertValid() const;
	   virtual void Dump(CDumpContext& dc) const;
   #endif

   protected:  // control bar embedded members
	   CMFCMenuBar       m_wndMenuBar;
	   CMFCToolBar       m_wndToolBar;
	   CMFCStatusBar     m_wndStatusBar;
	   CMFCToolBarImages m_UserImages;
	   CProjectWnd         m_wndProject;
	   COutputWnd        m_wndOutput;
	   CPropertiesWnd    m_wndProperties;

   // Generated message map functions
   protected:
	   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	   afx_msg void OnWindowManager();
	   afx_msg void OnViewCustomize();
	   afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	   afx_msg void OnApplicationLook(UINT id);
	   afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	   afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	   DECLARE_MESSAGE_MAP()

	   BOOL CreateDockingWindows();
	   void SetDockingWindowIcons(BOOL bHiColorIcons);
   };



NAMESPACE_END(GUI)



