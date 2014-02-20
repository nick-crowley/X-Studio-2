
#pragma once
#include "Logic/Event.h"
#include "ToolBarEx.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)

   // ------------------------------------ ENUMS -----------------------------------

   // ----------------------------- EVENTS AND DELEGATES ---------------------------

   // ----------------------------------- CLASSES ----------------------------------

   /// <summary>Properties window toolbar</summary>
   class CPropertiesToolBar : public CMFCToolBar
   {
      // --------------------- CONSTRUCTION ----------------------

      // ---------------------- ACCESSORS ------------------------			
      
      // ----------------------- MUTATORS ------------------------
   public:
	   virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	   {
		   CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	   }

	   virtual BOOL AllowShowOnList() const { return FALSE; }
   };


   /// <summary>Defines a source of Properties</summary>
   class PropertySource
   {
   public:
      virtual void  OnDisplayProperties(CMFCPropertyGridCtrl& grid) PURE;

      virtual void  OnPropertyUpdated(CMFCPropertyGridProperty* p)
      {}

      virtual bool  OnValidateProperty(CMFCPropertyGridProperty* p)
      {
         return true;
      }
   };


   /// <summary>Dockable properties window</summary>
   class CPropertiesWnd : public CDockablePane
   {
      // ------------------------ TYPES --------------------------
   public:
      
      // --------------------- CONSTRUCTION ----------------------
   public:
      CPropertiesWnd();
      virtual ~CPropertiesWnd();
       
      // ------------------------ STATIC -------------------------
   private:
      static CPropertiesWnd*  Instance;

   public:
      static void  Connect(PropertySource* src, bool connect);

      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
      
      // ----------------------- MUTATORS ------------------------
   protected:
      void AdjustLayout();
      void ConnectSource(PropertySource* src, bool connect);
      void InitPropList();
	   void SetPropListFont();
      void SetVSDotNetLook(BOOL bSet);

	   afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	   afx_msg void OnSize(UINT nType, int cx, int cy);
	   afx_msg void OnExpandAllProperties();
	   afx_msg void OnUpdateExpandAllProperties(CCmdUI* pCmdUI);
	   afx_msg void OnSortProperties();
	   afx_msg void OnUpdateSortProperties(CCmdUI* pCmdUI);
	   afx_msg void OnProperties1();
	   afx_msg void OnUpdateProperties1(CCmdUI* pCmdUI);
	   afx_msg void OnProperties2();
	   afx_msg void OnUpdateProperties2(CCmdUI* pCmdUI);
	   afx_msg void OnSetFocus(CWnd* pOldWnd);
	   afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);

	   DECLARE_MESSAGE_MAP()

      // -------------------- REPRESENTATION ---------------------
   protected:
      CFont                m_fntPropList;
	   CPropertiesToolBar   m_wndToolBar;
	   CMFCPropertyGridCtrl m_wndPropList;

      PropertySource*    Source;
   };



NAMESPACE_END2(GUI,Windows)

