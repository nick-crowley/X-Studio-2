// LanguageEditView.cpp : implementation file
//

#include "stdafx.h"
#include "LanguageEditView.h"
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
      ON_COMMAND_RANGE(ID_EDIT_UNDO, ID_EDIT_REDO, &LanguageEditView::OnCommandChangeText)
      ON_COMMAND_RANGE(ID_EDIT_COPY, ID_EDIT_CUT, &LanguageEditView::OnCommandChangeText)
      ON_COMMAND_RANGE(ID_EDIT_PASTE, ID_EDIT_PASTE, &LanguageEditView::OnCommandChangeText)
      ON_COMMAND_RANGE(ID_EDIT_CLEAR, ID_EDIT_CLEAR, &LanguageEditView::OnCommandChangeText)
      ON_COMMAND_RANGE(ID_EDIT_BOLD, ID_EDIT_ADD_BUTTON, &LanguageEditView::OnCommandChangeText)
      ON_COMMAND_RANGE(ID_VIEW_SOURCE, ID_VIEW_DISPLAY, &LanguageEditView::OnCommandChangeMode)
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

   /// <summary>Queries the state of editing mode commands.</summary>
   /// <param name="pCmd">The command.</param>
   void LanguageEditView::OnQueryModeCommand(CCmdUI* pCmd) const
   {
      // Disable if no string displayed
      if (RichEdit.IsReadOnly())
      {
         pCmd->Enable(FALSE);
         pCmd->SetCheck(FALSE);
      }
      else
      {
         // Check correct state
         pCmd->Enable();
         switch (pCmd->m_nID)
         {
         case ID_VIEW_SOURCE:    pCmd->SetCheck(RichEdit.GetEditMode() == LanguageEdit::EditMode::Source);   break;
         case ID_VIEW_EDITOR:    pCmd->SetCheck(RichEdit.GetEditMode() == LanguageEdit::EditMode::Edit);     break;
         case ID_VIEW_DISPLAY:   pCmd->SetCheck(RichEdit.GetEditMode() == LanguageEdit::EditMode::Display);  break;
         }
      }
   }

   /// <summary>Queries the state of clipboard commands.</summary>
   /// <param name="pCmd">The command.</param>
   void LanguageEditView::OnQueryClipboardCommand(CCmdUI* pCmd) const
   {
      // Disable if no string displayed
      if (RichEdit.IsReadOnly())
      {
         pCmd->Enable(FALSE);
         pCmd->SetCheck(FALSE);
      }
      else
      {
         // Check correct state
         pCmd->SetCheck(FALSE);
         switch (pCmd->m_nID)
         {
         // Clipboard: Set based on selection
         case ID_EDIT_CUT:
         case ID_EDIT_COPY:    
            return pCmd->Enable(RichEdit.HasSelection());

         // Paste/Delete: Always enabled
         case ID_EDIT_PASTE:   
         case ID_EDIT_CLEAR:   
            return pCmd->Enable(TRUE);

         // Undo:
         case ID_EDIT_UNDO:
            pCmd->Enable(RichEdit.CanUndo());
            pCmd->SetText( GuiString(RichEdit.CanUndo() ? L"Undo %s" : L"Undo", GetString(RichEdit.GetUndoName())).c_str() );
            break;
         // Redo:
         case ID_EDIT_REDO:
            pCmd->Enable(RichEdit.CanRedo());
            pCmd->SetText( GuiString(RichEdit.CanRedo() ? L"Redo %s" : L"Redo", GetString(RichEdit.GetRedoName())).c_str() );
            break;
         }
      }
   }

   /// <summary>Queries the state of formatting commands.</summary>
   /// <param name="pCmd">The command.</param>
   void LanguageEditView::OnQueryFormatCommand(CCmdUI* pCmd) const
   {
      // Disable if no string displayed
      if (RichEdit.IsReadOnly())
      {
         pCmd->Enable(FALSE);
         pCmd->SetCheck(FALSE);
      }
      else
      {
         CharFormat  cf(CFM_BOLD|CFM_ITALIC|CFM_UNDERLINE, 0);
         ParaFormat  pf(PFM_ALIGNMENT);

         // Check correct state
         pCmd->SetCheck(FALSE);
         switch (pCmd->m_nID)
         {
         // Button: Require editor mode
         case ID_EDIT_ADD_BUTTON:
            pCmd->Enable(RichEdit.GetEditMode() == LanguageEdit::EditMode::Edit);
            break;

         // Colour: Not implemented
         case ID_EDIT_COLOUR:
            pCmd->Enable(FALSE);
            break;

         // Format: Require editor mode
         case ID_EDIT_BOLD:       
         case ID_EDIT_ITALIC:     
         case ID_EDIT_UNDERLINE:  
            if (RichEdit.GetEditMode() == LanguageEdit::EditMode::Edit)
            {
               pCmd->Enable(TRUE);
               RichEdit.GetSelectionCharFormat(cf);
            
               // Check formatting 
               switch (pCmd->m_nID)
               {
               case ID_EDIT_BOLD:       return pCmd->SetCheck(cf.dwEffects & CFE_BOLD ? TRUE : FALSE);
               case ID_EDIT_ITALIC:     return pCmd->SetCheck(cf.dwEffects & CFE_ITALIC ? TRUE : FALSE);
               case ID_EDIT_UNDERLINE:  return pCmd->SetCheck(cf.dwEffects & CFE_UNDERLINE ? TRUE : FALSE);
               }
            }
            break;

         // Alignment: Require editor mode
         case ID_EDIT_LEFT:
         case ID_EDIT_RIGHT:
         case ID_EDIT_CENTRE:
         case ID_EDIT_JUSTIFY:
            if (RichEdit.GetEditMode() == LanguageEdit::EditMode::Edit)
            {
               pCmd->Enable(TRUE);
               RichEdit.GetParaFormat(pf);
            
               // Check alignment
               switch (pCmd->m_nID)
               {
               case ID_EDIT_LEFT:     return pCmd->SetCheck(pf.wAlignment == PFA_LEFT    ? TRUE : FALSE);
               case ID_EDIT_RIGHT:    return pCmd->SetCheck(pf.wAlignment == PFA_RIGHT   ? TRUE : FALSE);
               case ID_EDIT_CENTRE:   return pCmd->SetCheck(pf.wAlignment == PFA_CENTER  ? TRUE : FALSE);
               case ID_EDIT_JUSTIFY:  return pCmd->SetCheck(pf.wAlignment == PFA_JUSTIFY ? TRUE : FALSE);
               }
            }
            break;
         }
      }
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
   
   /// <summary>Changes the edit mode.</summary>
   /// <param name="nID">The command identifier.</param>
   void LanguageEditView::OnCommandChangeMode(UINT nID)
   {
      try
      {
         switch (nID)
         {
         case ID_VIEW_SOURCE:    RichEdit.SetEditMode(LanguageEdit::EditMode::Source);   break;
         case ID_VIEW_EDITOR:    RichEdit.SetEditMode(LanguageEdit::EditMode::Edit);     break;
         case ID_VIEW_DISPLAY:   RichEdit.SetEditMode(LanguageEdit::EditMode::Display);  break;
         }
      }
      catch (ExceptionBase& e) { 
         theApp.ShowError(HERE, e, L"Unable to change editor mode");
      }
   }

   /// <summary>Perform text formatting command.</summary>
   /// <param name="nID">The command identifier.</param>
   void LanguageEditView::OnCommandChangeText(UINT nID)
   {
      static int LastButtonID = 1;  

      // Execute
      switch (nID)
      {
      // Clipboard: Set based on selection
      case ID_EDIT_CUT:     RichEdit.Cut();     break;
      case ID_EDIT_COPY:    RichEdit.Copy();    break;
      case ID_EDIT_PASTE:   RichEdit.PasteFormat(CF_UNICODETEXT);   break;
      case ID_EDIT_CLEAR:   RichEdit.ReplaceSel(L"", TRUE);         break;

      // Undo/Redo:
      case ID_EDIT_UNDO:    RichEdit.Undo();    break;
      case ID_EDIT_REDO:    RichEdit.Redo();    break;

      // Button: Insert at caret
      case ID_EDIT_ADD_BUTTON:
         RichEdit.InsertButton(GuiString(L"Button %d", LastButtonID++), L"id");
         break;

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
      }
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
         if (!ToolBar.Create(this, PrefsLib.LargeToolbars ? IDR_EDITOR_24 : IDR_EDITOR_16, PrefsLib.LargeToolbars ? IDB_EDITOR_24_GREY : IDB_EDITOR_16_GREY))
            throw Win32Exception(HERE, L"Unable to create toolbar");

         ToolBar.SetRouteCommandsViaFrame(TRUE);

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
         fnStringSelectionChanged = GetDocument()->StringSelectionChanged.Register(this, &LanguageEditView::onStringSelectionChanged);
      
         // Init RichEdit
         RichEdit.Initialize(GetDocument());
      }
      catch (ExceptionBase& e) {
         Console.Log(HERE, e);
      }
   }
   
   /// <summary>Re-creates toolbar</summary>
   /// <param name="uFlags">The flags.</param>
   /// <param name="lpszSection">The section.</param>
   void LanguageEditView::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
   {
      CFormView::OnSettingChange(uFlags, lpszSection);

      // Re-create toolbar
      ToolBar.DestroyWindow();
      ToolBar.Create(this, PrefsLib.LargeToolbars ? IDR_EDITOR_24 : IDR_EDITOR_16, PrefsLib.LargeToolbars ? IDB_EDITOR_24_GREY : IDB_EDITOR_16_GREY);

      // Adjust layout
      AdjustLayout();
   }

   
   /// <summary>Display the currently selected string</summary>
   void LanguageEditView::onStringSelectionChanged()
   {
      try 
      {
         // Display new string
         RichEdit.Refresh();
      }
      catch (ExceptionBase& e) { 
         theApp.ShowError(HERE, e, L"Unable to display selected string");
      }
      catch (exception& e) { 
         theApp.ShowError(HERE, e, L"Unable to display selected string");
      }
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

   // ------------------------------- PRIVATE METHODS ------------------------------

   
NAMESPACE_END2(GUI,Views)


