
#pragma once

#include "FileTreeCtrl.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)

   /// <summary>Project explorer toolbar</summary>
   class CProjectToolBar : public CMFCToolBar
   {
      // --------------------- CONSTRUCTION ----------------------

      // ----------------------- MUTATORS ------------------------

   protected:
	   virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	   {
		   CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	   }

	   virtual BOOL AllowShowOnList() const { return FALSE; }
   };

   /// <summary>Project explorer docking pane</summary>
   class CProjectWnd : public CDockablePane
   {
      // ------------------------ TYPES --------------------------
   private:
	  
      // --------------------- CONSTRUCTION ----------------------
   public:
	   CProjectWnd();
      virtual ~CProjectWnd();

      // ------------------------ STATIC -------------------------
   protected:
      DECLARE_MESSAGE_MAP()

      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
      
      // ----------------------- MUTATORS ------------------------
   protected:
      void AdjustLayout();
	   void FillFileView();
      void OnChangeVisualStyle();

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

      // -------------------- REPRESENTATION ---------------------
   protected:
	   CImageList      Images;
      CFileTreeCtrl   TreeView;
	   CProjectToolBar Toolbar;
   };


NAMESPACE_END2(GUI,Windows)


