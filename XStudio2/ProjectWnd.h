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
      handler void OnProjectChanged();
      afx_msg void OnSetFocus(CWnd* pOldWnd);
	   afx_msg void OnSize(UINT nType, int cx, int cy);
	   
	   afx_msg void OnAddFolder();
	   afx_msg void OnFileOpenWith();
	   afx_msg void OnDummyCompile();
	   afx_msg void OnEditCut();
	   afx_msg void OnEditCopy();
	   afx_msg void OnEditClear();
	   afx_msg void OnProperties();

   private:

      // -------------------- REPRESENTATION ---------------------
   protected:
	   ImageListEx     Images;
      ProjectTreeCtrl TreeView;
	   CProjectToolBar Toolbar;
      EventHandler    fnProjectChanged;
   };


NAMESPACE_END2(GUI,Windows)


