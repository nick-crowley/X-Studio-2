#include "stdafx.h"
#include "ProjectTreeCtrl.h"
#include "ProjectDocument.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/// <summary>User interface controls</summary>
NAMESPACE_BEGIN2(GUI,Controls)

   // --------------------------------- APP WIZARD ---------------------------------

   BEGIN_MESSAGE_MAP(ProjectTreeCtrl, CTreeCtrl)
   END_MESSAGE_MAP()

   // -------------------------------- CONSTRUCTION --------------------------------

   ProjectTreeCtrl::ProjectTreeCtrl()
   {
   }

   ProjectTreeCtrl::~ProjectTreeCtrl()
   {
   }

   

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   /// <summary>Populates the entire treeview from the active project.</summary>
   void ProjectTreeCtrl::Populate()
   {
      // Clear
      DeleteAllItems();
      
      // Repopulate
      if (auto doc = ProjectDocument::GetActive())
      {
         // Root: Project Name
         HTREEITEM root = __super::InsertItem(doc->GetTitle(), 1, 1);
	      SetItemState(root, TVIS_BOLD, TVIS_BOLD);

         // Items: Populate recursively
         for (auto item : doc->Project.Items)
            SortChildren( InsertItem(item, root) );

         // Expand root
         Expand(root, TVE_EXPAND);
      }
   }

   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Gets the item icon.</summary>
   /// <param name="item">The item.</param>
   /// <returns></returns>
   int ProjectTreeCtrl::GetItemIcon(ProjectItem* item)
   {
      int icon = -1;

      // Choose icon
      switch (item->Type)
      {
      case ProjectItemType::Folder:    icon = 0;  break;
      case ProjectItemType::Variable:  icon = 6;  break;
      case ProjectItemType::File:      
         switch (dynamic_cast<ProjectFileItem*>(item)->FileType)
         {
         case FileType::Script:   icon = 2;  break;
         case FileType::Language: icon = 3;  break;
         case FileType::Mission:  icon = 4;  break;
         case FileType::Unknown:  icon = 5;  break;
         }
         break;
      }

      return icon;
   }

   /// <summary>Inserts an item and it's children</summary>
   /// <param name="item">The item.</param>
   /// <param name="parent">The parent.</param>
   HTREEITEM ProjectTreeCtrl::InsertItem(ProjectItem* item, HTREEITEM parent)
   {
      wstring name;

      // Generate name
      if (auto var = dynamic_cast<ProjectVariableItem*>(item))
         name = GuiString(L"%s = %d", var->Name.c_str(), var->Value);
      else
         name = item->Name;
      
      // Add item
      HTREEITEM node = __super::InsertItem(name.c_str(), GetItemIcon(item), GetItemIcon(item), parent);

      // Fixed: Display in bold
      if (item->Fixed)
	      SetItemState(node, TVIS_BOLD, TVIS_BOLD);

      // Folder: Insert children
      for (auto child : item->Children)
         InsertItem(child, node);

      // Fixed: Expand
      if (item->Fixed)
         Expand(node, TVE_EXPAND);
      return node;
   }

   // ------------------------------- PRIVATE METHODS ------------------------------

   BOOL ProjectTreeCtrl::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
   {
	   BOOL bRes = CTreeCtrl::OnNotify(wParam, lParam, pResult);

	   NMHDR* pNMHDR = (NMHDR*)lParam;
	   ASSERT(pNMHDR != NULL);

	   if (pNMHDR && pNMHDR->code == TTN_SHOW && GetToolTips() != NULL)
	   {
		   GetToolTips()->SetWindowPos(&wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
	   }

	   return bRes;
   }

/// <summary>User interface controls</summary>
NAMESPACE_END2(GUI,Controls)
