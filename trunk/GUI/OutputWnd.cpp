#include "stdafx.h"

#include "OutputList.h"
#include "MainWnd.h"
#include "Helpers.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)

   /// <summary>Child window IDs</summary>
   const UINT  IDC_OUTPUT_TAB = 1,
               IDC_GAMEDATA_LIST = 2,
               IDC_OUTPUT_LIST = 3,
               IDC_FIND1_LIST = 4,
               IDC_FIND2_LIST = 5;

   // --------------------------------- APP WIZARD ---------------------------------
   
   BEGIN_MESSAGE_MAP(COutputWnd, CDockablePane)
	   ON_WM_CREATE()
      ON_WM_SETTINGCHANGE()
	   ON_WM_SIZE()
   END_MESSAGE_MAP()

   // -------------------------------- CONSTRUCTION --------------------------------

   COutputWnd::COutputWnd() : fnGameDataFeedback(MainWnd::GameDataFeedback.Register(this, &COutputWnd::OnGameDataFeedback)),
                              fnFindReplaceFeedback(MainWnd::FindReplaceFeedback.Register(this, &COutputWnd::onFindReplaceFeedback)),
                              fnLoadSaveFeedback(MainWnd::LoadSaveFeedback.Register(this, &COutputWnd::onLoadSaveFeedback))
   {
   }

   COutputWnd::~COutputWnd()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------
   
   /// <summary>Activates a pane.</summary>
   /// <param name="op">The pane.</param>
   void COutputWnd::ActivatePane(Operation op)
   {
      switch (op)
      {
      case Operation::LoadGameData:      TabCtrl.SetActiveTab(0);  break;
      case Operation::ImportProject:     
      case Operation::LoadSaveDocument:  TabCtrl.SetActiveTab(1);  break;
      case Operation::FindAndReplace1:   TabCtrl.SetActiveTab(2);  break;
      case Operation::FindAndReplace2:   TabCtrl.SetActiveTab(3);  break;
      }
   }

   /// <summary>Creates the window.</summary>
   /// <param name="parent">The parent.</param>
   /// <exception cref="Logic::Win32Exception">Unable to create window</exception>
   void COutputWnd::Create(CWnd* parent)
   {
      DWORD style = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI;

      // Create window
      if (!__super::Create(GuiString(IDR_OUTPUT).c_str(), parent, MainWnd::DefaultSize, TRUE, IDR_OUTPUT, style))
         throw Win32Exception(HERE, L"Unable to create output window");
      SetIcon(theApp.LoadIconW(IDR_OUTPUT, ::GetSystemMetrics(SM_CXSMICON)), FALSE);

      // Dock bottom only
      EnableDocking(CBRS_ALIGN_BOTTOM);
   }

   /// <summary>Clears a pane.</summary>
   void COutputWnd::ClearPane(Operation op)
   {
      switch (op)
      {
      case Operation::LoadGameData:      GameDataList.Clear();  break;
      case Operation::ImportProject:     
      case Operation::LoadSaveDocument:  OutputList.Clear();    break;
      case Operation::FindAndReplace1:   FindList1.Clear();     break;
      case Operation::FindAndReplace2:   FindList2.Clear();     break;
      }
   }

   // ------------------------------ PROTECTED METHODS -----------------------------
   
#ifdef RESCINDED
   /// <summary>Adjusts the column headers to fit the length of item text. (AppWizard Generated)</summary>
   /// <param name="wndListBox">ListBox.</param>
   void COutputWnd::AdjustHorzScroll(CListBox& wndListBox)
   {
	   CClientDC dc(this);
	   CFont* pOldFont = dc.SelectObject(&afxGlobalData.fontRegular);

	   int cxExtentMax = 0;

	   for (int i = 0; i < wndListBox.GetCount(); i++)
	   {
		   CString strItem;
		   wndListBox.GetText(i, strItem);

		   cxExtentMax = max(cxExtentMax, (int)dc.GetTextExtent(strItem).cx);
	   }

	   wndListBox.SetHorizontalExtent(cxExtentMax);
	   dc.SelectObject(pOldFont);
   }
