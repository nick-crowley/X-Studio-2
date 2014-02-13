#include "stdafx.h"
#include "ScriptObjectWnd.h"
#include "Logic/ScriptObjectLibrary.h"
#include <strsafe.h>
#include "Helpers.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)

   // --------------------------------- APP WIZARD ---------------------------------
  
   BEGIN_MESSAGE_MAP(CScriptObjectWnd, CGameDataWnd)
   END_MESSAGE_MAP()

   // -------------------------------- CONSTRUCTION --------------------------------

   CScriptObjectWnd::CScriptObjectWnd() 
   {
   }


   CScriptObjectWnd::~CScriptObjectWnd()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   // ------------------------------ PROTECTED METHODS -----------------------------

   /// <summary>Populates the group combo.</summary>
   void  CScriptObjectWnd::PopulateGroupCombo()
   {
      Groups.AddString(L"All Groups");

      // Populate group names
      for (ScriptObjectGroup g = ScriptObjectGroup::Constant; g <= ScriptObjectGroup::TransportClass; g++)
         Groups.AddString(GetString(g).c_str());

      // Select 'unfiltered'
      Groups.SetCurSel(0);
   }

   /// <summary>Populates the items</summary>
   /// <param name="searchTerm">The search term.</param>
   /// <param name="selectedGroup">The selected group.</param>
   void CScriptObjectWnd::PopulateItems(const wstring& searchTerm, UINT selectedGroup)
   {
      // Lookup matches
      auto Content = ScriptObjectLib.Query(searchTerm.c_str());
      ListView.SetItemCount(Content.size());
      
      // Redefine groups
      for (ScriptObjectGroup g = ScriptObjectGroup::Constant; g <= ScriptObjectGroup::TransportClass; g++)
      {
         LVGroup grp((UINT)g, GetString(g));
         
         // Insert group
         if (ListView.InsertGroup(grp.iGroupId, (LVGROUP*)&grp) != grp.iGroupId)
            throw Win32Exception(HERE, GuiString(L"Unable to insert script object group ") + GetString(g));
      }

      // Generate/insert display text for each command
      for (UINT i = 0; i < Content.size(); ++i)
      {
         // Define item
         LVItem item(i, Content[i].Text, (UINT)Content[i].Group, LVIF_TEXT | LVIF_GROUPID | LVIF_IMAGE);
         item.iImage = 3 + GameVersionIndex(Content[i].Version).Index;

         // Insert item
         if (ListView.InsertItem((LVITEM*)&item) == -1)
            throw Win32Exception(HERE, GuiString(L"Unable to insert %s '%s' (item %d)", GetString(Content[i].Group).c_str(), item.pszText, i));
      }
   }
   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
NAMESPACE_END2(GUI,Windows)
