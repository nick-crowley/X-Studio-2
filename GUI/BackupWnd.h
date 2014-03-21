#pragma once
#include "../Logic/BackupFile.h"
#include "../Logic/Event.h"
#include "ToolBarEx.h"
#include "ImageListEx.h"
#include "ProjectDocument.h"
#include "BackupList.h"

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

   protected:

      // --------------------- CONSTRUCTION ----------------------
   public:
      BackupWnd();
      virtual ~BackupWnd();
       
      // ------------------------ STATIC -------------------------
   public:
      DECLARE_MESSAGE_MAP()

      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
   protected:
      bool HasFocus() const;

      // ----------------------- MUTATORS ------------------------
   public:
      void  Create(CWnd* parent);
   
   protected:
      void AdjustLayout();
      void DrawItem(CDC& dc, CRect& rc, int index, UINT state, bool calculate);
      void Populate();
      void Refresh();
	   void UpdateFont();

      handler void OnBackupChanged(ProjectItem* item);
	   afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
      afx_msg void OnCommandDelete()     { OnPerformCommand(ID_BACKUP_DELETE); }
      afx_msg void OnCommandDiff()       { OnPerformCommand(ID_BACKUP_DIFF);   }
      afx_msg void OnCommandRevert()     { OnPerformCommand(ID_BACKUP_REVERT); }
      afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
      afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
      handler void OnDocumentSwitched();
      afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
      afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
      afx_msg void OnPaint();
      handler void OnProjectClosed();
      handler void OnProjectLoaded();
      afx_msg void OnQueryCommand(CCmdUI* pCmdUI);
      afx_msg void OnPerformCommand(UINT nID);
	   afx_msg void OnSetFocus(CWnd* pOldWnd);
	   afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
      afx_msg void OnSize(UINT nType, int cx, int cy);
      
      // -------------------- REPRESENTATION ---------------------
   private:
      static BackupWnd*  Instance;

   protected:
      ImageListEx    Images;
      BackupToolBar  ToolBar;
      BackupFile     Backup;
	   CListBox       List;
      CFont          BoldFont;

      ProjectItemHandler fnBackupChanged;
      EventHandler       fnDocumentSwitched,
                         fnProjectClosed,
                         fnProjectLoaded;
   };



NAMESPACE_END2(GUI,Windows)

