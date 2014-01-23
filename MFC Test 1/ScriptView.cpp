
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

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNCREATE(ScriptView, CFormView)

   BEGIN_MESSAGE_MAP(ScriptView, CFormView)
	   ON_WM_CONTEXTMENU()
	   ON_WM_RBUTTONUP()
      ON_WM_SIZE()
      ON_COMMAND(ID_TEST_RUN_ALL, &ScriptView::OnRuntests)
      ON_COMMAND(ID_TEST_COMPILE, &ScriptView::OnCompile)
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
   

   void ScriptView::OnCompile()
   {
      LineArray lines;
   
      try
      {
         // Get text in lines
         for (int i = 0; i < RichEdit.GetLineCount(); i++)
            lines.push_back(RichEdit.GetLine(i));

         // Test parser
         auto tree = DebugTests::CompileScript(lines);

         // Test error markup
         for (const auto& err : tree.GetErrors())
         {
            CHARFORMAT2 cf;
            cf.cbSize = sizeof(cf);
            cf.dwMask = CFM_UNDERLINE | CFM_UNDERLINETYPE;
            cf.dwEffects = CFE_UNDERLINE;
            cf.bUnderlineType = CFU_UNDERLINEWAVE;

            BYTE* colour = &cf.bRevAuthor + 1;
            //cf.bUnderlineColor = 0x01;
            *colour = 0x02;

            // Select error
            UINT start = RichEdit.LineIndex(err.Line-1);
            RichEdit.SetSel(start+err.Start, start+err.End);

            // Underline
            RichEdit.SetSelectionCharFormat(cf);
         }
      }
      catch (ExceptionBase&  e)
      {
         CString sz;
         sz.Format(L"Unable to parse script : %s\n\n" L"Source: %s()", e.Message.c_str(), e.Source.c_str());
         AfxMessageBox(sz);
      }
   }

  
   
   void ScriptView::OnRuntests()
   {
      DebugTests::RunAll();
      AfxMessageBox(L"Tests complete");
   }


   void ScriptView::OnInitialUpdate()
   {
	   CFormView::OnInitialUpdate();
	   ResizeParentToFit();

      // Enable EN_CHANGE
      //RichEdit.SetEventMask(RichEdit.GetEventMask() | ENM_UPDATE | ENM_CHANGE);

      // TEST: set paragraph
      //PARAFORMAT2 pf;
      //pf.cbSize = sizeof(pf);
      //pf.dwMask = PFM_LINESPACING;
      //pf.bLineSpacingRule = 0; // Single spaced
      //pf.dyLineSpacing = 20;
      //
      //RichEdit.SetSel(0,-1);
      //RichEdit.SetParaFormat(pf);

      // Set background colour
      RichEdit.SetBackgroundColor(FALSE, RGB(0,0,0));
      RichEdit.SetEventMask(RichEdit.GetEventMask() | ENM_UPDATE | ENM_CHANGE);

      // Convert script to RTF (ansi)
      string txt;
      RtfScriptWriter w(txt);
      w.Write(GetDocument()->Script);
      w.Close();

      // Display text
      //SETTEXTEX opt = {ST_DEFAULT, CP_ACP};
      //RichEdit.SetBackgroundColor(FALSE, RGB(0,0,0));
      //RichEdit.SendMessage(EM_SETTEXTEX, (WPARAM)&opt, (LPARAM)txt.c_str());
      RichEdit.SetRtf(txt);
   }

   void ScriptView::OnRButtonUp(UINT /* nFlags */, CPoint point)
   {
	   ClientToScreen(&point);
	   OnContextMenu(this, point);
   }

   void ScriptView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
   {
	   theApp.GetContextMenuManager()->ShowPopupMenu(IDM_EDIT_POPUP, point.x, point.y, this, TRUE);
   }

   void ScriptView::OnSize(UINT nType, int cx, int cy)
   {
      CFormView::OnSize(nType, cx, cy);
      AdjustLayout();
   }

   BOOL ScriptView::PreCreateWindow(CREATESTRUCT& cs)
   {
	   // TODO: Modify the Window class or styles here by modifying
	   //  the CREATESTRUCT cs

	   return CFormView::PreCreateWindow(cs);
   }

   // ------------------------------- PRIVATE METHODS ------------------------------


NAMESPACE_END(GUI)


