#include "stdafx.h"
#include "SuggestionList.h"
#include "ScriptEdit.h"
#include "../Logic/GameObjectLibrary.h"
#include "../Logic/ScriptObjectLibrary.h"
#include "../Logic/SyntaxLibrary.h"
#include "../Logic/ScriptFile.h"
#include <algorithm>

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Controls)

   /// <summary>Default list sizes.</summary>
   const CSize SuggestionList::DefaultSize = CSize(300,200),
               SuggestionList::CommandSize = CSize(600,200);

   /// <summary>Visible items changed notification.</summary>
   const UINT UM_VISIBLE_CHANGED = (WM_USER+100);

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNAMIC(SuggestionList, CListCtrl)

   BEGIN_MESSAGE_MAP(SuggestionList, CListCtrl)
      ON_WM_CREATE()
      ON_WM_KILLFOCUS()
      ON_MESSAGE(UM_VISIBLE_CHANGED, &SuggestionList::OnVisibleItemsChanged)
      ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &SuggestionList::OnCustomDraw)
      ON_NOTIFY_REFLECT(LVN_GETDISPINFO, &SuggestionList::OnRetrieveItem)
      ON_NOTIFY_REFLECT(LVN_BEGINSCROLL, &SuggestionList::OnScrollBegin)
      ON_NOTIFY_REFLECT(LVN_KEYDOWN, &SuggestionList::OnKeyDown)
      ON_WM_LBUTTONDBLCLK()
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   /// <summary>Initializes a new instance of the <see cref="SuggestionList"/> class.</summary>
   SuggestionList::SuggestionList() : CustomDraw(*this)
   {
      // Highlight selected item whether focused or not
      CustomDraw.ActiveHighlight = GetSysColor(COLOR_HIGHLIGHT);
      CustomDraw.InactiveHighlight = GetSysColor(COLOR_HIGHLIGHT);
   }

   SuggestionList::~SuggestionList()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   /// <summary>Gets the default list size for suggestions.</summary>
   /// <param name="type">The type.</param>
   /// <returns></returns>
   CSize  SuggestionList::GetDefaultSize(Suggestion type)
   {
      switch (type)
      {
      case Suggestion::Command:
         return CommandSize;

      default:
         return DefaultSize;
      }
   }

   // ------------------------------- PUBLIC METHODS -------------------------------

   /// <summary>Creates the specified parent.</summary>
   /// <param name="parent">The script edit</param>
   /// <param name="rc">Initial display rectangle</param>
   /// <param name="type">suggestion type.</param>
   /// <param name="scr">Script file.</param>
   /// <returns></returns>
   /// <exception cref="Logic::ArgumentException">Suggestion type is None</exception>
   BOOL SuggestionList::Create(ScriptEdit* parent, CRect rc, Suggestion type, const ScriptFile* scr)
   {
      // Validate type
      if (type == Suggestion::None)
         throw ArgumentException(HERE, L"type", L"Suggestion type cannot be 'None'");

      // Set type
      SuggestionType = type;
      Script = scr;

      // Create
      DWORD style = WS_CHILD|WS_VISIBLE|WS_BORDER|LVS_REPORT|LVS_OWNERDATA|LVS_SHOWSELALWAYS|LVS_SINGLESEL|LVS_NOCOLUMNHEADER;
      return __super::Create(style, rc, parent, IDC_SUGGESTION_LIST);
   }
   
   /// <summary>Finds the index of the item with the longest match</summary>
   /// <param name="txt">text to match.</param>
   /// <returns>Zero-based index if successful, otherwise -1</returns>
   int SuggestionList::ContentArray::FindMatch(const wstring& txt) const
   {
      int index = 0,
          value_max = 0,
          index_max = -1;

      // Linear search for longest match
      for (auto& item : *this)
      {
         auto m = item.Match(txt);

         // Update longest match
         if (m > value_max)
         {
            value_max = m;
            index_max = index;
         }
         
         ++index;
      }

      // Return index of longest match
      return index_max;

      
      /*list<int> matches;
      transform(begin(), end(), matches, [&txt](const SuggestionItem& it){return it.Match(txt);} );

      return max_element(matches.begin(), matches.end()) - matches.begin();*/
   }

   /// <summary>Gets the script edit parent</summary>
   /// <returns></returns>
   ScriptEdit* SuggestionList::GetParent() const
   {
      return dynamic_cast<ScriptEdit*>(__super::GetParent());
   }

   /// <summary>Gets the text of the selected suggestion.</summary>
   /// <returns></returns>
   /// <exception cref="Logic::InvalidOperationException">No item selected</exception>
   /// <exception cref="Logic::NotImplementedException">Command selected</exception>
   wstring SuggestionList::GetSelected() const
   {
      // Ensure exists
      if (GetNextItem(-1, LVNI_SELECTED) == -1)
         throw InvalidOperationException(HERE, L"No item selected");

      // Get selection and format
      switch (SuggestionType)
      {
      case Suggestion::GameObject:   return VString(L"{%s}", Content[GetNextItem(-1, LVNI_SELECTED)].Text.c_str());
      case Suggestion::ScriptObject: return VString(L"[%s]", Content[GetNextItem(-1, LVNI_SELECTED)].Text.c_str());
      case Suggestion::Variable:     return VString(L"$%s", Content[GetNextItem(-1, LVNI_SELECTED)].Text.c_str());
      case Suggestion::Label:        return VString(L"%s:", Content[GetNextItem(-1, LVNI_SELECTED)].Text.c_str());
      case Suggestion::Command:      return Content[GetNextItem(-1, LVNI_SELECTED)].Text;
      default:  return L"Error";
      }
   }
   
   /// <summary>Calculates the length of a match between item and a string</summary>
   /// <param name="txt">The text.</param>
   /// <returns>Length of match in characters</returns>
   int SuggestionList::SuggestionItem::Match(const wstring& txt) const
   {
      int len = 0;

      // Compare 'n' characters of input + key
      for (auto t = txt.begin(), k = Key.begin(); t != txt.end() && k != Key.end(); ++t, ++k)
         if (*t == *k)
            ++len;

      // Return count
      return len;
   }

   /// <summary>Highlights the closest matching suggestion.</summary>
   /// <param name="tok">Token to match</param>
   void  SuggestionList::MatchSuggestion(const ScriptToken& tok)
   {
      // Exclude {,[,$,: etc.
      GuiString str(tok.ValueText); 

      // Linear search for partial substring
      int index = Content.FindMatch(str);

      // Search/display closest match
      if (index != -1)
      {
         //Console << L"Search for " << str << L" matched " << Content[index].Text << ENDL;
         SetItemState(index, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
         EnsureVisible(index, FALSE);

         // Raise 'Visible Items Changed'
         OnVisibleItemsChanged();
      }
      /*else
         Console << L"Search for " << str << L" provided no match" << ENDL;*/
   }
   
   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Initialises control and populates</summary>
   /// <param name="lpCreateStruct">The create structure.</param>
   /// <returns></returns>
   int SuggestionList::OnCreate(LPCREATESTRUCT lpCreateStruct)
   {
      try
      {
         if (__super::OnCreate(lpCreateStruct) == -1)
            throw Win32Exception(HERE, L"Unable to create base ListView");
      
         // Display items using a single column. Custom Draw handles the column illusion
         InsertColumn(0, L"text");
         SetColumnWidth(0, lpCreateStruct->cx);
         SetExtendedStyle(LVS_EX_FULLROWSELECT);

         // Populate
         PopulateContent();

         // Ensure we have content
         if (Content.size() == 0)
            throw AlgorithmException(HERE, L"Unable to create list of zero suggestions");

         // Display contents
         SetItemCountEx(Content.size());
         SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);

         // Shrink to fit
         ShrinkToFit();
         OnVisibleItemsChanged();
         return 0;
      }
      catch (ExceptionBase& e)
      {
         Console.Log(HERE, e);
         return -1;
      }
   }
   
   /// <summary>Custom draw the items</summary>
   /// <param name="pNMHDR">header.</param>
   /// <param name="pResult">result.</param>
   void SuggestionList::OnCustomDraw(NMHDR *pNMHDR, LRESULT *pResult)
   {
      NMCUSTOMDRAW* pDraw = reinterpret_cast<NMCUSTOMDRAW*>(pNMHDR);
      *pResult = CustomDraw.OnCustomDraw(pDraw);
   }
   
   /// <summary>Custom draws the items</summary>
   /// <param name="dc">Device context</param>
   /// <param name="item">Item data.</param>
   void  SuggestionList::SuggestionCustomDraw::onDrawSubItem(CDC* dc, ItemData& item) 
   {
      try
      {
         // Get item
         auto data = reinterpret_cast<SuggestionList&>(ListView).Content[item.Index];

         // Measure both items
         CSize txt = dc->GetTextExtent(data.Text.c_str()),
               type = dc->GetTextExtent(data.Type.c_str());

         // TEXT: Truncate if necessary   [Indicates window has shrunk to maximum extent]
         if (txt.cx + type.cx > item.Rect.Width())
         {
            CRect rc(item.Rect.left, item.Rect.top, item.Rect.Width()-type.cx, item.Rect.bottom);
            dc->DrawText(data.Text.c_str(), rc, DT_LEFT|DT_SINGLELINE|DT_END_ELLIPSIS);
         }
         else 
            dc->DrawText(data.Text.c_str(), item.Rect, DT_LEFT|DT_SINGLELINE);


         // Draw type in grey
         if (!item.Selected)
            dc->SetTextColor(GetSysColor(COLOR_GRAYTEXT));

         // TYPE: RHS. Don't Truncate. 
         dc->DrawText(data.Type.c_str(), item.Rect, DT_RIGHT|DT_SINGLELINE);
      }
      catch (ExceptionBase& e) {
         Console.Log(HERE, e);
      }
   }

   
   /// <summary>Raise 'Visible Items Changed' when user scrolls with the keyboard</summary>
   /// <param name="pNMHDR">notify NMHDR.</param>
   /// <param name="pResult">notify result.</param>
   void SuggestionList::OnKeyDown(NMHDR *pNMHDR, LRESULT *pResult)
   {
      auto data = reinterpret_cast<NMLVKEYDOWN*>(pNMHDR);
      
      //Console << "SuggestionList::OnKeyDown" << ENDL;

      // Trap scroll keys
      switch (data->wVKey)
      {
      case VK_UP:
      case VK_DOWN:
      case VK_PRIOR:
      case VK_NEXT:
      case VK_HOME:
      case VK_END:
         // Raise 'Visible Items Changed' after key processed
         PostMessage(UM_VISIBLE_CHANGED);
         break;
      }

      *pResult = 0;
   }
   
   /// <summary>Destroys self if focus to lost</summary>
   /// <param name="pNewWnd">The new WND.</param>
   void SuggestionList::OnKillFocus(CWnd* pNewWnd)
   {
      __super::OnKillFocus(pNewWnd);

      // Inform suggestions mediator 
      GetParent()->Suggestions.OnKillFocus(pNewWnd);
   }

   /// <summary>Inform suggestions mediator when user double-clicks an item</summary>
   /// <param name="nFlags">flags.</param>
   /// <param name="point">point.</param>
   void SuggestionList::OnLButtonDblClk(UINT nFlags, CPoint point)
   {
      UINT flags = LVHT_ONITEM;
      int item = HitTest(point, &flags);

      // Inform suggestions mediator
      if (item != -1)
         GetParent()->Suggestions.OnItemDoubleClick(item);

      __super::OnLButtonDblClk(nFlags, point);
   }


   /// <summary>Supplies virtual list item</summary>
   /// <param name="pNMHDR">notify NMHDR.</param>
   /// <param name="pResult">notify result.</param>
   void SuggestionList::OnRetrieveItem(NMHDR *pNMHDR, LRESULT *pResult)
   {
      LVITEM& item = reinterpret_cast<NMLVDISPINFO*>(pNMHDR)->item;
      
      // Supply text/type
      if (item.mask & LVIF_TEXT)
      {
         const wstring& txt = (item.iSubItem==0 ? Content[item.iItem].Text : Content[item.iItem].Type);
         item.pszText = (WCHAR*)txt.c_str();
      }

      *pResult = 0;
   }
   
   /// <summary>Raise 'Visible Items Changed' when user scrolls</summary>
   /// <param name="pNMHDR">notify NMHDR.</param>
   /// <param name="pResult">notify result.</param>
   void SuggestionList::OnScrollBegin(NMHDR *pNMHDR, LRESULT *pResult)
   {
      auto data = reinterpret_cast<NMLVSCROLL*>(pNMHDR);
      
      //Console << "SuggestionList::OnScrollBegin" << ENDL;
      
      // Raise 'Visible Items Changed' after scroll processed
      PostMessage(UM_VISIBLE_CHANGED);

      *pResult = 0;
   }
   
   /// <summary>Resize window to fit current items</summary>
   /// <param name="wParam">Ignored.</param>
   /// <param name="lParam">Ignored.</param>
   LRESULT SuggestionList::OnVisibleItemsChanged(WPARAM wParam, LPARAM lParam)
   {
      // DEBUG:
      //Console << "SuggestionList::OnVisibleItemsChanged()" << ENDL;

      CWindowDC dc(this);
      WindowRect wnd(this);
      int width = 0;

      // Measure visible items
      for (int index = GetTopIndex(), end = min(GetTopIndex()+GetCountPerPage(), (int)Content.size()); index < end; ++index)
      {
         auto& item = Content[index];
         auto w = dc.GetTextExtent(item.Text.c_str()).cx + dc.GetTextExtent(item.Type.c_str()).cx + 10;
         width = max(w, width);
      }

      // Enforce min/max widths
      width = max(width, 200);
      width = min(width, 700);
      
      // Adjust for scrollBar
      auto wndWidth = width + 2*GetSystemMetrics(SM_CXEDGE);
      if (GetCountPerPage() < (int)Content.size())
         wndWidth += GetSystemMetrics(SM_CXVSCROLL);

      // Resize window + column
      SetWindowPos(nullptr,-1,-1, wndWidth, wnd.Height(), SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);
      SetColumnWidth(0, width);

      return 0;
   }
   
   /// <summary>Populates the list.</summary>
   /// <returns></returns>
   void SuggestionList::PopulateContent() 
   {
      // Clear previous
      Content.clear();
      
      // Populate
      switch (SuggestionType)
      {
      // Query GameObjectLibrary 
      case Suggestion::GameObject:  
         for (auto& obj : GameObjectLib.Query(L""))
            Content.push_back( SuggestionItem(obj->Name, GetString(obj->Type)) );
         break;

      // Query ScriptObjectLibrary 
      case Suggestion::ScriptObject:
         for (auto& obj : ScriptObjectLib.Query(L""))
            Content.push_back( SuggestionItem(obj->Text, GetString(obj->Group)) );
         break;

      // Query ScriptFile
      case Suggestion::Variable:    
         for (auto& var : Script->Variables)
            Content.push_back( SuggestionItem(var.Name, GetString(var.Type).c_str()) );
         break;

      // Query ScriptFile
      case Suggestion::Label:       
         for (auto& lab : Script->Labels)
            Content.push_back( SuggestionItem(lab.Name, VString(L"Line %d", lab.LineNumber)) );
         break;

      // Query SyntaxLibrary
      case Suggestion::Command: 
         for (auto& obj : SyntaxLib.Query(L"", Script->Game))
            Content.push_back( SuggestionItem(obj->DisplayText, GetString(obj->Group), obj->Hash) );
         break;
      }

      // Sort keys alphabetically
      sort(Content.begin(), Content.end(), [](SuggestionItem& a, SuggestionItem& b) {return a.Key < b.Key;} );
   }
   
   /// <summary>Shrinks to fit.</summary>
   void  SuggestionList::ShrinkToFit()
   {
      try
      {
         // Check if less than 1 page of items
         if (GetCountPerPage() > (int)Content.size() && !Content.empty())
         {
            ClientRect wnd(this);
            CRect rc(0,0,0,0);

            // Calculate item height
            if (!GetItemRect(0, &rc, LVIR_LABEL) || !rc.Height())
               throw Win32Exception(HERE, L"Unable to retrieve item height");

            // Resize
            SetWindowPos(nullptr,-1,-1, wnd.Width(), rc.Height()*Content.size(), SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);
         }
      }
      catch (ExceptionBase& e) { 
         Console.Log(HERE, e);
      }
   }
   

   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
NAMESPACE_END2(GUI,Controls)




