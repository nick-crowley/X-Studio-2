#include "stdafx.h"
#include "SuggestionList.h"
#include "ScriptEdit.h"
#include <algorithm>

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Controls)

   #define CTRL_ID   666

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNAMIC(SuggestionList, CListCtrl)

   BEGIN_MESSAGE_MAP(SuggestionList, CListCtrl)
      ON_WM_SIZE()
      ON_WM_CREATE()
      ON_WM_KILLFOCUS()
      ON_NOTIFY_REFLECT(LVN_GETDISPINFO, &SuggestionList::OnRetrieveItem)
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
      return CListCtrl::Create(WS_CHILD|WS_VISIBLE|WS_BORDER|LVS_REPORT|LVS_OWNERDATA|LVS_SHOWSELALWAYS|LVS_SINGLESEL|LVS_NOCOLUMNHEADER, rc, parent, CTRL_ID);
   }

   ScriptEdit* SuggestionList::GetParent() const
   {
      return dynamic_cast<ScriptEdit*>(CListCtrl::GetParent());
   }

   wstring SuggestionList::GetSuggestion() const
   {
      if (GetNextItem(-1, LVNI_SELECTED) == -1)
         throw InvalidOperationException(HERE, L"No item selected");

      return Content[GetNextItem(-1, LVNI_SELECTED)];
   }

   void  SuggestionList::MatchSuggestion(const wstring& txt)
   {
      // Linear search for partial substring
      auto it = find_if(Content.begin(), Content.end(), [&txt](const wstring& s)->bool { return s.find(txt) != wstring::npos; });

      // Select and display item
      if (it != Content.end())
      {
         UINT item = it-Content.begin();
         SetItemState(item, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
         EnsureVisible(item, FALSE);
      }
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
      
      // Setup control
      InsertColumn(0, L"text");
      SetColumnWidth(0, DefaultSize.cx - GetSystemMetrics(SM_CXVSCROLL));
      SetExtendedStyle(LVS_EX_FULLROWSELECT);

      // Populate 
      SetItemCountEx(Populate());
      SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
      return 0;
   }

   int SuggestionList::Populate()
   {
      // Clear previous (if any)
      Content.clear();

      // Populate with false data
      Content.push_back(L"argon");
      Content.push_back(L"boron");
      Content.push_back(L"oxygen");
      Content.push_back(L"calcium");
      Content.push_back(L"sodium");
      Content.push_back(L"monotonium");
      Content.push_back(L"mercury");
      Content.push_back(L"gold");

      // Sort contents
      sort(Content.begin(), Content.end());

      /*for (int i = 0; i < 50; ++i)
         Content.push_back(GuiString(L"Variable #%03d", i));*/

      // Return count
      return Content.size();
   }
   
   void SuggestionList::OnRetrieveItem(NMHDR *pNMHDR, LRESULT *pResult)
   {
      LVITEM& item = reinterpret_cast<NMLVDISPINFO*>(pNMHDR)->item;
      
      // Supply text
      item.pszText = (WCHAR*)Content[item.iItem].c_str();

      *pResult = 0;
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



