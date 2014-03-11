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

   /// <summary>Creates the window.</summary>
   /// <param name="parent">The parent.</param>
   /// <exception cref="Logic::Win32Exception">Unable to create window</exception>
   void CGameObjectWnd::Create(CWnd* parent)
   {
      __super::Create(parent, L"Game Objects", IDR_GAME_OBJECTS, IDR_GAME_OBJECTS);
   }

   // ------------------------------ PROTECTED METHODS -----------------------------

   /// <summary>Gets the item text.</summary>
   /// <param name="index">The index.</param>
   /// <returns></returns>
   wstring  CGameObjectWnd::GetItemText(UINT index)
   {
      auto obj = reinterpret_cast<const GameObject*>(ListView.GetItemData(index));

      // Format item
      return obj ? obj->GetDisplayText() : L"";
   }

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
      auto Content = GameObjectLib.Query(searchTerm);
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
      UINT i = 0;
      for (auto& obj : Content)
      {
         LVItem item(i++, obj->Name, obj->Type - MainType::Dock, LVIF_TEXT|LVIF_GROUPID|LVIF_IMAGE|LVIF_PARAM);
         item.iImage = 1;
         item.lParam = (LPARAM)obj;

         // Insert item
         if (ListView.InsertItem((LVITEM*)&item) == -1)
            throw Win32Exception(HERE, GuiString(L"Unable to insert %s '%s' (item %d)", GetString(obj->Type).c_str(), item.pszText, i-1));
      }
   }
   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
NAMESPACE_END2(GUI,Windows)
