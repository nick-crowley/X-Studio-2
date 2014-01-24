#include "stdafx.h"
#include "SuggestionList.h"
#include "ScriptEdit.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Controls)

   #define CTRL_ID   666

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNAMIC(SuggestionList, CListCtrl)

   BEGIN_MESSAGE_MAP(SuggestionList, CListCtrl)
      ON_WM_SIZE()
      ON_WM_CREATE()
      ON_WM_KILLFOCUS()
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

   BOOL SuggestionList::Create(ScriptEdit* parent, CPoint& pt)
   {
      // Calc position
      CRect rc(pt, DefaultSize);
      rc.OffsetRect(0, -DefaultSize.cy);

      // Create
      return CListCtrl::Create(WS_CHILD|WS_VISIBLE|WS_BORDER|LVS_REPORT|LVS_SHOWSELALWAYS|LVS_SINGLESEL|LVS_NOCOLUMNHEADER, rc, parent, CTRL_ID);
   }

   ScriptEdit* SuggestionList::GetParent() const
   {
      return dynamic_cast<ScriptEdit*>(CListCtrl::GetParent());
   }

   void  SuggestionList::MatchSuggestion(const wstring& txt)
   {
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
      
      // Select first item?
      SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
      return 0;
   }
   
   void SuggestionList::OnKillFocus(CWnd* pNewWnd)
   {
      CListCtrl::OnKillFocus(pNewWnd);

      // Close if focus lost to anything but parent
      if (pNewWnd != GetParent())
         GetParent()->CloseSuggestions();
   }

   void SuggestionList::OnSize(UINT nType, int cx, int cy)
   {
      CListCtrl::OnSize(nType, cx, cy);
      AdjustLayout();
   }
   
   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
NAMESPACE_END2(GUI,Controls)


