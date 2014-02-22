#pragma once

#include "ProjectTreeCtrl.h"
#include "ToolBarEx.h"
#include "ImageListEx.h"
#include "Logic/Event.h"
#include "ProjectDocument.h"

/// <summary>User interface</summary>
FORWARD_DECLARATION2(Logic,Projects,class ProjectItem)

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)

   /// <summary>Project explorer toolbar</summary>
   class CProjectToolBar : public ToolBarEx
   {
      // --------------------- CONSTRUCTION ----------------------

      // ----------------------- MUTATORS ------------------------

   protected:
	   virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	   {
		   ToolBarEx::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
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
      void       AdjustLayout();
      
      handler void OnChangeVisualStyle();
	   afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
      afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
      afx_msg void OnPaint();
      handler void OnProjectLoaded();
      afx_msg void OnSetFocus(CWnd* pOldWnd);
	   afx_msg void OnSize(UINT nType, int cx, int cy);
	   afx_msg void OnTreeView_DoubleClick(NMHDR* pNMHDR, LRESULT* pResult);
	   afx_msg void OnCreateFolder();
	   afx_msg void OnAddExisting();
	   afx_msg void OnOpenItem();
	   afx_msg void OnRenameItem();
	   afx_msg void OnRemoveItem();
	   afx_msg void OnDeleteItem();
	   afx_msg void OnViewProperties();

   private:

      // -------------------- REPRESENTATION ---------------------
   protected:
	   ImageListEx     Images;
      ProjectTreeCtrl TreeView;
	   CProjectToolBar Toolbar;
      EventHandler    fnProjectLoaded;
   };


NAMESPACE_END2(GUI,Windows)


