
// MFC Test 1View.cpp : implementation of the ScriptView class
//

#include "stdafx.h"
#include "ScriptDocument.h"
#include "ScriptView.h"
#include "PropertiesWnd.h"
#include "Logic/DebugTests.h"
#include "Logic/RtfScriptWriter.h"
#include "Logic/ScriptParser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/// <summary>User interface</summary>
NAMESPACE_BEGIN(GUI)

   // --------------------------------- STATIC DATA --------------------------------

   CaretMovedEvent   ScriptView::CaretMoved;

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNCREATE(ScriptView, CFormView)

   BEGIN_MESSAGE_MAP(ScriptView, CFormView)
	   ON_WM_CONTEXTMENU()
      ON_NOTIFY(EN_SELCHANGE,IDC_SCRIPT_EDIT,&ScriptView::OnSelectionChange)
	   ON_WM_RBUTTONUP()
      ON_WM_SIZE()
      ON_WM_ACTIVATE()
   END_MESSAGE_MAP()

   // -------------------------------- CONSTRUCTION --------------------------------
   
   ScriptView::ScriptView() : CFormView(ScriptView::IDD)
   {
   }

   ScriptView::~ScriptView()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   #ifdef _DEBUG
   void ScriptView::AssertValid() const
   {
	   CFormView::AssertValid();
   }

   void ScriptView::Dump(CDumpContext& dc) const
   {
	   CFormView::Dump(dc);
   }

   ScriptDocument* ScriptView::GetDocument() const // non-debug version is inline
   {
	   ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(ScriptDocument)));
	   return (ScriptDocument*)m_pDocument;
   }
   #endif //_DEBUG

   // ------------------------------ PROTECTED METHODS -----------------------------
   
   void  ScriptView::AdjustLayout()
   {
      // Destroyed/Minimised
	   if (RichEdit.GetSafeHwnd() == nullptr || (AfxGetMainWnd() != nullptr && AfxGetMainWnd()->IsIconic()))
         return;
         
      CRect view, combo;
      GetClientRect(view);
      ScopeCombo.GetClientRect(combo);

      // Anchor combos at top of view
      ScopeCombo.SetWindowPos(nullptr, 0, 0, view.Width()/2, combo.Height(), SWP_NOZORDER | SWP_NOACTIVATE);
      VariablesCombo.SetWindowPos(nullptr, view.Width()/2, 0, view.Width()/2, combo.Height(), SWP_NOZORDER | SWP_NOACTIVATE);

      // Stretch RichEdit below
      RichEdit.SetWindowPos(nullptr, 0, combo.Height(), view.Width(), view.Height()-combo.Height(), SWP_NOZORDER | SWP_NOACTIVATE);
   }

   void ScriptView::DoDataExchange(CDataExchange* pDX)
   {
      CFormView::DoDataExchange(pDX);
      DDX_Control(pDX, IDC_SCRIPT_EDIT, RichEdit);
      DDX_Control(pDX, IDC_SCOPE_COMBO, ScopeCombo);
      DDX_Control(pDX, IDC_VARIABLES_COMBO, VariablesCombo);
   }

   void ScriptView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
   {
      //Console << L"OnActivateView: bActivate=" << bActivate << (pActivateView==this?L" this":L" another") << ENDL;
      
      // Raise 'DISPLAY PROPERTIES'
      if (bActivate != FALSE)
         CPropertiesWnd::DisplayProperties.Raise(this, PropertyTarget::ScriptView);

      // Handle
      CFormView::OnActivateView(bActivate, pActivateView, pDeactiveView);
   }
   
   void ScriptView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
   {
	   theApp.GetContextMenuManager()->ShowPopupMenu(IDM_EDIT_POPUP, point.x, point.y, this, TRUE);
   }

   void ScriptView::OnInitialUpdate()
   {
	   CFormView::OnInitialUpdate();
	   ResizeParentToFit();

      // Convert script to RTF (ansi)
      string txt;
      RtfScriptWriter w(txt);
      w.Write(GetDocument()->Script);
      w.Close();

      // Setup RichEdit
      RichEdit.SetBackgroundColor(FALSE, RGB(0,0,0));
      RichEdit.SetEventMask(ENM_CHANGE | ENM_SELCHANGE | ENM_SCROLL | ENM_KEYEVENTS | ENM_MOUSEEVENTS); 

      // Display text
      RichEdit.SetRtf(txt);
   }

   void ScriptView::OnRButtonUp(UINT /* nFlags */, CPoint point)
   {
	   ClientToScreen(&point);
	   OnContextMenu(this, point);
   }
   
   void ScriptView::OnSize(UINT nType, int cx, int cy)
   {
      CFormView::OnSize(nType, cx, cy);
      AdjustLayout();
   }

   void ScriptView::OnSelectionChange(NMHDR* pNMHDR, LRESULT* result)
   {
      SELCHANGE *pSel = reinterpret_cast<SELCHANGE*>(pNMHDR);
      
      // Get caret position
      POINT pt = {RichEdit.GetCaretIndex(), 1+RichEdit.LineFromChar(-1)};

      // Raise 'CARET MOVED'
      CaretMoved.Raise(pt);
   }

   BOOL ScriptView::PreCreateWindow(CREATESTRUCT& cs)
   {
	   // TODO: Modify the Window class or styles here by modifying
	   //  the CREATESTRUCT cs

	   return CFormView::PreCreateWindow(cs);
   }

   // ------------------------------- PRIVATE METHODS ------------------------------


NAMESPACE_END(GUI)


