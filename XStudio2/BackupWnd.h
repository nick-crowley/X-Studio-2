#pragma once
#include "Logic/BackupFile.h"
#include "Logic/Event.h"
#include "ToolBarEx.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)

   // ------------------------------------ ENUMS -----------------------------------

   // ----------------------------- EVENTS AND DELEGATES ---------------------------

   // ----------------------------------- CLASSES ----------------------------------

   /// <summary>Dockable properties window</summary>
   class BackupWnd : public CDockablePane
   {
      // ------------------------ TYPES --------------------------
   public:
      /// <summary>Backup window toolbar</summary>
      class BackupToolBar : public ToolBarEx
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
      BackupWnd();
      virtual ~BackupWnd();
       
      // ------------------------ STATIC -------------------------
   public:
      DECLARE_MESSAGE_MAP()

      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
      
      // ----------------------- MUTATORS ------------------------
   public:
      void  Create(CWnd* parent);
   
   protected:
      void AdjustLayout();
      void Populate(BackupFile& f);
	   void UpdateFont();

	   afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
      handler void OnDocumentSwitched();
      afx_msg void OnPaint();
      afx_msg void OnQueryCommand(CCmdUI* pCmdUI);
      afx_msg void OnQueryCustomCommand(CCmdUI* pCmdUI);
	   afx_msg void OnSetFocus(CWnd* pOldWnd);
	   afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
      afx_msg void OnSize(UINT nType, int cx, int cy);
      
      // -------------------- REPRESENTATION ---------------------
   private:
      static BackupWnd*  Instance;

   protected:
      BackupToolBar  ToolBar;
	   CListBox       List;
      EventHandler   fnDocumentSwitched;
   };



NAMESPACE_END2(GUI,Windows)

