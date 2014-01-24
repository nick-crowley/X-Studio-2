#include "stdafx.h"
#include "SuggestionList.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Controls)

   #define CTRL_ID   666

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNAMIC(SuggestionList, CListCtrl)

   BEGIN_MESSAGE_MAP(SuggestionList, CListCtrl)
      ON_WM_SIZE()
      ON_WM_CREATE()
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   SuggestionList::SuggestionList()
   {
   }

   SuggestionList::~SuggestionList()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   #ifdef _DEBUG
   void SuggestionList::AssertValid() const
   {
	   CListCtrl::AssertValid();
   }
   
   void SuggestionList::Dump(CDumpContext& dc) const
   {
	   CListCtrl::Dump(dc);
   }
   #endif //_DEBUG

   BOOL SuggestionList::Create(CWnd* parent, CPoint& pt)
   {
      // Calc position
      CRect rc(pt, DefaultSize);
      rc.OffsetRect(0, -DefaultSize.cy);

      // Create
      return CListCtrl::Create(WS_CHILD|WS_VISIBLE|WS_BORDER|LVS_REPORT|LVS_SHOWSELALWAYS|LVS_SINGLESEL|LVS_NOCOLUMNHEADER, rc, parent, CTRL_ID);
   }
   
   // ------------------------------ PROTECTED METHODS -----------------------------
   
   void  SuggestionList::AdjustLayout()
   {
      // Destroyed/Minimised
	   if (GetSafeHwnd() == nullptr || (AfxGetMainWnd() != nullptr && AfxGetMainWnd()->IsIconic()))
         return;
         
      CRect wnd;
      GetClientRect(wnd);

      // TODO: Layout code
   }
   
   int SuggestionList::OnCreate(LPCREATESTRUCT lpCreateStruct)
   {
      if (CListCtrl::OnCreate(lpCreateStruct) == -1)
         return -1;

      // Setup columns
      InsertColumn(0, L"text");
      SetColumnWidth(0, DefaultSize.cx - GetSystemMetrics(SM_CXVSCROLL));

      // Setup style
      SetExtendedStyle(LVS_EX_FULLROWSELECT);

      // Populate somehow
      for (int i = 0; i < 50; ++i)
         InsertItem(i, GuiString(L"Variable #%03d", i).c_str());

      return 0;
   }

   void SuggestionList::OnSize(UINT nType, int cx, int cy)
   {
      CListCtrl::OnSize(nType, cx, cy);
      AdjustLayout();
   }
   
   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
NAMESPACE_END2(GUI,Controls)

