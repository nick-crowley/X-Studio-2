
#pragma once
#include "Logic/WorkerFeedback.h"

/// <summary>User interface controls</summary>
NAMESPACE_BEGIN2(GUI,Controls)

   
   /// <summary>Backup window list</summary>
   /// <remarks>This class isn't used</remarks>
   class BackupList : public CListBox
   {
      // ------------------------ TYPES --------------------------
   private:
	  
      // --------------------- CONSTRUCTION ----------------------
      
   public:
      BackupList();
	   virtual ~BackupList();

      // ------------------------ STATIC -------------------------
   protected:
	   DECLARE_MESSAGE_MAP()

      // ----------------------- MUTATORS ------------------------
   public:

   protected:
      afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	   //afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
      afx_msg BOOL OnEraseBkgnd(CDC* pDC);
      afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
      
   };


NAMESPACE_END2(GUI,Controls)

