
#pragma once
#include "Logic/Event.h"
#include "ToolBarEx.h"
#include "PropertyGrid.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)

   // ------------------------------------ ENUMS -----------------------------------

   // ----------------------------- EVENTS AND DELEGATES ---------------------------

   // ----------------------------------- CLASSES ----------------------------------

   /// <summary>Properties window toolbar</summary>
   class CPropertiesToolBar : public ToolBarEx
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
      /// <summary>Called to populate properties.</summary>
      /// <param name="p">The p.</param>
      virtual void  OnDisplayProperties(CMFCPropertyGridCtrl& grid) PURE;

      /// <summary>Called when property updated.</summary>
      /// <param name="p">The p.</param>
      virtual bool  OnPropertyUpdated(CMFCPropertyGridProperty* p)
      {
         return true;
      }

      /// <summary>Called when property needs validating.</summary>
      /// <param name="p">The p.</param>
      /// <returns></returns>
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
      //void InitPropList();
	   void SetPropListFont();
      void SetVSDotNetLook(BOOL bSet);

	   afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	   afx_msg void OnCommand1();
	   afx_msg void OnCommand2();
      afx_msg void OnExpandAllProperties();
      afx_msg void OnQueryCommand1(CCmdUI* pCmdUI);
	   afx_msg void OnQueryCommand2(CCmdUI* pCmdUI);
      afx_msg void OnQueryExpandAllProperties(CCmdUI* pCmdUI);
      afx_msg void OnQuerySortProperties(CCmdUI* pCmdUI);
      afx_msg LRESULT OnPropertyUpdated(WPARAM wParam, LPARAM lParam);
	   afx_msg void OnSetFocus(CWnd* pOldWnd);
	   afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
      afx_msg void OnSize(UINT nType, int cx, int cy);
      afx_msg void OnSortProperties();

	   DECLARE_MESSAGE_MAP()

      // -------------------- REPRESENTATION ---------------------
   protected:
      CFont                m_fntPropList;
	   CPropertiesToolBar   m_wndToolBar;
	   PropertyGrid         m_wndPropList;

      PropertySource*    Source;
   };



NAMESPACE_END2(GUI,Windows)

