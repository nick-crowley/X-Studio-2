#include "stdafx.h"
#include "CommandWnd.h"
#include <strsafe.h>
#include "Helpers.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN(GUI)

   // --------------------------------- APP WIZARD ---------------------------------
  
   BEGIN_MESSAGE_MAP(CCommandWnd, CGameDataWnd)
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

   // ------------------------------ PROTECTED METHODS -----------------------------

   void  CCommandWnd::PopulateGroupCombo()
   {
      Groups.AddString(L"All Groups");

      // Populate group names
      for (UINT g = (UINT)CommandGroup::ARRAY; g < (UINT)CommandGroup::HIDDEN; ++g)
         Groups.AddString(GuiString(IDS_FIRST_COMMAND_GROUP + g).c_str());

      // Select 'unfiltered'
      Groups.SetCurSel(0);
   }

   void CCommandWnd::PopulateItems(const wstring& searchTerm, UINT selectedGroup)
   {
      // Lookup matches
      auto Content = SyntaxLib.Query(searchTerm.c_str(), GameVersion::TerranConflict);
      ListView.SetItemCount(Content.size());
            
      // Redefine groups
      for (auto pair : SyntaxLib.GetGroups())
      {
         const wstring& name = pair.second;
         UINT id = (UINT)pair.first;
               
         // Insert group
         LVGroup g(id, name);
         if (ListView.InsertGroup(g.iGroupId, (LVGROUP*)&g) != g.iGroupId)
            throw Win32Exception(HERE, GuiString(L"Unable to insert command group ") + name);
      }

      // Generate/insert display text for each command
      for (UINT i = 0; i < Content.size(); ++i)
      {
         LVItem item(i, Content[i]->GetDisplayText(), (UINT)Content[i]->Group, LVIF_TEXT | LVIF_GROUPID | LVIF_IMAGE);
         item.iImage = 0;

         // Insert item
         if (ListView.InsertItem((LVITEM*)&item) == -1)
            throw Win32Exception(HERE, GuiString(L"Unable to insert command '%s' (item %d, group %d)", item.pszText, i, item.iGroupId));
      }
   }
   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
NAMESPACE_END(GUI)
