
// DiffView.cpp : implementation of the DiffView class
//

#include "stdafx.h"
#include "DiffDocument.h"
#include "DiffView.h"
#include "PropertiesWnd.h"
#include "../Logic/RtfScriptWriter.h"
#include "../Logic/ScriptParser.h"
#include "../Logic/StringStream.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Views)

   // --------------------------------- STATIC DATA --------------------------------

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNCREATE(DiffView, CFormView)

   BEGIN_MESSAGE_MAP(DiffView, CFormView)
      ON_WM_ACTIVATE()
      ON_WM_CONTEXTMENU()
      ON_WM_DESTROY()
	   ON_WM_RBUTTONUP()
      ON_WM_SIZE()
      ON_WM_SETFOCUS()
      ON_WM_SETTINGCHANGE()
      ON_COMMAND(ID_EDIT_COPY, &DiffView::OnClipboardCopy)
      ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, &DiffView::OnQueryCommand)
      ON_EN_HSCROLL(IDC_DIFF_EDIT, &DiffView::OnEditHScroll)
      ON_EN_VSCROLL(IDC_DIFF_EDIT, &DiffView::OnEditVScroll)
   END_MESSAGE_MAP()

   // -------------------------------- CONSTRUCTION --------------------------------
   
   DiffView::DiffView() : CFormView(DiffView::IDD), Type(DiffViewType::Original)
   {
   }

   DiffView::~DiffView()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------
   
   /// <summary>Gets the diff document</summary>
   DiffDocument* DiffView::GetDocument() const 
   {
#ifdef _DEBUG
	   return dynamic_cast<DiffDocument*>(m_pDocument);
#else
      return reinterpret_cast<DiffDocument*>(m_pDocument);
#endif
   }

   /// <summary>Sets whether view is original or alternate.</summary>
   /// <param name="t">The t.</param>
   void  DiffView::SetType(DiffViewType t)
   {
      Type = t;
   }

   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Adjusts the layout.</summary>
   void  DiffView::AdjustLayout()
   {
      // Destroyed/Minimised
	   if (!RichEdit.GetSafeHwnd() || theApp.IsMimized())
         return;
         
      ClientRect wnd(this);

      // Stretch Edit over view
      RichEdit.SetWindowPos(nullptr, 0, 0, wnd.Width(), wnd.Height(), SWP_NOZORDER | SWP_NOACTIVATE);
   }

   /// <summary>Does the data exchange.</summary>
   /// <param name="pDX">The p dx.</param>
   void DiffView::DoDataExchange(CDataExchange* pDX)
   {
      __super::DoDataExchange(pDX);
      DDX_Control(pDX, IDC_DIFF_EDIT, RichEdit);
   }

   /// <summary>Show properties</summary>
   /// <param name="bActivate">whether activating.</param>
   /// <param name="pActivateView">The activate view.</param>
   /// <param name="pDeactiveView">The deactive view.</param>
   void DiffView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
   {
      // Show properties
      if (bActivate)
         CPropertiesWnd::Connect(GetDocument(), true);

      // Activate
      __super::OnActivateView(bActivate, pActivateView, pDeactiveView);
   }
   
   /// <summary>Copy text selection to clipboard</summary>
   void DiffView::OnClipboardCopy()
   {
      RichEdit.Copy();
   }

   /// <summary>Display script edit context window</summary>
   /// <param name="pWnd">window clicked</param>
   /// <param name="point">The point in screen co-ordinates</param>
   void DiffView::OnContextMenu(CWnd* pWnd, CPoint point)
   {
      // Ensure scriptEdit was clicked
      if (pWnd && pWnd->m_hWnd == RichEdit.m_hWnd)
	      theApp.GetContextMenuManager()->ShowPopupMenu(IDM_EDIT_POPUP, point.x, point.y, this, TRUE);
   }

   /// <summary>Scroll the opposite view</summary>
   void  DiffView::OnEditHScroll()
   {
      //Console << "DiffView::OnEditHScroll" << ENDL;

      // Scroll opposite view
      RichEdit.ScrollSibling();
   }

   /// <summary>Scroll the opposite view.</summary>
   void  DiffView::OnEditVScroll()
   {
      //Console << "DiffView::OnEditVScroll" << ENDL;

      // Scroll opposite view
      RichEdit.ScrollSibling();
   }

   /// <summary>Clear properties</summary>
   void DiffView::OnDestroy()
   {
      // Hide properties info
      CPropertiesWnd::Connect(GetDocument(), false);

      // Destroy
      __super::OnDestroy();
   }


   /// <summary>Initialize and populate the edit</summary>
   void DiffView::OnInitialUpdate()
   {
      // Init
	   __super::OnInitialUpdate();
	   ResizeParentToFit();

      try
      {
         // Init DiffEdit
         RichEdit.Initialize(Type, GetDocument(), GetDocument()->Source);

         // Display original/alternate text
         RichEdit.SetPlainText(Type == DiffViewType::Original ? GetDocument()->Original : GetDocument()->Alternate);
      }
      catch (ExceptionBase& e)
      {
         theApp.ShowError(HERE, e, L"Unable to initialise the script view");
      }
   }

   /// <summary>Query state of clipboard copy context menu command</summary>
   /// <param name="pCmdUI">UI object</param>
   void DiffView::OnQueryCommand(CCmdUI *pCmdUI)
   {
      BOOL state = TRUE, 
           check = FALSE;

      //
      switch (pCmdUI->m_nID)
      {
      // Clipboard
      case ID_EDIT_COPY:     state = RichEdit.HasSelection();            break;
      }

      // Set state
      pCmdUI->SetCheck(check);
      pCmdUI->Enable(state);
   }
   
   /// <summary>Invoke context menu</summary>
   /// <param name="nFlags">flags.</param>
   /// <param name="point">point in client co-ords.</param>
   void DiffView::OnRButtonUp(UINT nFlags, CPoint point)
   {
	   ClientToScreen(&point);
	   OnContextMenu(this, point);
   }

   
   /// <summary>Adjust layout</summary>
   /// <param name="uFlags">The flags.</param>
   /// <param name="lpszSection">The section.</param>
   void DiffView::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
   {
      // Adjust layout
      AdjustLayout();
   }
   
   /// <summary>Set the focus to the diff Edit</summary>
   /// <param name="pOldWnd">The old WND.</param>
   void DiffView::OnSetFocus(CWnd* pOldWnd)
   {
      __super::OnSetFocus(pOldWnd);
      RichEdit.SetFocus();
   }
   /// <summary>Adjusts the layout</summary>
   /// <param name="nType">Type of resize.</param>
   /// <param name="cx">The width.</param>
   /// <param name="cy">The height.</param>
   void DiffView::OnSize(UINT nType, int cx, int cy)
   {
      __super::OnSize(nType, cx, cy);
      AdjustLayout();
   }
   
   // ------------------------------- PRIVATE METHODS ------------------------------


NAMESPACE_END2(GUI,Views)

