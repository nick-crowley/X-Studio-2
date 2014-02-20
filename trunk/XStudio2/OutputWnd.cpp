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

   // --------------------------------- APP WIZARD ---------------------------------
   
   BEGIN_MESSAGE_MAP(COutputWnd, CDockablePane)
	   ON_WM_CREATE()
	   ON_WM_SIZE()
   END_MESSAGE_MAP()

   // -------------------------------- CONSTRUCTION --------------------------------

   COutputWnd::COutputWnd() : fnGameDataFeedback(MainWnd::GameDataFeedback.Register(this, &COutputWnd::onGameDataFeedback)),
                              fnFindReplaceFeedback(MainWnd::FindReplaceFeedback.Register(this, &COutputWnd::onFindReplaceFeedback)),
                              fnLoadSaveFeedback(MainWnd::LoadSaveFeedback.Register(this, &COutputWnd::onLoadSaveFeedback))
   {
      
   }

   COutputWnd::~COutputWnd()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------
   
   void COutputWnd::UpdateFonts()
   {
	   
   }

   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Activates a pane.</summary>
   /// <param name="op">The pane.</param>
   void COutputWnd::ActivatePane(Operation op)
   {
      TabCtrl.SetActiveTab((int)op);
   }

   /// <summary>REM: Adjusts the column headers to fit the length of item text.</summary>
   /// <param name="wndListBox">ListBox.</param>
   void COutputWnd::AdjustHorzScroll(CListBox& wndListBox)
   {
	   CClientDC dc(this);
	   CFont* pOldFont = dc.SelectObject(&afxGlobalData.fontRegular);

	   int cxExtentMax = 0;

	   for (int i = 0; i < wndListBox.GetCount(); i ++)
	   {
		   CString strItem;
		   wndListBox.GetText(i, strItem);

		   cxExtentMax = max(cxExtentMax, (int)dc.GetTextExtent(strItem).cx);
	   }

	   wndListBox.SetHorizontalExtent(cxExtentMax);
	   dc.SelectObject(pOldFont);
   }

   int COutputWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
   {
      try
      {
	      if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		      throw Win32Exception(HERE, L"Unable to create output window dockable pane");

	      CRect rectDummy;
	      rectDummy.SetRectEmpty();

	      // Create tabs window:
	      if (!TabCtrl.Create(CMFCTabCtrl::STYLE_FLAT, rectDummy, this, 1))
            throw Win32Exception(HERE, L"Unable to create output window tab control");
	      
         // Setup ImageList:
         Images.Create(IDB_OUTPUT_ICONS, 16, 6, RGB(255,0,255));

	      // Create output panes:
	      const DWORD dwStyle = LVS_REPORT | LVS_NOCOLUMNHEADER | LVS_SHAREIMAGELISTS | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL;

         // Create output panes:
         UINT nID = 2;
         for (CListCtrl* c : { &GameDataList, &OutputList, &FindList1, &FindList2 })
         {
            if (!c->CreateEx(LVS_EX_FULLROWSELECT, dwStyle, rectDummy, &TabCtrl, nID++))
               throw Win32Exception(HERE, L"Unable to create output window listview");

            c->SetImageList(&Images, LVSIL_SMALL);
         }

	      // Attach list windows to tab:
	      TabCtrl.AddTab(&GameDataList, L"Game Data", (UINT)0);
	      TabCtrl.AddTab(&OutputList, L"Compiler", (UINT)1);
	      TabCtrl.AddTab(&FindList1, L"Find Results 1", (UINT)2);
         TabCtrl.AddTab(&FindList2, L"Find Results 2", (UINT)3);
	      return 0;
      }
      catch (ExceptionBase& e)
      {
         Console.Log(HERE, e);
         return -1;
      }
   }
   
   void COutputWnd::onFindReplaceFeedback(const WorkerProgress& wp)
   {
      auto findList = (wp.Operation == Operation::FindAndReplace1 ? &FindList1 : &FindList2);

      // New operation: clear previous content
      if (wp.Type == ProgressType::Operation)
         findList->DeleteAllItems();

      // Insert item
      findList->InsertItem(wp);
   }

   void COutputWnd::onGameDataFeedback(const WorkerProgress& wp)
   {
      // Insert item
      GameDataList.InsertItem(wp);
   }

   void COutputWnd::onLoadSaveFeedback(const WorkerProgress& wp)
   {
      // Insert item
      OutputList.InsertItem(wp);
   }

   void COutputWnd::OnSize(UINT nType, int cx, int cy)
   {
	   CDockablePane::OnSize(nType, cx, cy);

	   // Tab control should cover the whole client area:
	   TabCtrl.SetWindowPos (NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
   }

   // ------------------------------- PRIVATE METHODS ------------------------------
   
   

NAMESPACE_END2(GUI,Windows)


