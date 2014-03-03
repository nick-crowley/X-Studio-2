#include "stdafx.h"

#include "PropertiesWnd.h"
#include "ScriptView.h"
#include "MainWnd.h"
#include "Logic/ScriptObjectLibrary.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)

   /// <summary>Properties window instance.</summary>
   CPropertiesWnd*  CPropertiesWnd::Instance = nullptr;

   const UINT IDC_PROPERTY_GRID = 4;

   // --------------------------------- APP WIZARD ---------------------------------

   BEGIN_MESSAGE_MAP(CPropertiesWnd, CDockablePane)
	   ON_WM_CREATE()
	   ON_WM_SIZE()
      ON_WM_SETFOCUS()
	   ON_WM_SETTINGCHANGE()
	   ON_COMMAND(ID_EXPAND_ALL, OnExpandAllProperties)
      ON_COMMAND(ID_SORTPROPERTIES, OnSortProperties)
      ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyUpdated)
      ON_UPDATE_COMMAND_UI_RANGE(ID_INSERT_ARGUMENT, ID_REMOVE_ARGUMENT, OnQueryCustomCommand)
      ON_UPDATE_COMMAND_UI(ID_EXPAND_ALL, OnQueryExpandAllProperties)
      ON_UPDATE_COMMAND_UI(ID_SORTPROPERTIES, OnQuerySortProperties)
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
      EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);
   }

   // ------------------------------ PROTECTED METHODS -----------------------------

   /// <summary>Adjusts the layout.</summary>
   void CPropertiesWnd::AdjustLayout()
   {
      // Ensure exists
	   if (!GetSafeHwnd() || (AfxGetMainWnd() && AfxGetMainWnd()->IsIconic()))
         return;

	   CRect rectClient;
	   GetClientRect(rectClient);

      // Toolbar height
	   int barHeight = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

      // Snap toolbar to top, stretch grid over remainder
	   m_wndToolBar.SetWindowPos(nullptr, rectClient.left, rectClient.top, rectClient.Width(), barHeight, SWP_NOACTIVATE | SWP_NOZORDER);
	   m_wndPropList.SetWindowPos(nullptr, rectClient.left, rectClient.top + barHeight, rectClient.Width(), rectClient.Height() - barHeight, SWP_NOACTIVATE | SWP_NOZORDER);
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

      // Clear 
      SetRedraw(FALSE);
      m_wndPropList.RemoveAll();

      // Disconnect
      if (!connect)
         Source = nullptr;
      
      // Connect + Populate
      else 
      {
         Source = src;
         Source->OnDisplayProperties(m_wndPropList);
      }

      // Redraw
      SetRedraw(TRUE);
      m_wndPropList.Invalidate();
      m_wndPropList.UpdateWindow();
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

         // Create property grid
	      if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, IDC_PROPERTY_GRID))
	         throw Win32Exception(HERE, L"Failed to create Properties Grid");

         SetPropListFont();

	      // Grid
         m_wndPropList.EnableHeaderCtrl(FALSE);
	      m_wndPropList.EnableDescriptionArea();
	      m_wndPropList.SetVSDotNetLook();
	      m_wndPropList.MarkModifiedProperties();

         // Toolbar
         m_wndToolBar.Create(this, IDR_PROPERTIES, L"Properties");

         // Layout
	      AdjustLayout();
	      return 0;
      }
      catch (ExceptionBase& e) {
         Console.Log(HERE, e);
         return -1;
      }
   }

   void CPropertiesWnd::OnExpandAllProperties()
   {
	   m_wndPropList.ExpandAll();
   }

   LRESULT CPropertiesWnd::OnPropertyUpdated(WPARAM wParam, LPARAM lParam)
   {
      CMFCPropertyGridProperty* p = reinterpret_cast<CMFCPropertyGridProperty*>(lParam);

      if (Source)
         Source->OnPropertyUpdated(p);

      return 0;
   }

   void CPropertiesWnd::OnQueryCustomCommand(CCmdUI* pCmdUI)
   {
      if (Source)
         Source->OnQueryCustomCommand(pCmdUI);
	   
      //pCmdUI->Enable(TRUE);
      //pCmdUI->ContinueRouting();
   }

   
   void CPropertiesWnd::OnQueryExpandAllProperties(CCmdUI* /* pCmdUI */)
   {
   }

   void CPropertiesWnd::OnQuerySortProperties(CCmdUI* pCmdUI)
   {
	   pCmdUI->SetCheck(m_wndPropList.IsAlphabeticMode());
   }

   
   void CPropertiesWnd::OnSetFocus(CWnd* pOldWnd)
   {
      CDockablePane::OnSetFocus(pOldWnd);
	   m_wndPropList.SetFocus();
   }

   void CPropertiesWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
   {
	   CDockablePane::OnSettingChange(uFlags, lpszSection);
	   SetPropListFont();
   }

   void CPropertiesWnd::OnSize(UINT nType, int cx, int cy)
   {
	   CDockablePane::OnSize(nType, cx, cy);
	   AdjustLayout();
   }

   void CPropertiesWnd::OnSortProperties()
   {
	   m_wndPropList.SetAlphabeticMode(!m_wndPropList.IsAlphabeticMode());
   }
   
   void CPropertiesWnd::SetPropListFont()
   {
	   ::DeleteObject(m_fntPropList.Detach());

	   LOGFONT lf;
	   afxGlobalData.fontRegular.GetLogFont(&lf);

	   NONCLIENTMETRICS info;
	   info.cbSize = sizeof(info);

	   afxGlobalData.GetNonClientMetrics(info);

	   lf.lfHeight = info.lfMenuFont.lfHeight;
	   lf.lfWeight = info.lfMenuFont.lfWeight;
	   lf.lfItalic = info.lfMenuFont.lfItalic;

	   m_fntPropList.CreateFontIndirect(&lf);

	   m_wndPropList.SetFont(&m_fntPropList);
   }
   
   void CPropertiesWnd::SetVSDotNetLook(BOOL bSet)
	{
		m_wndPropList.SetVSDotNetLook(bSet);
		m_wndPropList.SetGroupNameFullWidth(bSet);
	}
   
   /*void CPropertiesWnd::InitPropList()
   {
	   CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("Appearance"));

	   pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("3D Look"), (_variant_t) false, _T("Specifies the window's font will be non-bold and controls will have a 3D border")));

	   CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("Border"), _T("Dialog Frame"), _T("One of: None, Thin, Resizable, or Dialog Frame"));
	   pProp->AddOption(_T("None"));
	   pProp->AddOption(_T("Thin"));
	   pProp->AddOption(_T("Resizable"));
	   pProp->AddOption(_T("Dialog Frame"));
	   pProp->AllowEdit(FALSE);

	   pGroup1->AddSubItem(pProp);
	   pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("Caption"), (_variant_t) _T("About"), _T("Specifies the text that will be displayed in the window's title bar")));

	   m_wndPropList.AddProperty(pGroup1);

	   CMFCPropertyGridProperty* pSize = new CMFCPropertyGridProperty(_T("Window Size"), 0, TRUE);

	   pProp = new CMFCPropertyGridProperty(_T("Height"), (_variant_t) 250l, _T("Specifies the window's height"));
	   pProp->EnableSpinControl(TRUE, 50, 300);
	   pSize->AddSubItem(pProp);

	   pProp = new CMFCPropertyGridProperty( _T("Width"), (_variant_t) 150l, _T("Specifies the window's width"));
	   pProp->EnableSpinControl(TRUE, 50, 200);
	   pSize->AddSubItem(pProp);

	   m_wndPropList.AddProperty(pSize);

	   CMFCPropertyGridProperty* pGroup2 = new CMFCPropertyGridProperty(_T("Font"));

	   LOGFONT lf;
	   CFont* font = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	   font->GetLogFont(&lf);

	   lstrcpy(lf.lfFaceName, _T("Arial"));

	   pGroup2->AddSubItem(new CMFCPropertyGridFontProperty(_T("Font"), lf, CF_EFFECTS | CF_SCREENFONTS, _T("Specifies the default font for the window")));
	   pGroup2->AddSubItem(new CMFCPropertyGridProperty(_T("Use System Font"), (_variant_t) true, _T("Specifies that the window uses MS Shell Dlg font")));

	   m_wndPropList.AddProperty(pGroup2);

	   CMFCPropertyGridProperty* pGroup3 = new CMFCPropertyGridProperty(_T("Misc"));
	   pProp = new CMFCPropertyGridProperty(_T("(Name)"), _T("Application"));
	   pProp->Enable(FALSE);
	   pGroup3->AddSubItem(pProp);

	   CMFCPropertyGridColorProperty* pColorProp = new CMFCPropertyGridColorProperty(_T("Window Color"), RGB(210, 192, 254), NULL, _T("Specifies the default window color"));
	   pColorProp->EnableOtherButton(_T("Other..."));
	   pColorProp->EnableAutomaticButton(_T("Default"), ::GetSysColor(COLOR_3DFACE));
	   pGroup3->AddSubItem(pColorProp);

	   static const TCHAR szFilter[] = _T("Icon Files(*.ico)|*.ico|All Files(*.*)|*.*||");
	   pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("Icon"), TRUE, _T(""), _T("ico"), 0, szFilter, _T("Specifies the window icon")));

	   pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("Folder"), _T("c:\\")));

	   m_wndPropList.AddProperty(pGroup3);

	   CMFCPropertyGridProperty* pGroup4 = new CMFCPropertyGridProperty(_T("Hierarchy"));

	   CMFCPropertyGridProperty* pGroup41 = new CMFCPropertyGridProperty(_T("First sub-level"));
	   pGroup4->AddSubItem(pGroup41);

	   CMFCPropertyGridProperty* pGroup411 = new CMFCPropertyGridProperty(_T("Second sub-level"));
	   pGroup41->AddSubItem(pGroup411);

	   pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("Item 1"), (_variant_t) _T("Value 1"), _T("This is a description")));
	   pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("Item 2"), (_variant_t) _T("Value 2"), _T("This is a description")));
	   pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("Item 3"), (_variant_t) _T("Value 3"), _T("This is a description")));

	   pGroup4->Expand(FALSE);
	   m_wndPropList.AddProperty(pGroup4);
   }*/
   
   
   // ------------------------------- PRIVATE METHODS ------------------------------

   

NAMESPACE_END2(GUI,Windows)

