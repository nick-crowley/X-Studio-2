// LanguageEditView.cpp : implementation file
//

#include "stdafx.h"
#include "LanguageEditView.h"
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
      ON_COMMAND_RANGE(ID_VIEW_SOURCE, ID_VIEW_DISPLAY, &LanguageEditView::OnCommandChangeMode)
      ON_COMMAND(ID_EDIT_ADD_BUTTON, &LanguageEditView::OnCommandInsertButton)
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
         switch (pCmd->m_nID)
         {
         // Clipboard: Set based on selection
         case ID_EDIT_CUT:
         case ID_EDIT_COPY:    return pCmd->Enable(RichEdit.HasSelection());
         case ID_EDIT_PASTE:   
         case ID_EDIT_CLEAR:   return pCmd->Enable(TRUE);

         // Button: Require editor mode
         case ID_EDIT_ADD_BUTTON:
            pCmd->SetCheck(FALSE);
            pCmd->Enable(RichEdit.GetEditMode() == LanguageEdit::EditMode::Edit);
            break;

         // Format: Require editor mode
         case ID_EDIT_BOLD:       
         case ID_EDIT_ITALIC:     
         case ID_EDIT_UNDERLINE:  
            if (RichEdit.GetEditMode() == LanguageEdit::EditMode::Edit)
            {
               pCmd->Enable();
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
               pCmd->Enable();
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

   /// <summary>Inserts a new button</summary>
   void LanguageEditView::OnCommandInsertButton()
   {
      static int LastID = 1;
      RichEdit.InsertButton(GuiString(L"Button %d", LastID++), L"id", Colour::Orange);
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
         // Listen for string SelectionChanged
         auto strView = GetDocument()->GetView<LanguageStringView>();
         fnStringSelectionChanged = strView->SelectionChanged.Register(this, &LanguageEditView::onStringSelectionChanged);
      
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


