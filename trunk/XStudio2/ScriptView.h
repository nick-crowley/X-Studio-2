
// MFC Test 1View.h : interface of the ScriptView class
//

#pragma once

#include "afxcmn.h"
#include "Logic/Common.h"
#include "ScriptDocument.h"
#include "ScriptEdit.h"
//#include "afxwin.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Views)

   // ------------------ EVENTS AND DELEGATES -----------------

   typedef Event<POINT>                  CaretMovedEvent;
   typedef CaretMovedEvent::DelegatePtr  CaretMovedHandler;

   // ------------------------ CLASSES ------------------------

   /// <summary>Script view</summary>
   class ScriptView : public CFormView
   {
      // ------------------------ TYPES --------------------------
   public:
	   enum{ IDD = IDR_SCRIPTVIEW };
      
      // --------------------- CONSTRUCTION ----------------------
   protected: 
	   ScriptView();
	public:
      virtual ~ScriptView();   
	  
      // ------------------------ STATIC -------------------------
   public:
      static CaretMovedEvent   CaretMoved;
   
      DECLARE_DYNCREATE(ScriptView)
      DECLARE_MESSAGE_MAP()
   
      // --------------------- PROPERTIES ------------------------
			
      // ---------------------- ACCESSORS ------------------------			
   public:
   #ifdef _DEBUG
	   virtual void AssertValid() const;
	   virtual void Dump(CDumpContext& dc) const;
   #endif

	   ScriptDocument* GetDocument() const;

      // ----------------------- MUTATORS ------------------------

   protected:
      void AdjustLayout();
      void PopulateVariables();
      void PopulateScope();
      virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
      virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
      void UpdateScope();
      

      virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
      afx_msg void OnClipboardCopy();
      afx_msg void OnClipboardCut();
      afx_msg void OnClipboardPaste();
      afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
      handler void OnCompileComplete();
	   virtual void OnInitialUpdate(); 
      afx_msg void OnQueryClipboardCut(CCmdUI *pCmdUI);
      afx_msg void OnQueryClipboardPaste(CCmdUI *pCmdUI);
      afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
      afx_msg void OnSelectionChange(NMHDR* pNMHDR, LRESULT* result);
      afx_msg void OnSetFocus(CWnd* pOldWnd);
      afx_msg void OnSize(UINT nType, int cx, int cy);	  

      // -------------------- REPRESENTATION ---------------------
   public:
      ScriptEdit RichEdit;
      CComboBox  ScopeCombo;
      CComboBox  VariablesCombo;

   protected:
      EventHandler fnCompileComplete;
   };

   #ifndef _DEBUG  
   inline ScriptDocument* ScriptView::GetDocument() const
      { return reinterpret_cast<ScriptDocument*>(m_pDocument); }
   #endif

   
NAMESPACE_END2(GUI,Views)

