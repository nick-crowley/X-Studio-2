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
      // Calculate position  (Offset rectangle above line)
      CRect rc(pt, DefaultSize);
      rc.OffsetRect(0, -DefaultSize.cy);

      // Create
      DWORD style = WS_CHILD|WS_VISIBLE|WS_BORDER|LVS_REPORT|LVS_OWNERDATA|LVS_SHOWSELALWAYS|LVS_SINGLESEL|LVS_NOCOLUMNHEADER;
      return CListCtrl::Create(style, rc, parent, CTRL_ID);
   }

   /// <summary>Gets the script edit parent</summary>
   /// <returns></returns>
   ScriptEdit* SuggestionList::GetParent() const
   {
      return dynamic_cast<ScriptEdit*>(CListCtrl::GetParent());
   }

   /// <summary>Gets the text of the selected suggestion.</summary>
   /// <returns></returns>
   wstring SuggestionList::GetSuggestion() const
   {
      if (GetNextItem(-1, LVNI_SELECTED) == -1)
         throw InvalidOperationException(HERE, L"No item selected");

      // Get selected
      return Content[GetNextItem(-1, LVNI_SELECTED)];
   }

   /// <summary>Highlights the closest matching suggestion.</summary>
   /// <param name="tok">Token to match</param>
   void  SuggestionList::MatchSuggestion(const ScriptToken& tok)
   {
      GuiString str(tok.Text);

      // Format token text
      switch (tok.Type)
      {
      case TokenType::GameObject:
      case TokenType::ScriptObject:
      case TokenType::Variable:
         str = str.TrimLeft(L"${[");
         str = str.TrimRight(L"}]");
         break;
      }

      // Linear search for partial substring
      auto it = find_if(Content.begin(), Content.end(), [&str](const wstring& s)->bool { return s.find(str) != wstring::npos; });

      // Select and display item
      if (it != Content.end())
      {
         Console << L"Search for " << str << L" matched " << *it << ENDL;
         UINT item = it-Content.begin();
         SetItemState(item, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
         EnsureVisible(item, FALSE);
      }
      else
         Console << L"Search for " << str << L" provided no match" << ENDL;
   }
   
   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Initialises control and populates</summary>
   /// <param name="lpCreateStruct">The create structure.</param>
   /// <returns></returns>
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

   /// <summary>Populates the list.</summary>
   /// <returns></returns>
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

      // Return count
      return Content.size();
   }
   
   /// <summary>Supplies virtual list item</summary>
   /// <param name="pNMHDR">notify NMHDR.</param>
   /// <param name="pResult">notify result.</param>
   void SuggestionList::OnRetrieveItem(NMHDR *pNMHDR, LRESULT *pResult)
   {
      LVITEM& item = reinterpret_cast<NMLVDISPINFO*>(pNMHDR)->item;
      
      // Supply text
      item.pszText = (WCHAR*)Content[item.iItem].c_str();

      *pResult = 0;
   }

   /// <summary>Destroys self if focus to lost</summary>
   /// <param name="pNewWnd">The new WND.</param>
   void SuggestionList::OnKillFocus(CWnd* pNewWnd)
   {
      CListCtrl::OnKillFocus(pNewWnd);

      // Close if focus lost to anything but parent
      if (pNewWnd != GetParent())
         GetParent()->CloseSuggestions();
   }

   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
NAMESPACE_END2(GUI,Controls)



