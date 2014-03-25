
// ScriptView.cpp : implementation of the ScriptView class
//

#include "stdafx.h"
#include "ScriptDocument.h"
#include "ScriptView.h"
#include "PropertiesWnd.h"
#include "../Logic/RtfScriptWriter.h"
#include "../Logic/ScriptParser.h"
#include "../Logic/StringStream.h"
#include "../Logic/ScriptFileReader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Views)

   // --------------------------------- STATIC DATA --------------------------------

   /// <summary>Caret movement event</summary>
   CaretMovedEvent   ScriptView::CaretMoved;

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNCREATE(ScriptView, CFormView)

   BEGIN_MESSAGE_MAP(ScriptView, CFormView)
      ON_WM_ACTIVATE()
      ON_WM_CONTEXTMENU()
      ON_WM_DESTROY()
	   ON_WM_RBUTTONUP()
      ON_WM_SIZE()
      ON_WM_SETFOCUS()
      ON_WM_SETTINGCHANGE()
      ON_CBN_SELCHANGE(IDC_SCOPE_COMBO,&ScriptView::OnScopeSelectionChange)
      ON_COMMAND(ID_EDIT_COMMENT, &ScriptView::OnEditComment)
      ON_COMMAND(ID_EDIT_INDENT, &ScriptView::OnEditIndent)
      ON_COMMAND(ID_EDIT_OUTDENT, &ScriptView::OnEditOutdent)
      ON_COMMAND(ID_EDIT_FORMAT_SEL, &ScriptView::OnEditFormatSel)
      ON_COMMAND(ID_EDIT_FORMAT_DOC, &ScriptView::OnEditFormatDoc)
      ON_COMMAND(ID_EDIT_REFACTOR, &ScriptView::OnEditRefactor)
      ON_COMMAND(ID_EDIT_OPEN_SCRIPT, &ScriptView::OnEditOpenScript)
      ON_COMMAND(ID_GAMEDATA_LOOKUP, &ScriptView::OnEditLookupOnline)
      ON_COMMAND(ID_EDIT_GOTO_LABEL, &ScriptView::OnEditGotoLabel)
      ON_COMMAND(ID_EDIT_VIEW_STRING, &ScriptView::OnEditViewString)
      ON_COMMAND(ID_EDIT_UNDO, &ScriptView::OnEditUndo)
      ON_COMMAND(ID_EDIT_REDO, &ScriptView::OnEditRedo)
      ON_COMMAND(ID_EDIT_COPY, &ScriptView::OnClipboardCopy)
      ON_COMMAND(ID_EDIT_CUT, &ScriptView::OnClipboardCut)
      ON_COMMAND(ID_EDIT_PASTE, &ScriptView::OnClipboardPaste)
      ON_NOTIFY(EN_SELCHANGE,IDC_SCRIPT_EDIT,&ScriptView::OnTextSelectionChange)
      ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, &ScriptView::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, &ScriptView::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_EDIT_COMMENT, &ScriptView::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_EDIT_INDENT, &ScriptView::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_EDIT_OUTDENT, &ScriptView::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_EDIT_FORMAT_SEL, &ScriptView::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_EDIT_FORMAT_DOC, &ScriptView::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_EDIT_REFACTOR, &ScriptView::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_EDIT_OPEN_SCRIPT, &ScriptView::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_EDIT_GOTO_LABEL, &ScriptView::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_GAMEDATA_LOOKUP, &ScriptView::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_EDIT_VIEW_STRING, &ScriptView::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, &ScriptView::OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, &ScriptView::OnQueryCommand)
   END_MESSAGE_MAP()

   // -------------------------------- CONSTRUCTION --------------------------------
   
   ScriptView::ScriptView() 
             : CFormView(ScriptView::IDD), 
               fnCompileComplete(RichEdit.CompileComplete.Register(this, &ScriptView::OnCompileComplete)),
               fnTextChanged(RichEdit.TextChanged.Register(this, &ScriptView::OnTextChanged))
   {
   }

   ScriptView::~ScriptView()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   /// <summary>Finds and highlights the next match, if any</summary>
   /// <param name="start">Starting offset</param>
   /// <param name="m">Match data</param>
   /// <returns>True if found, false otherwise</returns>
   bool  ScriptView::FindNext(UINT start, MatchData& m) const
   {
      return RichEdit.FindNext(start, m);
   }
   
   /// <summary>Gets the document.</summary>
   /// <returns></returns>
   ScriptDocument* ScriptView::GetDocument() const
   { 
      return dynamic_cast<ScriptDocument*>(m_pDocument); 
   }

   /// <summary>Gets the script.</summary>
   /// <returns></returns>
   ScriptFile&  ScriptView::GetScript() const
   {
      return GetDocument()->Script;
   }

   /// <summary>Gets the selection.</summary>
   /// <returns></returns>
   CHARRANGE  ScriptView::GetSelection() const
   {
      return RichEdit.GetSelection();
   }

   /// <summary>Replaces the current match</summary>
   /// <param name="m">Match data</param>
   /// <returns>True if replaced, false if match was no longer selected</returns>
   bool  ScriptView::Replace(MatchData& m)
   {
      return RichEdit.Replace(m);
   }
   
   /// <summary>Replaces the contents of the current text selection</summary>
   /// <param name="txt">Text</param>
   void  ScriptView::Replace(const wstring& txt)
   {
      RichEdit.ReplaceSel(txt.c_str(), TRUE);
   }

   /// <summary>Sets the text selection.</summary>
   /// <param name="rng">text range</param>
   void  ScriptView::SetSelection(CHARRANGE rng)
   {
      RichEdit.SetSel(rng);
   }

   /// <summary>No longer used</summary>
   /// <param name="pMsg">MSG.</param>
   /// <returns></returns>
   BOOL ScriptView::PreTranslateMessage(MSG* pMsg)
   {
      return CFormView::PreTranslateMessage(pMsg);
   }

   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Adjusts the layout.</summary>
   void  ScriptView::AdjustLayout()
   {
      // Destroyed/Minimised
	   if (!RichEdit.GetSafeHwnd() || theApp.IsMimized())
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

   /// <summary>Show properties</summary>
   /// <param name="bActivate">whether activating.</param>
   /// <param name="pActivateView">The activate view.</param>
   /// <param name="pDeactiveView">The deactive view.</param>
   void ScriptView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
   {
      // Show properties
      if (bActivate)
         CPropertiesWnd::Connect(GetDocument(), true);


      CFormView::OnActivateView(bActivate, pActivateView, pDeactiveView);
   }

   /// <summary>Display script edit context window</summary>
   /// <param name="pWnd">window clicked</param>
   /// <param name="point">The point in screen co-ordinates</param>
   void ScriptView::OnContextMenu(CWnd* pWnd, CPoint point)
   {
      // Ensure scriptEdit was clicked
      if (pWnd && pWnd->m_hWnd == RichEdit.m_hWnd)
      {
         // Move caret to cursor
         if (!RichEdit.HasSelection())
            RichEdit.SetCaretLocation(CursorPoint(&RichEdit));

         // Popup
	      theApp.GetContextMenuManager()->ShowPopupMenu(IDM_SCRIPTEDIT_POPUP, point.x, point.y, this, TRUE);
      }
   }

   /// <summary>Called when compilation complete.</summary>
   void ScriptView::OnCompileComplete()
   {
      // Re-Populate variables/scope
      PopulateVariables();
      PopulateScope();
   }
   
   /// <summary>Nothing.</summary>
   void ScriptView::OnArgumentChanged()
   {
   }

   /// <summary>Clear properties</summary>
   void ScriptView::OnDestroy()
   {
      // Hide properties info
      CPropertiesWnd::Connect(GetDocument(), false);

      // Destroy
      __super::OnDestroy();
   }
   

   /// <summary>Goto label at the caret</summary>
   void ScriptView::OnEditGotoLabel()
   {
      // Ensure label at caret
      if (!RichEdit.CanGotoLabel())
         return;

      try
      {
         // Parse command at caret
         auto cmd = ScriptParser::Parse(GetDocument()->Script, RichEdit.GetLineText(-1));

         // Lookup and goto label
         ScrollTo( GetDocument()->Script.Labels[cmd.GetLabelName()] );
      }
      // Unable to parse command
      catch (ExceptionBase&) {
      }
   }

   /// <summary>lookup selected command in MSCI reference</summary>
   void ScriptView::OnEditLookupOnline()
   {
      // Ensure command at caret has URL
      if (!RichEdit.CanLookupOnline())
         return;

      try
      {
         // Identify command + launch URL
         auto cmd = ScriptParser::Identify(GetDocument()->Script, RichEdit.GetLineText(-1));
         ShellExecute(*theApp.m_pMainWnd, L"open", cmd.URL.c_str(), nullptr, nullptr, SW_SHOWMAXIMIZED);
      }
      // Unable to identify command
      catch (ExceptionBase&) {
      }
   }
   
   /// <summary>Open script name at the caret</summary>
   void ScriptView::OnEditOpenScript()
   {
      // Ensure script-call at caret
      if (!RichEdit.CanOpenScript())
         return;

      try
      {
         // Parse script-call name from command at caret
         auto cmd = ScriptParser::Parse(GetDocument()->Script, RichEdit.GetLineText(-1));
         auto script = cmd.GetScriptCallName();

         // Not defined by string: Abort 
         if (script.empty())
            return;

         // Check for .PCK/.XML in folder and game-folder
         ScriptCallPath path(GetDocument()->FullPath.Folder, script);

         // Open if found
         if (path.Exists())
            theApp.OpenDocumentFile(path.c_str(), TRUE);
         else
            theApp.ShowMessage(VString(L"Cannot find the script '%s' in the document folder or game folder", script.c_str()));
      }
      // Unable to parse command
      catch (ExceptionBase&) {
      }
   }


   /// <summary>View string at the caret</summary>
   void ScriptView::OnEditViewString()
   {
   }

   /// <summary>Displays script and populates variables/scope combos</summary>
   void ScriptView::OnInitialUpdate()
   {
      // Init
	   __super::OnInitialUpdate();
      ResizeParentToFit();
      
      // Attach edit to document
      GetDocument()->AttachEdit(RichEdit);

      // Set font
      ScopeCombo.SetFont(&theApp.ToolWindowFont);
      VariablesCombo.SetFont(&theApp.ToolWindowFont);

      try
      {
         string txt;

         // Get font size
         int size = PrefsLib.ScriptViewFont.lfHeight;
         if (size < 0)
         {  // Height (Pixels -> points)
            auto dc = GetDC();
            size = MulDiv(-size, 72, dc->GetDeviceCaps(LOGPIXELSY));
            ReleaseDC(dc);
         }

         // Convert script to RTF (ansi)
         RtfScriptWriter w(StreamPtr(new StringStream(txt)), PrefsLib.ScriptViewFont.lfFaceName, size);
         w.Write(GetScript());
         w.Close();

         // Display script text
         RichEdit.Initialize(GetDocument());
         RichEdit.SetRtf(txt);

         // Populate variables/scope
         PopulateVariables();
         PopulateScope();

         // Ensure init hasn't modified document
         GetDocument()->SetModifiedFlag(FALSE);
      }
      catch (ExceptionBase& e)
      {
         theApp.ShowError(HERE, e, L"Unable to initialise the script view");
      }
   }
   
   /// <summary>Perform command</summary>
   /// <param name="nID">ID</param>
   void ScriptView::OnPerformCommand(UINT nID)
   {
      switch (nID)
      {
      // Undo/Redo
      case ID_EDIT_UNDO:   RichEdit.Undo();     break;
      case ID_EDIT_REDO:   RichEdit.Redo();     break;

      // Clipboard
      case ID_EDIT_CUT:    RichEdit.Cut();      break;
      case ID_EDIT_COPY:   RichEdit.Copy();     break;
      case ID_EDIT_PASTE:
         if (RichEdit.CanPaste(CF_UNICODETEXT))
            RichEdit.PasteFormat(CF_UNICODETEXT);
         break;

      // Indent/Outdent
      case ID_EDIT_INDENT:
      case ID_EDIT_OUTDENT:
         if (RichEdit.HasSelection())
            RichEdit.IndentSelection(nID == ID_EDIT_INDENT);
         break;

      // Format:
      case ID_EDIT_FORMAT_SEL: RichEdit.FormatSelection();  break;
      case ID_EDIT_FORMAT_DOC: RichEdit.FormatDocument();   break;

      // Comment:
      case ID_EDIT_COMMENT:   RichEdit.CommentSelection();  break;

      // Refactor:
      case ID_EDIT_REFACTOR:  RichEdit.Refactor();          break;

      // Goto Label
      /*case ID_EDIT_GOTO_LABEL:
      case ID_EDIT_VIEW_STRING:
      case ID_EDIT_OPEN_SCRIPT:*/
      }
   }

   /// <summary>Query state of clipboard copy context menu command</summary>
   /// <param name="pCmdUI">UI object</param>
   void ScriptView::OnQueryCommand(CCmdUI *pCmdUI)
   {
      BOOL state = TRUE, 
           check = FALSE;

      //
      switch (pCmdUI->m_nID)
      {
      // Clipboard
      case ID_EDIT_CUT:     
      case ID_EDIT_COPY:        state = RichEdit.HasSelection();            break;
      case ID_EDIT_PASTE:       state = RichEdit.CanPaste(CF_UNICODETEXT);  break;

      // Commment
      case ID_EDIT_COMMENT:     state = TRUE;                            break;

      // Code
      case ID_EDIT_REFACTOR:    state = RichEdit.CanRefactor();          break;
      case ID_EDIT_OPEN_SCRIPT: state = RichEdit.CanOpenScript();        break;
      case ID_EDIT_GOTO_LABEL:  state = RichEdit.CanGotoLabel();         break;
      case ID_EDIT_VIEW_STRING: state = RichEdit.CanViewString();        break;
      case ID_GAMEDATA_LOOKUP:  state = RichEdit.CanLookupOnline();      break;

      // Indent/Outdent
      case ID_EDIT_INDENT:
      case ID_EDIT_OUTDENT:     state = RichEdit.HasSelection();         break;

      // Format 
      case ID_EDIT_FORMAT_SEL:  state = RichEdit.HasSelection();         break;
      case ID_EDIT_FORMAT_DOC:  state = TRUE;                            break;  

      // Undo/Redo
      case ID_EDIT_UNDO:        state = RichEdit.CanUndo();
                                pCmdUI->SetText(RichEdit.GetUndoMenuItem().c_str());    break;

      case ID_EDIT_REDO:        state = RichEdit.CanRedo();
                                pCmdUI->SetText(RichEdit.GetRedoMenuItem().c_str());    break;
      }

      // Set state
      pCmdUI->SetCheck(check);
      pCmdUI->Enable(state);
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
         // Lookup and Scroll to label
         ScrollTo(GetScript().Labels[ScopeCombo.GetCurSel()-1]);
   }

   /// <summary>Updates combobox fonts.</summary>
   /// <param name="uFlags">The flags.</param>
   /// <param name="lpszSection">The section.</param>
   void ScriptView::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
   {
      // Set font
      ScopeCombo.SetFont(&theApp.ToolWindowFont);
      VariablesCombo.SetFont(&theApp.ToolWindowFont);

      // Adjust layout
      AdjustLayout();
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
   
   /// <summary>Modify document when text changes.</summary>
   void ScriptView::OnTextChanged()
   {
      GetDocument()->SetModifiedFlag(TRUE);
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
   
   /// <summary>Scroll edit to a script label</summary>
   /// <param name="l">label.</param>
   void ScriptView::ScrollTo(const ScriptLabel& l)
   {
      // Scroll to a couple of lines preceeding the label 
      RichEdit.EnsureVisible( l.LineNumber - 4 );
   }

   /// <summary>Populates the variables dropdown</summary>
   void ScriptView::PopulateVariables()
   {
      // Clear
      VariablesCombo.ResetContent();
      VariablesCombo.AddString(L"(Variables)");

      // Populate variables
      for (auto& var : GetScript().Variables)
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
      for (auto& label : GetScript().Labels)
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
      wstring label = GetScript().FindScope( RichEdit.GetCaretLocation().y );

      if (!label.empty())
         ScopeCombo.SelectString(-1, label.c_str());
      else
         ScopeCombo.SetCurSel(0);
   }

   // ------------------------------- PRIVATE METHODS ------------------------------


NAMESPACE_END2(GUI,Views)

