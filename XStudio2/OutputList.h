
#pragma once
#include "Logic/WorkerFeedback.h"

/// <summary>User interface controls</summary>
NAMESPACE_BEGIN2(GUI,Controls)

   
   /// <summary>Output window list</summary>
   class OutputList : public CListCtrl
   {
      // ------------------------ TYPES --------------------------
   private:
	  
      // --------------------- CONSTRUCTION ----------------------
      
   public:
      OutputList();
	   virtual ~OutputList();

      // ------------------------ STATIC -------------------------
   protected:
	   DECLARE_MESSAGE_MAP()

      // ----------------------- MUTATORS ------------------------
   public:
      void  Create(CWnd* parent, DWORD style, UINT nID, CImageList* images);
      void  InsertItem(const WorkerProgress& wp);

   protected:
      afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	   afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	   afx_msg void OnEditCopy();
	   afx_msg void OnEditClear();
      afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
      afx_msg void OnSize(UINT nType, int cx, int cy);
	   afx_msg void OnViewOutput();
   };


NAMESPACE_END2(GUI,Controls)

