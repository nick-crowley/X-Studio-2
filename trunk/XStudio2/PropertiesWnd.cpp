#include "stdafx.h"

#include "PropertiesWnd.h"
#include "ScriptView.h"
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

   // --------------------------------- APP WIZARD ---------------------------------

   BEGIN_MESSAGE_MAP(CPropertiesWnd, CDockablePane)
	   ON_WM_CREATE()
	   ON_WM_SIZE()
	   ON_COMMAND(ID_EXPAND_ALL, OnExpandAllProperties)
	   ON_UPDATE_COMMAND_UI(ID_EXPAND_ALL, OnUpdateExpandAllProperties)
	   ON_COMMAND(ID_SORTPROPERTIES, OnSortProperties)
	   ON_UPDATE_COMMAND_UI(ID_SORTPROPERTIES, OnUpdateSortProperties)
	   ON_COMMAND(ID_PROPERTIES1, OnProperties1)
	   ON_UPDATE_COMMAND_UI(ID_PROPERTIES1, OnUpdateProperties1)
	   ON_COMMAND(ID_PROPERTIES2, OnProperties2)
	   ON_UPDATE_COMMAND_UI(ID_PROPERTIES2, OnUpdateProperties2)
	   ON_WM_SETFOCUS()
	   ON_WM_SETTINGCHANGE()
   END_MESSAGE_MAP()

   // -------------------------------- CONSTRUCTION --------------------------------

   CPropertiesWnd::CPropertiesWnd() 
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
   void  CPropertiesWnd::Connect(PropertiesSource* src, bool connect)
   {
      if (Instance)
         Instance->ConnectSource(src, connect);
   }

   // ------------------------------- PUBLIC METHODS -------------------------------

   // ------------------------------ PROTECTED METHODS -----------------------------

   /// <summary>Adjusts the layout.</summary>
   void CPropertiesWnd::AdjustLayout()
   {
	   if (GetSafeHwnd () == NULL || (AfxGetMainWnd() != NULL && AfxGetMainWnd()->IsIconic()))
	   {
		   return;
	   }

	   CRect rectClient;
	   GetClientRect(rectClient);

      // Toolbar height
	   int barHeight = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

      // Snap toolbar to top, stretch grid over remainder
	   m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), barHeight, SWP_NOACTIVATE | SWP_NOZORDER);
	   m_wndPropList.SetWindowPos(NULL, rectClient.left, rectClient.top + barHeight, rectClient.Width(), rectClient.Height() - barHeight, SWP_NOACTIVATE | SWP_NOZORDER);
   }

   /// <summary>Connects a source of properties.</summary>
   /// <param name="src">The source.</param>
   /// <param name="connect">Connect or disconnect.</param>
   /// <exception cref="Logic::ArgumentNullException">Source is null</exception>
   void  CPropertiesWnd::ConnectSource(PropertiesSource* src, bool connect)
   {
      REQUIRED(src);

      // Clear 
      m_wndPropList.RemoveAll();

      // Disconnect
      if (!connect)
         Source = nullptr;
      
      // Connect + Populate
      else if (src != Source)
      {
         Source = src;
         Source->OnDisplayProperties(m_wndPropList);
      }
   }

   /// <summary>Create child controls</summary>
   /// <param name="lpCreateStruct">create params</param>
   /// <returns></returns>
   int CPropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
   {
	   if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		   return -1;

	   CRect rectDummy;
	   rectDummy.SetRectEmpty();

      // Create property grid
	   if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	   {
		   TRACE0("Failed to create Properties Grid \n");
		   return -1;      // fail to create
	   }

      SetPropListFont();
	   //InitPropList();
      m_wndPropList.EnableHeaderCtrl(FALSE);
	   m_wndPropList.EnableDescriptionArea();
	   m_wndPropList.SetVSDotNetLook();
	   m_wndPropList.MarkModifiedProperties();
      //m_wndPropList.RemoveAll();

	   m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROPERTIES);
	   m_wndToolBar.LoadToolBar(IDR_PROPERTIES, 0, 0, TRUE /* Is locked */);
	   m_wndToolBar.CleanUpLockedImages();
	   m_wndToolBar.LoadBitmap(IDR_PROPERTIES, 0, 0, TRUE /* Locked */);

	   m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	   m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	   m_wndToolBar.SetOwner(this);

	   // All commands will be routed via this control , not via the parent frame:
	   m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	   AdjustLayout();
	   return 0;
   }

   void CPropertiesWnd::OnDisplayProperties(CWnd* pWnd, PropertyTarget type)
   {
      list<ScriptObject> command_names, param_types;
      
      // Clear contents
      m_wndPropList.RemoveAll();

      // TODO: Examine document type
      ScriptDocument* doc = dynamic_cast<ScriptView*>(pWnd)->GetDocument();
      
      // Get script objects used in combo boxes
      for (const ScriptObject& obj : ScriptObjectLib)
         if (obj.Group == ScriptObjectGroup::ObjectCommand)
            command_names.push_back(obj);
         else if (obj.Group == ScriptObjectGroup::ParameterType)
            param_types.push_back(obj);

      // GENERAL
      CMFCPropertyGridProperty* general = new CMFCPropertyGridProperty(_T("General"));
      general->AddSubItem(new CMFCPropertyGridProperty(L"Name", doc->Script.Name.c_str(), L"How script is referenced throughout the game"));
      general->AddSubItem(new CMFCPropertyGridProperty(L"Description", doc->Script.Description.c_str(), L"Short description of functionality"));
      general->AddSubItem(new CMFCPropertyGridProperty(L"Version", (_variant_t)doc->Script.Version, L"Current version number"));

      // Command ID
      CMFCPropertyGridProperty* option = new CMFCPropertyGridProperty(L"Command", doc->Script.CommandName.c_str(), L"ID of ship/station command implemented by the script");
      for (const ScriptObject& obj : command_names)
         option->AddOption(obj.Text.c_str(), FALSE);
      general->AddSubItem(option);

      // EngineVersion
      option = new CMFCPropertyGridProperty(L"Game Required", VersionString(doc->Script.Game).c_str(),  L"Minimum version of game required");
      option->AddOption(VersionString(GameVersion::AlbionPrelude).c_str(), FALSE);
      option->AddOption(VersionString(GameVersion::TerranConflict).c_str(), FALSE);
      option->AddOption(VersionString(GameVersion::Reunion).c_str(), FALSE);
      option->AddOption(VersionString(GameVersion::Threat).c_str(), FALSE);
      general->AddSubItem(option);

      // Signed:
      general->AddSubItem(option = new CMFCPropertyGridProperty(L"Signed", (_variant_t)false, L"Version number"));
      option->Enable(FALSE);



      // ARGUMENTS
      CMFCPropertyGridProperty* arguments = new CMFCPropertyGridProperty(_T("Arguments"));

      for (ScriptVariable& v : doc->Script.Variables)
         if (v.Type == VariableType::Argument)
         {
            option = new CMFCPropertyGridProperty(v.Name.c_str(), _variant_t(GetString(v.ValueType).c_str()), v.Description.c_str());
            // Argument type
            for (const ScriptObject& obj : param_types)
               option->AddOption(obj.Text.c_str(), FALSE);
            arguments->AddSubItem(option);
         }

      // Add nodes
      m_wndPropList.AddProperty(general);
      m_wndPropList.AddProperty(arguments);
   }

   void CPropertiesWnd::OnExpandAllProperties()
   {
	   m_wndPropList.ExpandAll();
   }

   void CPropertiesWnd::OnProperties1()
   {
	   // TODO: Add your command handler code here
   }
   
   void CPropertiesWnd::OnProperties2()
   {
	   // TODO: Add your command handler code here
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
   
   void CPropertiesWnd::InitPropList()
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

   void CPropertiesWnd::OnUpdateExpandAllProperties(CCmdUI* /* pCmdUI */)
   {
   }

   void CPropertiesWnd::OnUpdateSortProperties(CCmdUI* pCmdUI)
   {
	   pCmdUI->SetCheck(m_wndPropList.IsAlphabeticMode());
   }

   void CPropertiesWnd::OnUpdateProperties1(CCmdUI* pCmdUI)
   {
	   pCmdUI->Enable(FALSE);
   }

   void CPropertiesWnd::OnUpdateProperties2(CCmdUI* pCmdUI)
   {
	   pCmdUI->Enable(FALSE);
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

   
   // ------------------------------- PRIVATE METHODS ------------------------------

   

NAMESPACE_END2(GUI,Windows)

