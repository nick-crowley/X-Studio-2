#include "stdafx.h"
#include "GameObjectWnd.h"
#include <strsafe.h>
#include "Logic/GameObjectLibrary.h"
#include "Helpers.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)

   // --------------------------------- APP WIZARD ---------------------------------
  
   BEGIN_MESSAGE_MAP(CGameObjectWnd, CGameDataWnd)
   END_MESSAGE_MAP()

   // -------------------------------- CONSTRUCTION --------------------------------

   CGameObjectWnd::CGameObjectWnd() 
   {
   }


   CGameObjectWnd::~CGameObjectWnd()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   // ------------------------------ PROTECTED METHODS -----------------------------

   /// <summary>Populates the group combo.</summary>
   void  CGameObjectWnd::PopulateGroupCombo()
   {
      Groups.AddString(L"All Groups");

      // Populate group names
      for (MainType type = MainType::Dock; type <= MainType::TechWare; type++)
         Groups.AddString(GetString(type).c_str());

      // Select 'unfiltered'
      Groups.SetCurSel(0);
   }

   /// <summary>Populates the items</summary>
   /// <param name="searchTerm">The search term.</param>
   /// <param name="selectedGroup">The selected group.</param>
   void CGameObjectWnd::PopulateItems(const wstring& searchTerm, UINT selectedGroup)
   {
      // Lookup matches
      auto Content = GameObjectLib.Query(searchTerm.c_str());
      ListView.SetItemCount(Content.size());
      
      // Redefine groups
      for (MainType type = MainType::Dock; type <= MainType::TechWare; type++) 
      {
         LVGroup grp(type - MainType::Dock, GetString(type));
         // Insert group
         if (ListView.InsertGroup(grp.iGroupId, (LVGROUP*)&grp) != grp.iGroupId)
            throw Win32Exception(HERE, GuiString(L"Unable to insert game object group ") + GetString(type));
      }

      // Generate/insert display text for each command
      for (UINT i = 0; i < Content.size(); ++i)
      {
         LVItem item(i, Content[i].Name, Content[i].Type - MainType::Dock, LVIF_TEXT | LVIF_GROUPID | LVIF_IMAGE);
         item.iImage = 1;

         // Insert item
         if (ListView.InsertItem((LVITEM*)&item) == -1)
            throw Win32Exception(HERE, GuiString(L"Unable to insert %s '%s' (item %d)", GetString(Content[i].Type).c_str(), item.pszText, i));
      }
   }
   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
NAMESPACE_END2(GUI,Windows)
