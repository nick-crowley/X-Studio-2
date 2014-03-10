#pragma once

#include "afxcmn.h"
#include "Logic/Common.h"
#include "DiffDocument.h"
#include "ScriptEdit.h"


/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Views)

   // ------------------ EVENTS AND DELEGATES -----------------

   // ------------------------ CLASSES ------------------------

   /// <summary>Script difference view</summary>
   class DiffView : public CFormView
   {
      // ------------------------ TYPES --------------------------
   public:
	   enum{ IDD = IDR_DIFFVIEW };

      // --------------------- CONSTRUCTION ----------------------
   protected: 
	   DiffView();
	public:
      virtual ~DiffView();   
	  
      // ------------------------ STATIC -------------------------
      DECLARE_DYNCREATE(DiffView)
      DECLARE_MESSAGE_MAP()
   
      // --------------------- PROPERTIES ------------------------
			
      // ---------------------- ACCESSORS ------------------------			
   public:
	   DiffDocument*  GetDocument() const;
      //ScriptFile&    GetScript() const;

      // ----------------------- MUTATORS ------------------------
   public:

   protected:
      void AdjustLayout();
      virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
      

      virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
      afx_msg void OnDestroy();
      afx_msg void OnClipboardCopy();
      afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	   virtual void OnInitialUpdate(); 
      afx_msg void OnQueryCommand(CCmdUI *pCmdUI);
      afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
      afx_msg void OnSetFocus(CWnd* pOldWnd);
      afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
      afx_msg void OnSize(UINT nType, int cx, int cy);	  

      // -------------------- REPRESENTATION ---------------------
   public:
      ScriptEdit RichEdit;
   };


   
NAMESPACE_END2(GUI,Views)

