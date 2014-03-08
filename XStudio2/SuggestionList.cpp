#include "stdafx.h"
#include "SuggestionList.h"
#include "ScriptEdit.h"
#include "Logic/GameObjectLibrary.h"
#include "Logic/ScriptObjectLibrary.h"
#include "Logic/SyntaxLibrary.h"
#include "Logic/ScriptFile.h"
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

   /// <summary>Creates the specified parent.</summary>
   /// <param name="parent">The script edit</param>
   /// <param name="pt">The character position in script edit client co-ordinates</param>
   /// <param name="type">suggestion type.</param>
   /// <param name="scr">Script file.</param>
   /// <returns></returns>
   /// <exception cref="Logic::ArgumentException">Suggestion type is None</exception>
   BOOL SuggestionList::Create(ScriptEdit* parent, CPoint& pt, Suggestion type, const ScriptFile* scr)
   {
      // Validate type
      if (type == Suggestion::None)
         throw ArgumentException(HERE, L"type", L"Suggestion type cannot be 'None'");

      // Calculate position  (Offset rectangle above line)
      CSize sz = (type == Suggestion::Command ? CommandSize : DefaultSize);
      CRect rc(pt, sz);
      rc.OffsetRect(0, -sz.cy);

      // Set type
      SuggestionType = type;
      Script = scr;

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
      case Suggestion::GameObject:   return GuiString(L"{%s}", Content[GetNextItem(-1, LVNI_SELECTED)].Text.c_str());
      case Suggestion::ScriptObject: return GuiString(L"[%s]", Content[GetNextItem(-1, LVNI_SELECTED)].Text.c_str());
      case Suggestion::Variable:     return GuiString(L"$%s", Content[GetNextItem(-1, LVNI_SELECTED)].Text.c_str());
      case Suggestion::Label:        return GuiString(L"%s:", Content[GetNextItem(-1, LVNI_SELECTED)].Text.c_str());
      case Suggestion::Command:      return Content[GetNextItem(-1, LVNI_SELECTED)].Text;
      default:  return L"Error";
      }
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
         str = str.TrimLeft(L"{[");
         str = str.TrimRight(L"}]");
         break;

      case TokenType::Variable:  str = str.TrimLeft(L"$");  break;
      case TokenType::Label:     str = str.TrimRight(L":"); break;
      }

      // Linear search for partial substring
      int index = 0;
      auto it = find_if(Content.begin(), Content.end(), [&](const SuggestionItem& item)->bool { 
         return item.Key.find(str) != wstring::npos ? true : (++index, false); 
      });

      // Search/display closest match
      if (it != Content.end())
      {
         Console << L"Search for " << str << L" matched " << it->Text << ENDL;
         SetItemState(index, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
         EnsureVisible(index, FALSE);
      }
      else
         Console << L"Search for " << str << L" provided no match" << ENDL;
   }
   
   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Populates the list.</summary>
   /// <returns></returns>
   void SuggestionList::PopulateContent() 
   {
      // Clear previous
      Content.clear();
      
      // Populate
      switch (SuggestionType)
      {
      case Suggestion::GameObject:  
         for (auto& obj : GameObjectLib.Query(L""))
            Content.push_back( SuggestionItem(obj->Name, GetString(obj->Type)) );
         break;

      case Suggestion::ScriptObject:
         for (auto& obj : ScriptObjectLib.Query(L""))
            Content.push_back( SuggestionItem(obj->Text, GetString(obj->Group)) );
         break;

      case Suggestion::Variable:    
         for (auto& var : Script->Variables)
            Content.push_back( SuggestionItem(var.Name, GetString(var.Type).c_str()) );
         break;

      case Suggestion::Label:       
         for (auto& lab : Script->Labels)
            Content.push_back( SuggestionItem(lab.Name, GuiString(L"Line %d", lab.LineNumber)) );
         break;

      case Suggestion::Command: 
         for (auto& obj : SyntaxLib.Query(L"", Script->Game))
            Content.push_back( SuggestionItem(obj->DisplayText, GetString(obj->Group), obj->Hash) );
         break;
      }

      // Sort keys alphabetically
      sort(Content.begin(), Content.end(), [](SuggestionItem& a, SuggestionItem& b) {return a.Key < b.Key;} );
   }

   /// <summary>Initialises control and populates</summary>
   /// <param name="lpCreateStruct">The create structure.</param>
   /// <returns></returns>
   int SuggestionList::OnCreate(LPCREATESTRUCT lpCreateStruct)
   {
      if (CListCtrl::OnCreate(lpCreateStruct) == -1)
         return -1;
      
      // Setup control
      InsertColumn(0, L"text");
      InsertColumn(1, L"type", LVCFMT_RIGHT);
      SetColumnWidth(0, lpCreateStruct->cx-GetSystemMetrics(SM_CXVSCROLL)-120);
      SetColumnWidth(1, 120);
      SetExtendedStyle(LVS_EX_FULLROWSELECT);

      // Populate
      PopulateContent();

      // Display contents
      SetItemCountEx(Content.size());
      SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
      return 0;
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

   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
NAMESPACE_END2(GUI,Controls)