#endif

   /// <summary>Create window.</summary>
   /// <param name="lpCreateStruct">The lp create structure.</param>
   /// <returns></returns>
   int COutputWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
   {
      try
      {
         // Create base
	      if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		      throw Win32Exception(HERE, L"Unable to create output window dockable pane");

	      CRect rectDummy;
	      rectDummy.SetRectEmpty();

	      // Create tabs window:
	      if (!TabCtrl.Create(CMFCTabCtrl::STYLE_FLAT, rectDummy, this, IDC_OUTPUT_TAB))
            throw Win32Exception(HERE, L"Unable to create output window tab control");
	      
         // Setup ImageList:
         Images.Create(IDB_OUTPUT_ICONS, 16, 6, RGB(255,0,255));

	      // Create output panes:
	      const DWORD dwStyle = LVS_REPORT | LVS_NOCOLUMNHEADER | LVS_SHAREIMAGELISTS | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL;

         // Create output panes:
         GameDataList.Create(&TabCtrl, dwStyle, IDC_GAMEDATA_LIST, &Images);
         OutputList.Create(&TabCtrl, dwStyle, IDC_OUTPUT_LIST, &Images);
         FindList1.Create(&TabCtrl, dwStyle, IDC_FIND1_LIST, &Images);
         FindList2.Create(&TabCtrl, dwStyle, IDC_FIND2_LIST, &Images);

	      // Attach list windows to tab:
	      TabCtrl.AddTab(&GameDataList, L"Game Data", 0);
	      TabCtrl.AddTab(&OutputList, L"Compiler", 1);
	      TabCtrl.AddTab(&FindList1, L"Find Results 1", 2);
         TabCtrl.AddTab(&FindList2, L"Find Results 2", 3);

         // Set fonts
         UpdateFonts();
	      return 0;
      }
      catch (ExceptionBase& e)
      {
         Console.Log(HERE, e);
         return -1;
      }
   }
   
   /// <summary>Displays find replace feedback.</summary>
   /// <param name="wp">The wp.</param>
   void COutputWnd::onFindReplaceFeedback(const WorkerProgress& wp)
   {
      auto findList = (wp.Operation == Operation::FindAndReplace1 ? &FindList1 : &FindList2);

      // New operation: clear previous content
      if (wp.Type == ProgressType::Operation)
         findList->DeleteAllItems();

      // Insert item
      findList->InsertItem(wp);
   }

   /// <summary>Displays game data feedback.</summary>
   /// <param name="wp">The wp.</param>
   void COutputWnd::OnGameDataFeedback(const WorkerProgress& wp)
   {
      // Insert item
      GameDataList.InsertItem(wp);
   }

   /// <summary>Displays document load/save feedback.</summary>
   /// <param name="wp">The wp.</param>
   void COutputWnd::onLoadSaveFeedback(const WorkerProgress& wp)
   {
      // Insert item
      OutputList.InsertItem(wp);
   }

   /// <summary>Updates window font.</summary>
   /// <param name="uFlags">The u flags.</param>
   /// <param name="lpszSection">The LPSZ section.</param>
   void COutputWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
   {
      // Update fonts
      UpdateFonts();
   }
   
   /// <summary>Adjusts layout</summary>
   /// <param name="nType">Type of the n.</param>
   /// <param name="cx">The width.</param>
   /// <param name="cy">The height.</param>
   void COutputWnd::OnSize(UINT nType, int cx, int cy)
   {
	   CDockablePane::OnSize(nType, cx, cy);

	   // Tab control should cover the whole client area:
	   TabCtrl.SetWindowPos (NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
   }
   
   /// <summary>Updates the fonts.</summary>
   void COutputWnd::UpdateFonts()
   {
      // Update fonts
      TabCtrl.SetFont(&theApp.ToolWindowFont);
   }

   // ------------------------------- PRIVATE METHODS ------------------------------
   
   

NAMESPACE_END2(GUI,Windows)


