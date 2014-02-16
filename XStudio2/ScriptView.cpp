
// ScriptView.cpp : implementation of the ScriptView class
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
NAMESPACE_BEGIN2(GUI,Views)

   // --------------------------------- STATIC DATA --------------------------------

   CaretMovedEvent   ScriptView::CaretMoved;

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNCREATE(ScriptView, CFormView)

   BEGIN_MESSAGE_MAP(ScriptView, CFormView)
      ON_CBN_SELCHANGE(IDC_SCOPE_COMBO,&ScriptView::OnScopeSelectionChange)
      ON_NOTIFY(EN_SELCHANGE,IDC_SCRIPT_EDIT,&ScriptView::OnTextSelectionChange)
      ON_WM_ACTIVATE()
      ON_WM_CONTEXTMENU()
	   ON_WM_RBUTTONUP()
      ON_WM_SIZE()
      ON_WM_SETFOCUS()
      ON_COMMAND(ID_EDIT_COMMENT, &ScriptView::OnEditComment)
      ON_COMMAND(ID_EDIT_INDENT, &ScriptView::OnEditIndent)
      ON_COMMAND(ID_EDIT_OUTDENT, &ScriptView::OnEditOutdent)
      ON_COMMAND(ID_EDIT_UNDO, &ScriptView::OnEditUndo)
      ON_COMMAND(ID_EDIT_REDO, &ScriptView::OnEditRedo)
      ON_COMMAND(ID_EDIT_COPY, &ScriptView::OnClipboardCopy)
      ON_COMMAND(ID_EDIT_CUT, &ScriptView::OnClipboardCut)
      ON_COMMAND(ID_EDIT_PASTE, &ScriptView::OnClipboardPaste)
      ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, &ScriptView::OnQueryClipboardCut)
      ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, &ScriptView::OnQueryClipboardPaste)
      ON_UPDATE_COMMAND_UI(ID_EDIT_COMMENT, &ScriptView::OnQueryEditComment)
      ON_UPDATE_COMMAND_UI(ID_EDIT_INDENT, &ScriptView::OnQueryEditIndent)
      ON_UPDATE_COMMAND_UI(ID_EDIT_OUTDENT, &ScriptView::OnQueryEditOutdent)
      ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, &ScriptView::OnQueryEditUndo)
      ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, &ScriptView::OnQueryEditRedo)
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

   /// <summary>Finds and highlights the next match, if any</summary>
   /// <param name="src">search data</param>
   /// <returns>True if found, false otherwise</returns>
   bool  ScriptView::FindNext(SearchData& src)
   {
      // Find next match
      auto pos = RichEdit.GetAllText().find(src.Term, src.LastMatch.cpMax);

      // Found: Return location + Highlight text
      if (pos != wstring::npos)
      {
         src.SetMatch(pos);
         SetSelection(src.LastMatch);
      } 
      else
      {  // Not found: Clear selection
         RichEdit.SetSel(src.LastMatch.cpMin, src.LastMatch.cpMin);
         src.SetMatch(-1);
      }
         
      // Return result
      return pos != wstring::npos;
   }

   /// <summary>Sets the text selection.</summary>
   /// <param name="rng">text range</param>
   void  ScriptView::SetSelection(CHARRANGE rng)
   {
      RichEdit.SetSel(rng);
   }

   /// <summary>Relay mouse events to tooltip</summary>
   /// <param name="pMsg">MSG.</param>
   /// <returns></returns>
   BOOL ScriptView::PreTranslateMessage(MSG* pMsg)
   {
      // Relay mouse messages to tooltip
      if(pMsg->message== WM_LBUTTONDOWN ||
         pMsg->message== WM_LBUTTONUP ||
         pMsg->message== WM_MOUSEMOVE)
      {
         ToolTip.RelayEvent(pMsg);
      }

      return CFormView::PreTranslateMessage(pMsg);
   }

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

   /// <summary>Toggle comment on selected commands</summary>
   void ScriptView::OnEditComment()
   {
      RichEdit.CommentSelection();
   }
   
   /// <summary>Indent selected commands</summary>
   void ScriptView::OnEditIndent()
   {
      if (RichEdit.HasSelection())
         RichEdit.IndentSelection(true);
   }

   /// <summary>Outdent selected commands</summary>
   void ScriptView::OnEditOutdent()
   {
      if (RichEdit.HasSelection())
         RichEdit.IndentSelection(false);
   }

   /// <summary>Undo last edit operation</summary>
   void ScriptView::OnEditUndo()
   {
      RichEdit.Undo();
   }

   /// <summary>Redo last edit operation</summary>
   void ScriptView::OnEditRedo()
   {
      RichEdit.Redo();
   }

   void ScriptView::OnInitialUpdate()
   {
	   CFormView::OnInitialUpdate();
	   ResizeParentToFit();

      // Attach edit to document
      GetDocument()->AttachEdit(RichEdit);

      try
      {
         // Create tooltip
         ToolTip.Create(this);
         ToolTip.AddTool(&RichEdit, L"This is supposed to be the title of the tooltip", CRect(0,0,500,500), 42);
         ToolTip.Activate(TRUE);
         

         CMFCToolTipInfo params;
         params.m_bBoldLabel = TRUE;
         params.m_bDrawDescription = TRUE;
         params.m_bDrawIcon = TRUE;
         params.m_bRoundedCorners = TRUE;
         params.m_bDrawSeparator = TRUE;

         /*params.m_clrFill = RGB(255, 255, 255);
         params.m_clrFillGradient = RGB(228, 228, 240);
         params.m_clrText = RGB(61, 83, 80);
         params.m_clrBorder = RGB(144, 149, 168);*/
         ToolTip.SetParams(&params);
         ToolTip.SetDescription(L"The quick brown fox jumped over the lazy dog.");
         ToolTip.SetFixedWidth(400, 600);
         ToolTip.SetDelayTime(TTDT_INITIAL, 1500);
         ToolTip.SetDelayTime(TTDT_AUTOPOP, 30*1000);
         ToolTip.SetDelayTime(TTDT_RESHOW, 3000);

         // Convert script to RTF (ansi)
         string txt;
         RtfScriptWriter w(txt);
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
      catch (ExceptionBase& e)
      {
         theApp.ShowError(HERE, e, L"Unable to initialise the script view");
      }
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
   
   /// <summary>Query state of COMMENT context menu command</summary>
   /// <param name="pCmdUI">UI object</param>
   void ScriptView::OnQueryEditComment(CCmdUI *pCmdUI)
   {
      pCmdUI->Enable(TRUE);
   }

   /// <summary>Query state of INDENT  context menu command</summary>
   /// <param name="pCmdUI">UI object</param>
   void ScriptView::OnQueryEditIndent(CCmdUI *pCmdUI)
   {
      pCmdUI->Enable(RichEdit.HasSelection());
   }

   /// <summary>Query state of OUTDENT context menu command</summary>
   /// <param name="pCmdUI">UI object</param>
   void ScriptView::OnQueryEditOutdent(CCmdUI *pCmdUI)
   {
      pCmdUI->Enable(RichEdit.HasSelection());
   }

   /// <summary>Query state of UNDO context menu command</summary>
   /// <param name="pCmdUI">UI object</param>
   void ScriptView::OnQueryEditUndo(CCmdUI *pCmdUI)
   {
      pCmdUI->Enable(RichEdit.CanUndo());
   }

   /// <summary>Query state of REDO context menu command</summary>
   /// <param name="pCmdUI">UI object</param>
   void ScriptView::OnQueryEditRedo(CCmdUI *pCmdUI)
   {
      pCmdUI->Enable(RichEdit.CanRedo());
   }

   /// <summary>Invoke context menu</summary>
   /// <param name="nFlags">flags.</param>
   /// <param name="point">point in client co-ords.</param>
   void ScriptView::OnRButtonUp(UINT nFlags, CPoint point)
   {
	   ClientToScreen(&point);
	   OnContextMenu(this, point);
   }

   
   /// <summary>Scrolls to the desired label</summary>
   void ScriptView::OnScopeSelectionChange()
   {
      // Ensure item selected
      if (ScopeCombo.GetCurSel() > 0)
         // Scroll to a couple of lines preceeding the label 
         RichEdit.EnsureVisible( GetDocument()->Script.Labels[ScopeCombo.GetCurSel()-1].LineNumber - 4 );
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
   void ScriptView::OnTextSelectionChange(NMHDR* pNMHDR, LRESULT* result)
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
      wstring label = GetDocument()->Script.FindScope( RichEdit.GetCaretLocation().y );

      if (!label.empty())
         ScopeCombo.SelectString(-1, label.c_str());
      else
         ScopeCombo.SetCurSel(0);
   }

   // ------------------------------- PRIVATE METHODS ------------------------------


NAMESPACE_END2(GUI,Views)

