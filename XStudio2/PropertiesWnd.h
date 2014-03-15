
#pragma once
#include "Logic/Event.h"
#include "ToolBarEx.h"
#include "PropertySource.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)

   // ------------------------------------ ENUMS -----------------------------------

   // ----------------------------- EVENTS AND DELEGATES ---------------------------

   // ----------------------------------- CLASSES ----------------------------------

   /// <summary>Dockable properties window</summary>
   class CPropertiesWnd : public CDockablePane
   {
      // ------------------------ TYPES --------------------------
   public:
      /// <summary>Properties window toolbar</summary>
      class PropertiesToolBar : public ToolBarEx
      {
         // --------------------- CONSTRUCTION ----------------------

         // ---------------------- ACCESSORS ------------------------			
      
         // ----------------------- MUTATORS ------------------------
      public:
	      virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	      {
		      __super::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	      }

	      virtual BOOL AllowShowOnList() const { return FALSE; }
      };

      // --------------------- CONSTRUCTION ----------------------
   public:
      CPropertiesWnd();
      virtual ~CPropertiesWnd();
       
      // ------------------------ STATIC -------------------------
   public:
      static void  Connect(PropertySource* src, bool connect);
      
      DECLARE_MESSAGE_MAP()

      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
      
      // ----------------------- MUTATORS ------------------------
   public:
      void  Create(CWnd* parent);
   
   protected:
      void AdjustLayout();
      void ConnectSource(PropertySource* src, bool connect);
	   void UpdateFont();

	   afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
      afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
      afx_msg void OnCommandExpandAll();
      afx_msg void OnCommandSort();
      afx_msg void OnPaint();
      afx_msg void OnQueryCommand(CCmdUI* pCmdUI);
      afx_msg void OnQueryCustomCommand(CCmdUI* pCmdUI);
      afx_msg LRESULT OnPropertyUpdated(WPARAM wParam, LPARAM lParam);
	   afx_msg void OnSetFocus(CWnd* pOldWnd);
	   afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
      afx_msg void OnSize(UINT nType, int cx, int cy);
      
      // -------------------- REPRESENTATION ---------------------
   private:
      static CPropertiesWnd*  Instance;

   protected:
      PropertiesToolBar    ToolBar;
	   CMFCPropertyGridCtrl Grid;

      PropertySource*    Source;
   };



NAMESPACE_END2(GUI,Windows)

