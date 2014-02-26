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
      ON_COMMAND_RANGE(ID_VIEW_SOURCE, ID_VIEW_DISPLAY, &LanguageEditView::OnCommandEditMode)
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
   
   /// <summary>Called when query edit mode.</summary>
   /// <param name="pCmd">The command.</param>
   void LanguageEditView::OnQueryEditMode(CCmdUI* pCmd) const
   {
      // Disable if no string displayed
      pCmd->Enable(RichEdit.IsWindowEnabled());

      // Check correct state
      switch (pCmd->m_nID)
      {
      case ID_VIEW_SOURCE:    pCmd->SetCheck(RichEdit.GetEditMode() == LanguageEdit::EditMode::Source);   break;
      case ID_VIEW_EDITOR:    pCmd->SetCheck(RichEdit.GetEditMode() == LanguageEdit::EditMode::Edit);     break;
      case ID_VIEW_DISPLAY:   pCmd->SetCheck(RichEdit.GetEditMode() == LanguageEdit::EditMode::Display);  break;
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
   void LanguageEditView::OnCommandEditMode(UINT nID)
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


