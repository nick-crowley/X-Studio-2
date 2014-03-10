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

   /// <summary>Default list sizes.</summary>
   const CSize SuggestionList::DefaultSize = CSize(300,200),
               SuggestionList::CommandSize = CSize(600,200);

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNAMIC(SuggestionList, CListCtrl)

   BEGIN_MESSAGE_MAP(SuggestionList, CListCtrl)
      ON_WM_CREATE()
      ON_WM_KILLFOCUS()
      ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &SuggestionList::OnCustomDraw)
      ON_NOTIFY_REFLECT(LVN_GETDISPINFO, &SuggestionList::OnRetrieveItem)
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   SuggestionList::SuggestionList() : CustomDraw(*this)
   {
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
      return CListCtrl::Create(style, rc, parent, IDC_SUGGESTION_LIST);
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
      // Exclude {,[,$,: etc.
      GuiString str(tok.ValueText); 

      // Linear search for partial substring
      int index = 0;
      for (auto& item : Content)
      {
         if (StrCmpNI(str.c_str(), item.Key.c_str(), str.length()) == 0)
            break;
         ++index;
      }

      // Search/display closest match
      if (index != Content.size())
      {
         //Console << L"Search for " << str << L" matched " << Content[index].Text << ENDL;
         SetItemState(index, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
         EnsureVisible(index, FALSE);
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

      // Shrink to fit
      ShrinkToFit();
      return 0;
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

         Console << "SuggestionCustomDraw::onDrawSubItem(): iSubItem=" << item.SubItem << "  rect=" << item.Rect << ENDL;
         // Text:
         if (item.SubItem == 0)
         {
            dc->DrawText(data.Text.c_str(), item.Rect, DT_LEFT|DT_SINGLELINE|DT_END_ELLIPSIS);
         }
         // Type:
         else if (item.SubItem == 1)
         {
            dc->DrawText(data.Type.c_str(), item.Rect, DT_RIGHT|DT_SINGLELINE|DT_END_ELLIPSIS);

            //auto src = ListView.GetItemText(item.Index, 1);
            //auto flags = item.Selected ? RenderFlags::Selected : RenderFlags::Inverted;

            //try
            //{  // Parse+Draw
            //   RichTextRenderer::DrawLine(dc, item.Rect, RichStringParser((LPCWSTR)src).Output, flags);
            //}
            //catch (ExceptionBase&) {
            //   dc->SetTextColor((COLORREF)RichTextColour::Red);
            //   dc->DrawText(src, item.Rect, DT_LEFT|DT_SINGLELINE);
            //}
            
         }
      }
      catch (ExceptionBase& e) {
         Console.Log(HERE, e);
      }
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
            Content.push_back( SuggestionItem(lab.Name, GuiString(L"Line %d", lab.LineNumber)) );
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
         if (GetCountPerPage() > (int)Content.size())
         {
            ClientRect wnd(this);
            CRect rc(0,0,0,0);

            // Resize
            if (!GetItemRect(0, &rc, LVIR_LABEL) || !rc.Height())
               throw Win32Exception(HERE, L"Unable to retrieve item height");
            SetWindowPos(nullptr,-1,-1, wnd.Width(), rc.Height()*Content.size(), SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);
         }
      }
      catch (ExceptionBase& e) { 
         Console.Log(HERE, e);
      }
   }

   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
NAMESPACE_END2(GUI,Controls)



