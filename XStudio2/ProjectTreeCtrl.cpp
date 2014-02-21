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
         auto root = InsertItem(TreeItem(doc), nullptr);

         // Folders: Populate recursively
         for (auto item : doc->Project.Items)
            InsertItem(TreeItem(item), root);
      }
   }

   // ------------------------------ PROTECTED METHODS -----------------------------

   /// <summary>Inserts an item and it's children</summary>
   /// <param name="item">The item.</param>
   /// <param name="parent">The parent.</param>
   HTREEITEM ProjectTreeCtrl::InsertItem(TreeItem& item, HTREEITEM parent)
   {
      // Insert item/folder
      auto node = CTreeCtrl::InsertItem(item.mask, item.pszText, item.iImage, item.iSelectedImage, item.state, item.stateMask, item.lParam, parent, TVI_SORT);

      // Children: Insert children
      if (item.Data)
         for (auto it : item.Data->Children)
            InsertItem(TreeItem(it), node);

      // Return item
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
