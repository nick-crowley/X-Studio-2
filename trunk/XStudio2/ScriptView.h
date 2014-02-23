#pragma once

#include "afxcmn.h"
#include "Logic/Common.h"
#include "ScriptDocument.h"
#include "ScriptEdit.h"


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
      DECLARE_DYNCREATE(ScriptView)
      DECLARE_MESSAGE_MAP()

   public:
      static CaretMovedEvent   CaretMoved;
   
      // --------------------- PROPERTIES ------------------------
			
      // ---------------------- ACCESSORS ------------------------			
   public:
      bool            FindNext(UINT start, MatchData& m) const;
	   ScriptDocument* GetDocument() const;
      ScriptFile&     GetScript() const;
      CHARRANGE       GetSelection() const;

      // ----------------------- MUTATORS ------------------------
   public:
      bool  Replace(MatchData& m);
      void  SetSelection(CHARRANGE rng);
      BOOL  PreTranslateMessage(MSG* pMsg) override;

   protected:
      void AdjustLayout();
      void PopulateVariables();
      void PopulateScope();
      virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
      virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
      void UpdateScope();
      

      virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
      afx_msg void OnDestroy();
      afx_msg void OnEditComment();
      afx_msg void OnEditIndent();
      afx_msg void OnEditOutdent();
      afx_msg void OnEditUndo();
      afx_msg void OnEditRedo();
      afx_msg void OnClipboardCopy();
      afx_msg void OnClipboardCut();
      afx_msg void OnClipboardPaste();
      afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
      handler void OnCompileComplete();
	   virtual void OnInitialUpdate(); 
      afx_msg void OnQueryClipboardCut(CCmdUI *pCmdUI);
      afx_msg void OnQueryClipboardPaste(CCmdUI *pCmdUI);
      afx_msg void OnQueryEditComment(CCmdUI *pCmdUI);
      afx_msg void OnQueryEditIndent(CCmdUI *pCmdUI);
      afx_msg void OnQueryEditOutdent(CCmdUI *pCmdUI);
      afx_msg void OnQueryEditUndo(CCmdUI *pCmdUI);
      afx_msg void OnQueryEditRedo(CCmdUI *pCmdUI);
      afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
      afx_msg void OnScopeSelectionChange();
      afx_msg void OnSetFocus(CWnd* pOldWnd);
      afx_msg void OnSize(UINT nType, int cx, int cy);	  
      afx_msg void OnTextSelectionChange(NMHDR* pNMHDR, LRESULT* result);

      // -------------------- REPRESENTATION ---------------------
   public:
      ScriptEdit RichEdit;
      CComboBox  ScopeCombo;
      CComboBox  VariablesCombo;

   protected:
      EventHandler fnCompileComplete;

   };


   
NAMESPACE_END2(GUI,Views)

