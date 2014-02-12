#include "stdafx.h"
#include "CommandWnd.h"
#include <strsafe.h>
#include "Helpers.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)

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

   /// <summary>Populates the group combo.</summary>
   void  CCommandWnd::PopulateGroupCombo()
   {
      Groups.AddString(L"All Groups");

      // Populate group names
      for (CommandGroup g : SyntaxLib.GetGroups())
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
      auto Content = SyntaxLib.Query(searchTerm.c_str(), GameVersion::TerranConflict);
      ListView.SetItemCount(Content.size());
            
      // Define groups
      for (CommandGroup g : SyntaxLib.GetGroups())
      {
         LVGroup grp((UINT)g, GetString(g));
         // Insert group
         if (ListView.InsertGroup(grp.iGroupId, (LVGROUP*)&grp) != grp.iGroupId)
            throw Win32Exception(HERE, GuiString(L"Unable to insert command group ") + GetString(g));
      }

      // Generate/insert display text for each command
      for (UINT i = 0; i < Content.size(); ++i)
      {
         LVItem item(i, Content[i]->DisplayText, (UINT)Content[i]->Group, LVIF_TEXT | LVIF_GROUPID | LVIF_IMAGE);
         item.iImage = 0;

         // Set icon
         if (Content[i]->IsCompatible(GameVersion::Threat))
            item.iImage = 3;
         else if (Content[i]->IsCompatible(GameVersion::Reunion))
            item.iImage = 4;
         else
            item.iImage = Content[i]->IsCompatible(GameVersion::TerranConflict) ? 5 : 6;

         // Insert item
         if (ListView.InsertItem((LVITEM*)&item) == -1)
            throw Win32Exception(HERE, GuiString(L"Unable to insert %s command '%s' (item %d)", GetString(Content[i]->Group).c_str(), item.pszText, i) );
      }
   }
   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
NAMESPACE_END2(GUI,Windows)
