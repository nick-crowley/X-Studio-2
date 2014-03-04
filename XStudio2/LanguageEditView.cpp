// LanguageEditView.cpp : implementation file
//

#include "stdafx.h"
#include "LanguageEditView.h"
#include "LanguageStringView.h"
#include "PropertiesWnd.h"
#include "Helpers.h"
#include "afxcview.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Views)

   // --------------------------------- APP WIZARD ---------------------------------
  
   // -------------------------------- CONSTRUCTION --------------------------------

   LanguageEditView::LanguageEditView() : CFormView(LanguageEditView::IDD)
   {
   }

   LanguageEditView::~LanguageEditView()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   IMPLEMENT_DYNCREATE(LanguageEditView, CFormView)

   BEGIN_MESSAGE_MAP(LanguageEditView, CFormView)
      ON_WM_SIZE()
      ON_WM_CREATE()
      ON_WM_SETTINGCHANGE()
      ON_COMMAND(ID_EDIT_CUT, &LanguageEditView::OnCommandEditCut)
      ON_COMMAND(ID_EDIT_COPY, &LanguageEditView::OnCommandEditCopy)
      ON_COMMAND(ID_EDIT_PASTE, &LanguageEditView::OnCommandEditPaste)
      ON_COMMAND(ID_EDIT_CLEAR, &LanguageEditView::OnCommandEditClear)
      ON_COMMAND(ID_EDIT_COLOUR, &LanguageEditView::OnCommandEditColour)
      ON_COMMAND(ID_EDIT_SELECT_ALL, &LanguageEditView::OnCommandEditSelectAll)
      ON_COMMAND(ID_EDIT_ADD_BUTTON, &LanguageEditView::OnCommandEditAddButton)
      ON_COMMAND_RANGE(ID_EDIT_UNDO, ID_EDIT_REDO, &LanguageEditView::OnPerformCommand)
      ON_COMMAND_RANGE(ID_EDIT_BOLD, ID_EDIT_UNDERLINE, &LanguageEditView::OnPerformCommand)
      ON_COMMAND_RANGE(ID_EDIT_LEFT, ID_EDIT_JUSTIFY, &LanguageEditView::OnPerformCommand)
      ON_COMMAND_RANGE(ID_VIEW_SOURCE, ID_VIEW_DISPLAY, &LanguageEditView::OnPerformCommand)
      ON_NOTIFY(EN_SELCHANGE,IDC_STRING_EDIT,&LanguageEditView::OnTextSelectionChange)
      ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, &LanguageEditView::OnQueryClipboard)
      ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, &LanguageEditView::OnQueryClipboard)
      ON_UPDATE_COMMAND_UI(ID_EDIT_FIND, &LanguageEditView::OnQueryClipboard)
      ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, &LanguageEditView::OnQueryClipboard)
      ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, &LanguageEditView::OnQueryClipboard)
      ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, &LanguageEditView::OnQueryClipboard)
      ON_UPDATE_COMMAND_UI(ID_EDIT_COLOUR, &LanguageEditView::OnQueryFormat)
      ON_UPDATE_COMMAND_UI(ID_EDIT_ADD_BUTTON, &LanguageEditView::OnQueryFormat)
      ON_UPDATE_COMMAND_UI_RANGE(ID_EDIT_UNDO, ID_EDIT_REDO, &LanguageEditView::OnQueryClipboard)
      ON_UPDATE_COMMAND_UI_RANGE(ID_EDIT_BOLD, ID_EDIT_UNDERLINE, &LanguageEditView::OnQueryFormat)
      ON_UPDATE_COMMAND_UI_RANGE(ID_EDIT_LEFT, ID_EDIT_JUSTIFY, &LanguageEditView::OnQueryAlignment)
      ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_SOURCE, ID_VIEW_DISPLAY, &LanguageEditView::OnQueryMode)
   END_MESSAGE_MAP()
   
   // ------------------------------- PUBLIC METHODS -------------------------------

   /// <summary>Gets the language document</summary>
   LanguageDocument* LanguageEditView::GetDocument() const 
   {
#ifdef _DEBUG
	   return dynamic_cast<LanguageDocument*>(m_pDocument);
#else
      return reinterpret_cast<LanguageDocument*>(m_pDocument);
#endif
   }

   /// <summary>Gets the edit control</summary>
   LanguageEdit& LanguageEditView::GetEdit() const 
   {
      return const_cast<LanguageEdit&>(RichEdit);
   }

   /// <summary>Called when activate view.</summary>
   /// <param name="bActivate">activated.</param>
   /// <param name="pActivateView">The activate view.</param>
   /// <param name="pDeactiveView">The deactive view.</param>
   void LanguageEditView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
   {
      // Show properties
      if (bActivate)
         CPropertiesWnd::Connect(GetDocument(), true);  

      CFormView::OnActivateView(bActivate, pActivateView, pDeactiveView);
   }

   /// <summary>Query state of editor mode command</summary>
   void LanguageEditView::OnQueryMode(CCmdUI* pCmd)
   {
      bool state = true, 
           checked = false;

      // Query mode
      switch (pCmd->m_nID)
      {
      case ID_VIEW_SOURCE:     checked = (GetDocument()->CurrentMode == EditMode::Source);   break;
      case ID_VIEW_EDITOR:     checked = (GetDocument()->CurrentMode == EditMode::Edit);     break;
      case ID_VIEW_DISPLAY:    checked = (GetDocument()->CurrentMode == EditMode::Display);  break;
      }

      // Set state
      pCmd->Enable(state ? TRUE : FALSE);
      pCmd->SetCheck(checked ? TRUE : FALSE);
   }

   // ------------------------------ PROTECTED METHODS -----------------------------

   /// <summary>Arrange controls</summary>
   void  LanguageEditView::AdjustLayout()
   {
      // Destroyed/Minimised
	   if (RichEdit.GetSafeHwnd() == nullptr || (AfxGetMainWnd() != nullptr && AfxGetMainWnd()->IsIconic()))
         return;
         
      ClientRect view(this);

      // Anchor toolbar to top
      int barHeight = ToolBar.CalcFixedLayout(FALSE, TRUE).cy;
	   ToolBar.SetWindowPos(nullptr, view.left, view.top, view.Width(), barHeight, SWP_NOACTIVATE | SWP_NOZORDER);

      // Stretch RichEdit over remainder
      RichEdit.SetWindowPos(nullptr, view.left, view.top+barHeight, view.Width(), view.Height()-barHeight, SWP_NOZORDER | SWP_NOACTIVATE);
   }

   /// <summary>Does the data exchange.</summary>
   /// <param name="pDX">The p dx.</param>
   void LanguageEditView::DoDataExchange(CDataExchange* pDX)
   {
      CFormView::DoDataExchange(pDX);
      DDX_Control(pDX, IDC_STRING_EDIT, RichEdit);
   }
   
   
   /// <summary>Creates the toolbar</summary>
   /// <param name="lpCreateStruct">The create structure.</param>
   /// <returns></returns>
   int LanguageEditView::OnCreate(LPCREATESTRUCT lpCreateStruct)
   {
      try
      {
         // Create view
         if (CFormView::OnCreate(lpCreateStruct) == -1)
            throw Win32Exception(HERE, L"Unable to base view");

         // Create toolbar
         if (!ToolBar.Create(this, IDR_EDITOR, L"Editor", true, IDB_EDITOR_GREY))  
            throw Win32Exception(HERE, L"Unable to create toolbar");

         return 0;
      }
      catch (ExceptionBase& e) {
         Console.Log(HERE, e);
         return -1;
      }
   }

   
   /// <summary>Initialise control</summary>
   void LanguageEditView::OnInitialUpdate()
   {
      CFormView::OnInitialUpdate();

      try
      {
         // Listen for string Selection Changed
         fnStringSelectionChanged = GetDocument()->StringSelectionChanged.Register(this, &LanguageEditView::OnStringSelectionChanged);
      
         // Init RichEdit
         RichEdit.Initialize(GetDocument());
      }
      catch (ExceptionBase& e) {
         Console.Log(HERE, e);
      }
   }
   /// <summary>Perform text formatting command.</summary>
   /// <param name="nID">The command identifier.</param>
   void LanguageEditView::OnPerformCommand(UINT nID)
   {
      static int LastButtonID = 1;  
      
      //AfxMessageBox(L"LanguageEditView::OnCommandChangeText");

      try
      {
         // Execute
         switch (nID)
         {
         // Clipboard: Set based on selection
         case ID_EDIT_CUT:        RichEdit.Cut();     break;
         case ID_EDIT_COPY:       RichEdit.Copy();    break;
         case ID_EDIT_PASTE:      RichEdit.PasteFormat(CF_UNICODETEXT);   break;
         case ID_EDIT_CLEAR:      RichEdit.ReplaceSel(L"", TRUE);         break;
         case ID_EDIT_SELECT_ALL: RichEdit.SetSel(0, -1);                 break;

         // Undo/Redo:
         case ID_EDIT_UNDO:       RichEdit.Undo();    break;
         case ID_EDIT_REDO:       RichEdit.Redo();    break;

         // Button: Insert at caret
         case ID_EDIT_ADD_BUTTON: RichEdit.InsertButton(GuiString(L"Button %d", LastButtonID++), L"id");  break;

         // Colour: Not implemented
         case ID_EDIT_COLOUR: 
            break;

         // Format: Require editor mode
         case ID_EDIT_BOLD:       RichEdit.ToggleFormatting(CFE_BOLD);       break;
         case ID_EDIT_ITALIC:     RichEdit.ToggleFormatting(CFE_ITALIC);     break;
         case ID_EDIT_UNDERLINE:  RichEdit.ToggleFormatting(CFE_UNDERLINE);  break;

         // Alignment: Require editor mode
         case ID_EDIT_LEFT:       RichEdit.SetParaFormat(ParaFormat(PFM_ALIGNMENT, Alignment::Left));     break;
         case ID_EDIT_RIGHT:      RichEdit.SetParaFormat(ParaFormat(PFM_ALIGNMENT, Alignment::Right));    break;
         case ID_EDIT_CENTRE:     RichEdit.SetParaFormat(ParaFormat(PFM_ALIGNMENT, Alignment::Centre));   break;
         case ID_EDIT_JUSTIFY:    RichEdit.SetParaFormat(ParaFormat(PFM_ALIGNMENT, Alignment::Justify));  break;

         // Mode: Attempt Change mode  (May fail if syntax errors)
         case ID_VIEW_SOURCE:     GetDocument()->CurrentMode = EditMode::Source;   break;
         case ID_VIEW_EDITOR:     GetDocument()->CurrentMode = EditMode::Edit;     break;
         case ID_VIEW_DISPLAY:    GetDocument()->CurrentMode = EditMode::Display;  break;
         }
      }
      catch (ExceptionBase& e) {
         theApp.ShowError(HERE, ApplicationException(e));
      }
   }

   
   /// <summary>Query state of paragraph alignment command</summary>
   void LanguageEditView::OnQueryAlignment(CCmdUI* pCmd)
   {
      bool state = false, 
           checked = false;

      // Query relevant states
      bool selection = GetDocument()->SelectedString != nullptr,
           library   = GetDocument()->Virtual,
           editMode  = GetDocument()->CurrentMode == EditMode::Edit,
           hasFocus  = ::GetFocus() == RichEdit;

      // Require: string, edit mode, non-library document, input-focus
      if (state = selection && !library && editMode && hasFocus)
      {
         ParaFormat  pf(PFM_ALIGNMENT);
         
         // Query alignment
         RichEdit.GetParaFormat(pf);
         switch (pCmd->m_nID)
         {
         case ID_EDIT_LEFT:     checked = (pf.wAlignment == PFA_LEFT);     break;
         case ID_EDIT_RIGHT:    checked = (pf.wAlignment == PFA_RIGHT);    break;
         case ID_EDIT_CENTRE:   checked = (pf.wAlignment == PFA_CENTER);   break;
         case ID_EDIT_JUSTIFY:  checked = (pf.wAlignment == PFA_JUSTIFY);  break;
         }
      }

      // Set state
      pCmd->Enable(state ? TRUE : FALSE);
      pCmd->SetCheck(checked ? TRUE : FALSE);
   }
   
   /// <summary>Query state of clipboard command</summary>
   void LanguageEditView::OnQueryClipboard(CCmdUI* pCmd)
   {
      bool state = false, 
           checked = false;

      // Query relevant states
      bool selection = RichEdit.HasSelection(),
           library   = GetDocument()->Virtual,
           validMode = GetDocument()->CurrentMode != EditMode::Display,
           hasFocus  = ::GetFocus() == RichEdit;

      // Disable-all if tool window is focused, or using 'display' mode
      if (hasFocus && validMode)
         switch (pCmd->m_nID)
         {
         // Non-modifying
         case ID_EDIT_COPY:        state = selection;    break;
         case ID_EDIT_SELECT_ALL:  state = true;         break;
         case ID_EDIT_FIND:        state = false;        break;

         // Modifying
         case ID_EDIT_CLEAR:  
         case ID_EDIT_CUT:         state = !library && selection;                            break;
         case ID_EDIT_PASTE:       state = !library && RichEdit.CanPaste(CF_UNICODETEXT);    break;
         case ID_EDIT_INSERT:      state = !library;                                         break;

         // Modifying: Set item text
         case ID_EDIT_UNDO:        state = !library && RichEdit.CanUndo();
                                   pCmd->SetText(RichEdit.GetUndoMenuItem().c_str());        break;

         case ID_EDIT_REDO:        state = !library && RichEdit.CanRedo();
                                   pCmd->SetText(RichEdit.GetRedoMenuItem().c_str());        break;
         }


      // Set state
      pCmd->Enable(state ? TRUE : FALSE);
      pCmd->SetCheck(checked ? TRUE : FALSE);
   }

   /// <summary>Query state of formatting command</summary>
   void LanguageEditView::OnQueryFormat(CCmdUI* pCmd)
   {
      bool state = false, 
           checked = false;

      // Query relevant states
      bool selection = GetDocument()->SelectedString != nullptr,
           library   = GetDocument()->Virtual,
           editMode  = GetDocument()->CurrentMode == EditMode::Edit,
           hasFocus  = ::GetFocus() == RichEdit;

      // Require: string, edit mode, non-library document, input-focus
      if (state = selection && !library && editMode && hasFocus)
      {
         CharFormat  cf(CFM_BOLD|CFM_ITALIC|CFM_UNDERLINE, 0);
         
         // Query format
         RichEdit.GetSelectionCharFormat(cf);
         switch (pCmd->m_nID)
         {
         case ID_EDIT_BOLD:       checked = (cf.dwEffects & CFE_BOLD) != 0;       break;
         case ID_EDIT_ITALIC:     checked = (cf.dwEffects & CFE_ITALIC) != 0;     break;
         case ID_EDIT_UNDERLINE:  checked = (cf.dwEffects & CFE_UNDERLINE) != 0;  break;
         case ID_EDIT_ADD_BUTTON: state = true;                                   break;
         case ID_EDIT_COLOUR:     state = RichEdit.HasSelection();                break;
         }
      }

      // Set state
      pCmd->Enable(state ? TRUE : FALSE);
      pCmd->SetCheck(checked ? TRUE : FALSE);
   }


   /// <summary>Re-creates toolbar</summary>
   /// <param name="uFlags">The flags.</param>
   /// <param name="lpszSection">The section.</param>
   void LanguageEditView::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
   {
      CFormView::OnSettingChange(uFlags, lpszSection);

      // Adjust layout
      AdjustLayout();
   }

   /// <summary>Adjusts layout</summary>
   /// <param name="nType">Type of the resize</param>
   /// <param name="cx">The new width</param>
   /// <param name="cy">The new height</param>
   void LanguageEditView::OnSize(UINT nType, int cx, int cy)
   {
      CFormView::OnSize(nType, cx, cy);
      AdjustLayout();
   }
   
   
   /// <summary>Display the currently selected string</summary>
   void LanguageEditView::OnStringSelectionChanged()
   {
      try 
      {
         // Display new string
         RichEdit.Refresh();
      }
      catch (ExceptionBase& e) { 
         GetDocument()->SelectedString = nullptr;
         theApp.ShowError(HERE, ApplicationException(e), L"Unable to display selected string");
      }

#ifdef RESCINDED     // I don't remember why I needed this
      catch (exception& e) { 
         GetDocument()->SelectedString = nullptr;
         theApp.ShowError(HERE, e, L"Unable to display selected string");
      }
#endif
   }

   /// <summary>Called when text selection changes (ie. caret has moved)</summary>
   /// <param name="pNMHDR">Message header</param>
   /// <param name="result">The result.</param>
   void LanguageEditView::OnTextSelectionChange(NMHDR* pNMHDR, LRESULT* result)
   {
      SELCHANGE *sel = reinterpret_cast<SELCHANGE*>(pNMHDR);
      *result = TRUE;
      
      try
      {
         // Set/Clear button selection
         if (sel->seltyp == SEL_OBJECT)      // Object selected (with no extra characters)
            GetDocument()->SelectedButton = RichEdit.GetButton(sel->chrg);
         else
            GetDocument()->SelectedButton = nullptr;
         
         // DEBUG:
         /*Console << "Selection changed:" 
                 << " SEL_EMPTY=" << (sel->seltyp == SEL_EMPTY)
                 << " SEL_TEXT=" << ((sel->seltyp & SEL_TEXT) != 0)
                 << " SEL_MULTICHAR=" << ((sel->seltyp & SEL_MULTICHAR) != 0)
                 << " SEL_OBJECT=" << ((sel->seltyp & SEL_OBJECT) != 0)
                 << " SEL_MULTIOBJECT=" << ((sel->seltyp & SEL_MULTIOBJECT) != 0) << ENDL;*/
      }
      catch (ExceptionBase& e) {
         Console.Log(HERE, e);
      }
   }
   // ------------------------------- PRIVATE METHODS ------------------------------

   
NAMESPACE_END2(GUI,Views)


