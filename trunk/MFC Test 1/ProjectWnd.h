
#pragma once

#include "FileTreeCtrl.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN(GUI)

   class CFileViewToolBar : public CMFCToolBar
   {
	   virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	   {
		   CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	   }

	   virtual BOOL AllowShowOnList() const { return FALSE; }
   };

   class CProjectWnd : public CDockablePane
   {
   // Construction
   public:
	   CProjectWnd();

	   void AdjustLayout();
	   void OnChangeVisualStyle();

   // Attributes
   protected:

	   CFileTreeCtrl TreeView;
	   CImageList Images;
	   CFileViewToolBar Toolbar;

   protected:
	   void FillFileView();

   // Implementation
   public:
	   virtual ~CProjectWnd();

   protected:
	   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	   afx_msg void OnSize(UINT nType, int cx, int cy);
	   afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	   afx_msg void OnProperties();
	   afx_msg void OnFileOpen();
	   afx_msg void OnFileOpenWith();
	   afx_msg void OnDummyCompile();
	   afx_msg void OnEditCut();
	   afx_msg void OnEditCopy();
	   afx_msg void OnEditClear();
	   afx_msg void OnPaint();
	   afx_msg void OnSetFocus(CWnd* pOldWnd);

	   DECLARE_MESSAGE_MAP()
   };


NAMESPACE_END(GUI)


