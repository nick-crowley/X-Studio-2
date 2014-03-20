#include "stdafx.h"
#include "CommandWnd.h"
#include <strsafe.h>
#include "Helpers.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)

   // --------------------------------- APP WIZARD ---------------------------------
  
   BEGIN_MESSAGE_MAP(CCommandWnd, CGameDataWnd)
      //ON_WM_CONTEXTMENU()
      ON_COMMAND(ID_GAMEDATA_LOOKUP, &CCommandWnd::OnCommandLookup)
   END_MESSAGE_MAP()

   // -------------------------------- CONSTRUCTION --------------------------------

   CCommandWnd::CCommandWnd() 
   {
   }


   CCommandWnd::~CCommandWnd()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   /// <summary>Creates the window.</summary>
   /// <param name="parent">The parent.</param>
   /// <exception cref="Logic::Win32Exception">Unable to create window</exception>
   void CCommandWnd::Create(CWnd* parent)
   {
      __super::Create(parent, L"Commands", IDR_COMMANDS, IDR_COMMANDS);
   }

   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Gets the current content.</summary>
   /// <returns></returns>
   CmdSyntaxArray  CCommandWnd::GetContent() const
   {
      // Convert group selection
      auto grp = (CommandGroup)(Groups.GetCurSel()-1);

      // Lookup matches
      return SyntaxLib.Query(GetSearchTerm(), GameVersion::TerranConflict, grp);
   }

   /// <summary>Gets the command text to be inserted for this item.</summary>
   /// <param name="index">Zero-based item index.</param>
   /// <returns></returns>
   wstring  CCommandWnd::GetItemText(UINT index)
   {
      auto cmd = reinterpret_cast<const CommandSyntax*>(ListView.GetItemData(index));

      // Format item
      return cmd ? cmd->GetDisplayText() : L"";
   }
   
   /// <summary>Populate when application state changes.</summary>
   /// <param name="s">The s.</param>
   void CCommandWnd::OnAppStateChanged(AppState s)
   {
      // Refresh groups
      Groups.ResetContent();
      PopulateGroupCombo();

      // Update content
      __super::OnAppStateChanged(s);
   }

   /// <summary>Called when context menu.</summary>
   /// <param name="wnd">window.</param>
   /// <param name="pt">Point in screen co-ordinates.</param>
   /*void  CCommandWnd::OnContextMenu(CWnd* wnd, CPoint pt)
   {
      __super::OnContextMenu(wnd, pt);
   }*/
   
   /// <summary>Launch MSCI reference URL for selected command</summary>
   void CCommandWnd::OnCommandLookup()
   {
      // Require selection
      if (ListView.GetNextItem(-1, LVNI_SELECTED) == -1)
         return;

      // Get content
      auto content = GetContent();
      UINT item = ListView.GetNextItem(-1, LVNI_SELECTED);

      // Lookup item
      if (item < content.size() && !content[item]->URL.empty())
         ShellExecute(*theApp.m_pMainWnd, TEXT("open"), content[item]->URL.c_str(), NULL, NULL, SW_SHOWMAXIMIZED);
   }
      
   /// <summary>Queries the state of a menu command.</summary>
   /// <param name="pCmdUI">The command UI.</param>
   void CCommandWnd::OnQueryCommand(CCmdUI* pCmdUI)
   {
      bool state = false;
      UINT item = ListView.GetNextItem(-1, LVNI_SELECTED);

      // State
      switch (pCmdUI->m_nID)
      {
      // Query selection
      case ID_EDIT_COPY:
      case ID_GAMEDATA_INSERT:  
         state = item != -1;
         break;

      // Query selection + MSCI URL
      case ID_GAMEDATA_LOOKUP:  
         if (item != -1)
         {
            auto content = GetContent();
            state = (item < content.size() && !content[item]->URL.empty());
         }
         break;
      }

      // Set state
      pCmdUI->Enable(state ? TRUE : FALSE);
      pCmdUI->SetCheck(FALSE);
   }

   /// <summary>Supply tooltip data.</summary>
   /// <param name="data">data.</param>
   void CCommandWnd::OnRequestTooltip(CustomTooltip::TooltipData* data)
   {
      int index = GetHotItemIndex();

      // DEUBG:
      //Console << "Requesting tooltip for item index=" << index << ENDL;

      // Verify index
      if (index != -1)
      {
         // Lookup current items
         auto Content = GetContent();

         // Provide item
         if (index < (int)Content.size())
         {
            data->ResetTo(CommandTooltipData(*Content[index]));
            return;
         }
      }

      // No item: Cancel
      data->Cancel();
   }

   /// <summary>Populates the group combo.</summary>
   void  CCommandWnd::PopulateGroupCombo()
   {
      Groups.AddString(L"All Groups");

      // Populate group names
      for (CommandGroup g : SyntaxLib.GetGroups())
         if (g != CommandGroup::HIDDEN)
            Groups.AddString(GetString(g).c_str());

      // Select 'unfiltered'
      Groups.SetCurSel(0);
   }

   /// <summary>Populates the items</summary>
   /// <param name="searchTerm">The search term.</param>
   /// <param name="selectedGroup">The selected group.</param>
   void CCommandWnd::PopulateItems(const wstring& searchTerm, UINT selectedGroup)
   {
      // Lookup matches
      auto Content = GetContent();
      ListView.SetItemCount(Content.size());
            
      // Define ListView groups
      for (CommandGroup g : SyntaxLib.GetGroups())
      {
         LVGroup grp((UINT)g, GetString(g));
         // Insert group
         if (ListView.InsertGroup(grp.iGroupId, (LVGROUP*)&grp) != grp.iGroupId)
            throw Win32Exception(HERE, GuiString(L"Unable to insert command group ") + GetString(g));
      }

      // Generate/insert display text for each command
      UINT i = 0;
      for (auto& obj : Content)
      {
         LVItem item(i++, obj->DisplayText, (UINT)obj->Group, LVIF_TEXT|LVIF_GROUPID|LVIF_IMAGE|LVIF_PARAM);
         item.iImage = 0;
         item.lParam = (LPARAM)obj;

         // Set icon
         if (obj->IsCompatible(GameVersion::Threat))
            item.iImage = 3;
         else if (obj->IsCompatible(GameVersion::Reunion))
            item.iImage = 4;
         else
            item.iImage = obj->IsCompatible(GameVersion::TerranConflict) ? 5 : 6;

         // Insert item
         if (ListView.InsertItem((LVITEM*)&item) == -1)
            throw Win32Exception(HERE, VString(L"Unable to insert %s command '%s' (item %d)", GetString(obj->Group).c_str(), item.pszText, i-1) );
      }
   }
   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
NAMESPACE_END2(GUI,Windows)
