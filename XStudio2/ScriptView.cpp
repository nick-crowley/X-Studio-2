
// MFC Test 1View.cpp : implementation of the ScriptView class
//

#include "stdafx.h"
#include "ScriptDocument.h"
#include "ScriptView.h"
#include "PropertiesWnd.h"
#include "Logic/DebugTests.h"
#include "Logic/RtfScriptTextWriter.h"
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
      ON_WM_SETFOCUS()
      ON_COMMAND(ID_EDIT_COPY, &ScriptView::OnClipboardCopy)
      ON_COMMAND(ID_EDIT_CUT, &ScriptView::OnClipboardCut)
      ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, &ScriptView::OnQueryClipboardCut)
      ON_COMMAND(ID_EDIT_PASTE, &ScriptView::OnClipboardPaste)
      ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, &ScriptView::OnQueryClipboardPaste)
   END_MESSAGE_MAP()

   // -------------------------------- CONSTRUCTION --------------------------------
   
   ScriptView::ScriptView() 
             : CFormView(ScriptView::IDD), fnCompileComplete(RichEdit.CompileComplete.Register(this, &ScriptView::OnCompileComplete))
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
   
   /// <summary>Copy text selection to clipboard</summary>
   void ScriptView::OnClipboardCopy()
   {
      RichEdit.Copy();
   }

   /// <summary>Cut text selection to clipboard</summary>
   void ScriptView::OnClipboardCut()
   {
      RichEdit.Cut();
   }

   /// <summary>Paste clipboard contents</summary>
   void ScriptView::OnClipboardPaste()
   {
      if (RichEdit.CanPaste(CF_UNICODETEXT))
         RichEdit.PasteSpecial(CF_UNICODETEXT);
   }

   /// <summary>Display script edit context window</summary>
   /// <param name="pWnd">window clicked</param>
   /// <param name="point">The point in screen co-ordinates</param>
   void ScriptView::OnContextMenu(CWnd* pWnd, CPoint point)
   {
      // Ensure scriptEdit was clicked
      if (pWnd && pWnd->m_hWnd == RichEdit.m_hWnd)
	      theApp.GetContextMenuManager()->ShowPopupMenu(IDM_EDIT_POPUP, point.x, point.y, this, TRUE);
   }

   void ScriptView::OnCompileComplete()
   {
      // Re-Populate variables/scope
      PopulateVariables();
      PopulateScope();
   }

   void ScriptView::OnInitialUpdate()
   {
	   CFormView::OnInitialUpdate();
	   ResizeParentToFit();

      // Convert script to RTF (ansi)
      string txt;
      RtfScriptTextWriter w(txt);
      w.Write(GetDocument()->Script);
      w.Close();

      // Setup RichEdit
      RichEdit.SetBackgroundColor(FALSE, RGB(0,0,0));
      RichEdit.SetEventMask(ENM_CHANGE | ENM_SELCHANGE | ENM_SCROLL | ENM_KEYEVENTS | ENM_MOUSEEVENTS); 

      // Display script text
      RichEdit.SetDocument(GetDocument());
      RichEdit.SetRtf(txt);

      // Populate variables/scope
      PopulateVariables();
      PopulateScope();
   }
   
   /// <summary>Query state of clipboard copy context menu command</summary>
   /// <param name="pCmdUI">UI object</param>
   void ScriptView::OnQueryClipboardCut(CCmdUI *pCmdUI)
   {
      pCmdUI->Enable(RichEdit.HasSelection());
   }

   /// <summary>Query state of clipboard paste context menu command</summary>
   /// <param name="pCmdUI">UI object</param>
   void ScriptView::OnQueryClipboardPaste(CCmdUI *pCmdUI)
   {
      pCmdUI->Enable(RichEdit.CanPaste(CF_UNICODETEXT));
   }

   /// <summary>Invoke context menu</summary>
   /// <param name="nFlags">flags.</param>
   /// <param name="point">point in client co-ords.</param>
   void ScriptView::OnRButtonUp(UINT nFlags, CPoint point)
   {
	   ClientToScreen(&point);
	   OnContextMenu(this, point);
   }


   /// <summary>Set the focus to the script Edit</summary>
   /// <param name="pOldWnd">The old WND.</param>
   void ScriptView::OnSetFocus(CWnd* pOldWnd)
   {
      CFormView::OnSetFocus(pOldWnd);

      RichEdit.SetFocus();
   }

   
   /// <summary>Adjusts the layout</summary>
   /// <param name="nType">Type of resize.</param>
   /// <param name="cx">The width.</param>
   /// <param name="cy">The height.</param>
   void ScriptView::OnSize(UINT nType, int cx, int cy)
   {
      CFormView::OnSize(nType, cx, cy);
      AdjustLayout();
   }

   /// <summary>Called when text selection changes (ie. caret has moved)</summary>
   /// <param name="pNMHDR">Message header</param>
   /// <param name="result">The result.</param>
   void ScriptView::OnSelectionChange(NMHDR* pNMHDR, LRESULT* result)
   {
      //SELCHANGE *pSel = reinterpret_cast<SELCHANGE*>(pNMHDR);
      *result = TRUE;
      
      try
      {
         // Update scope + Raise 'CARET MOVED'
         UpdateScope();
         CaretMoved.Raise(RichEdit.GetCaretLocation());
      }
      catch (ExceptionBase& e) {
         Console.Log(HERE, e);
      }
   }
   
   /// <summary>Populates the variables dropdown</summary>
   void ScriptView::PopulateVariables()
   {
      // Clear
      VariablesCombo.ResetContent();
      VariablesCombo.AddString(L"(Variables)");

      // Populate variables
      for (auto& var : GetDocument()->Script.Variables)
         VariablesCombo.AddString(var.Name.c_str());

      // Select heading
      VariablesCombo.SetCurSel(0);
   }

   /// <summary>Populates the scope labels dropdown</summary>
   void ScriptView::PopulateScope()
   {
      // Clear
      ScopeCombo.ResetContent();
      ScopeCombo.AddString(L"(Global scope)");

      // Populate labels
      for (auto& label : GetDocument()->Script.Labels)
         ScopeCombo.AddString(label.Name.c_str());

      // Select current scope
      UpdateScope();
   }

   /// <summary>Stub</summary>
   /// <param name="cs">The cs.</param>
   /// <returns></returns>
   BOOL ScriptView::PreCreateWindow(CREATESTRUCT& cs)
   {
	   // TODO: Modify the Window class or styles here by modifying
	   //  the CREATESTRUCT cs

	   return CFormView::PreCreateWindow(cs);
   }
   
   /// <summary>Selects the current scope in the labels dropdown</summary>
   void ScriptView::UpdateScope()
   {
      // Set/clear scope
      UINT line = RichEdit.GetCaretLocation().y;
      ScopeCombo.SetCurSel(1+GetDocument()->Script.FindScope(line));
   }

   // ------------------------------- PRIVATE METHODS ------------------------------


NAMESPACE_END(GUI)


