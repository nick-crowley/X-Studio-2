#include "stdafx.h"

#include "PropertiesWnd.h"
#include "ScriptView.h"
#include "MainWnd.h"
#include "../Logic/ScriptObjectLibrary.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)

   /// <summary>Properties window instance.</summary>
   CPropertiesWnd*  CPropertiesWnd::Instance = nullptr;

   // --------------------------------- APP WIZARD ---------------------------------

   BEGIN_MESSAGE_MAP(CPropertiesWnd, CDockablePane)
	   ON_WM_CREATE()
      ON_WM_CONTEXTMENU()
	   ON_WM_SIZE()
      ON_WM_PAINT()
      ON_WM_SETFOCUS()
	   ON_WM_SETTINGCHANGE()
	   ON_COMMAND(ID_EXPAND_ALL, OnCommandExpandAll)
      ON_COMMAND(ID_SORTPROPERTIES, OnCommandSort)
      ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyUpdated)
      ON_UPDATE_COMMAND_UI_RANGE(ID_INSERT_ARGUMENT, ID_REMOVE_ARGUMENT, OnQueryCustomCommand)
      ON_UPDATE_COMMAND_UI(ID_EXPAND_ALL, OnQueryCommand)
      ON_UPDATE_COMMAND_UI(ID_SORTPROPERTIES, OnQueryCommand)
   END_MESSAGE_MAP()

   // -------------------------------- CONSTRUCTION --------------------------------

   CPropertiesWnd::CPropertiesWnd() : Source(nullptr)
   {
      Instance = this;
   }

   CPropertiesWnd::~CPropertiesWnd()
   {
      Instance = nullptr;
   }

   // ------------------------------- STATIC METHODS -------------------------------

   /// <summary>Connects a source of properties to the properties window.</summary>
   /// <param name="src">The source.</param>
   /// <param name="connect">Connect or disconnect.</param>
   void  CPropertiesWnd::Connect(PropertySource* src, bool connect)
   {
      if (Instance)
         Instance->ConnectSource(src, connect);
   }

   // ------------------------------- PUBLIC METHODS -------------------------------

   /// <summary>Creates the window.</summary>
   /// <param name="parent">The parent.</param>
   /// <exception cref="Logic::Win32Exception">Unable to create window</exception>
   void CPropertiesWnd::Create(CWnd* parent)
   {
      DWORD style = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_RIGHT | CBRS_FLOAT_MULTI;

      // Create window
      if (!__super::Create(GuiString(IDR_PROPERTIES).c_str(), parent, MainWnd::DefaultSize, TRUE, IDR_PROPERTIES, style))
	      throw Win32Exception(HERE, L"Unable to create Properties window");

      SetIcon(theApp.LoadIconW(IDR_PROPERTIES, ::GetSystemMetrics(SM_CXSMICON)), FALSE);

      // Dock left/right
      EnableDocking(CBRS_ORIENT_VERT|CBRS_FLOAT_MULTI);
   }

   // ------------------------------ PROTECTED METHODS -----------------------------

   /// <summary>Adjusts the layout.</summary>
   void CPropertiesWnd::AdjustLayout()
   {
      // Destroyed/Minimised
	   if (!GetSafeHwnd() || theApp.IsMimized())
         return;

	   CRect rectClient;
	   GetClientRect(rectClient);

      // Toolbar height
	   int barHeight = ToolBar.CalcFixedLayout(FALSE, TRUE).cy;

      // Snap toolbar to top, stretch grid over remainder
	   ToolBar.SetWindowPos(nullptr, rectClient.left, rectClient.top, rectClient.Width(), barHeight, SWP_NOACTIVATE | SWP_NOZORDER);
	   Grid.SetWindowPos(nullptr, rectClient.left+1, rectClient.top+barHeight+1, rectClient.Width()-2, rectClient.Height()-barHeight-2, SWP_NOACTIVATE | SWP_NOZORDER);
   }

   /// <summary>Connects a source of properties.</summary>
   /// <param name="src">The source.</param>
   /// <param name="connect">Connect or disconnect.</param>
   /// <exception cref="Logic::ArgumentNullException">Source is null</exception>
   void  CPropertiesWnd::ConnectSource(PropertySource* src, bool connect)
   {
      REQUIRED(src);

      // Ignore if already disconnected
      if (!connect && Source != src)
         return;

      // Freeze
      //SetRedraw(FALSE);

      try
      {
         // Clear items
         Grid.RemoveAll();

         // Disconnect
         if (!connect)
            Source = nullptr;
         else 
         {  // Connect + Populate
            Source = src;
            Source->OnDisplayProperties(Grid);
         }
      }
      catch (ExceptionBase& e) {
         Console.Log(HERE, e);
      }

      // Redraw
      //SetRedraw(TRUE);
      /*Grid.Invalidate();
      Grid.UpdateWindow();*/
   }
   

   /// <summary>Select item and display context menu.</summary>
   /// <param name="pWnd">The WND.</param>
   /// <param name="point">The point.</param>
   void CPropertiesWnd::OnContextMenu(CWnd* pWnd, CPoint point)
   {
	   //CTreeCtrl* pWndTree = (CTreeCtrl*) &TreeView;
	   //ASSERT_VALID(pWndTree);

    //  // Display pane pop-up menu for toolbar
	   //if (pWnd != pWndTree)
	   //{
		  // CDockablePane::OnContextMenu(pWnd, point);
		  // return;
	   //}

	   //if (point != CPoint(-1, -1))
	   //{
		  // // Get position
		  // CPoint ptTree = point;
		  // pWndTree->ScreenToClient(&ptTree);

    //     // Select item
		  // UINT flags = TVHT_ONITEM;
		  // if (auto item = pWndTree->HitTest(ptTree, &flags))
    //        pWndTree->SelectItem(item);
	   //}

	   Grid.SetFocus();
	   theApp.GetContextMenuManager()->ShowPopupMenu(IDM_PROJECT_POPUP, point.x, point.y, this, TRUE);
   }

   
   /// <summary>expands all properties.</summary>
   void CPropertiesWnd::OnCommandExpandAll()
   {
	   Grid.ExpandAll();
   }

   /// <summary>Sorts all properties</summary>
   void CPropertiesWnd::OnCommandSort()
   {
	   Grid.SetAlphabeticMode(!Grid.IsAlphabeticMode());
   }
   
   /// <summary>Create child controls</summary>
   /// <param name="lpCreateStruct">create params</param>
   /// <returns></returns>
   int CPropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
   {
      try
      {
	      if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		      throw Win32Exception(HERE, L"Failed to create dockable pane");

	      CRect rectDummy;
	      rectDummy.SetRectEmpty();
         
         // Toolbar
         ToolBar.Create(this, IDR_PROPERTIES, L"Properties", false); 

         // Create property grid
	      if (!Grid.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, IDC_PROPERTY_GRID))
	         throw Win32Exception(HERE, L"Failed to create Properties Grid");

         // Set font
         UpdateFont();

	      // Grid
         Grid.EnableHeaderCtrl(FALSE);
	      Grid.EnableDescriptionArea();
         Grid.SetVSDotNetLook(TRUE);
		   Grid.SetGroupNameFullWidth(TRUE);
	      Grid.MarkModifiedProperties();

         // Layout
	      AdjustLayout();
	      return 0;
      }
      catch (ExceptionBase& e) {
         Console.Log(HERE, e);
         return -1;
      }
   }
   
   /// <summary>Manually paints border around grid.</summary>
   void CPropertiesWnd::OnPaint()
   {
	   CPaintDC dc(this); // device context for painting

	   CRect rc;
	   Grid.GetWindowRect(rc);
	   ScreenToClient(rc);

	   rc.InflateRect(1, 1);
	   dc.Draw3dRect(rc, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
   }

   /// <summary>Informs source a propery has been updated</summary>
   /// <param name="wParam">The w parameter.</param>
   /// <param name="lParam">The l parameter.</param>
   /// <returns></returns>
   LRESULT CPropertiesWnd::OnPropertyUpdated(WPARAM wParam, LPARAM lParam)
   {
      CMFCPropertyGridProperty* p = reinterpret_cast<CMFCPropertyGridProperty*>(lParam);

      // Inform source
      if (Source)
         Source->OnPropertyUpdated(p);

      return 0;
   }

   /// <summary>Query state of custom toolbar command</summary>
   /// <param name="pCmdUI">The p command UI.</param>
   void CPropertiesWnd::OnQueryCustomCommand(CCmdUI* pCmdUI)
   {
      // Delegate
      if (Source)
         Source->OnQueryCustomCommand(pCmdUI);
	   
      //pCmdUI->Enable(TRUE);
      //pCmdUI->ContinueRouting();
   }

   
   /// <summary>Called when query state of command.</summary>
   /// <param name="pCmdUI">The p command UI.</param>
   void CPropertiesWnd::OnQueryCommand(CCmdUI* pCmdUI)
   {
      switch (pCmdUI->m_nID)
      {
      case ID_EXPAND_ALL:     break;
      case ID_SORTPROPERTIES: pCmdUI->SetCheck(Grid.IsAlphabeticMode());
      }
   }

   
   /// <summary>Set focus to properties grid</summary>
   /// <param name="pOldWnd">The p old WND.</param>
   void CPropertiesWnd::OnSetFocus(CWnd* pOldWnd)
   {
      CDockablePane::OnSetFocus(pOldWnd);
	   Grid.SetFocus();
   }

   /// <summary>Update font when settings change.</summary>
   /// <param name="uFlags">The flags.</param>
   /// <param name="lpszSection">The section.</param>
   void CPropertiesWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
   {
	   CDockablePane::OnSettingChange(uFlags, lpszSection);

      // Update font
	   UpdateFont();
      AdjustLayout();
   }

   /// <summary>Adjusts the layout</summary>
   /// <param name="nType">Type of the n.</param>
   /// <param name="cx">The width.</param>
   /// <param name="cy">The height.</param>
   void CPropertiesWnd::OnSize(UINT nType, int cx, int cy)
   {
	   CDockablePane::OnSize(nType, cx, cy);
	   AdjustLayout();
   }
   
   /// <summary>Updates the font.</summary>
   void CPropertiesWnd::UpdateFont()
   {
	   /*::DeleteObject(m_fntPropList.Detach());

	   LOGFONT lf;
	   afxGlobalData.fontRegular.GetLogFont(&lf);

	   NONCLIENTMETRICS info;
	   info.cbSize = sizeof(info);

	   afxGlobalData.GetNonClientMetrics(info);

	   lf.lfHeight = info.lfMenuFont.lfHeight;
	   lf.lfWeight = info.lfMenuFont.lfWeight;
	   lf.lfItalic = info.lfMenuFont.lfItalic;

	   m_fntPropList.CreateFontIndirect(&lf);

	   Grid.SetFont(&m_fntPropList);*/

      // Set font
      Grid.SetFont(&theApp.ToolWindowFont);
   }
   
   
   // ------------------------------- PRIVATE METHODS ------------------------------

   

NAMESPACE_END2(GUI,Windows)

