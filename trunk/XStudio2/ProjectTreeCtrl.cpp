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
      ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT, &ProjectTreeCtrl::OnLabelEditBegin)
      ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, &ProjectTreeCtrl::OnLabelEditEnd)
      ON_NOTIFY_REFLECT(TVN_BEGINDRAG, &ProjectTreeCtrl::OnDragBegin)
      ON_WM_MOUSEMOVE()
      ON_WM_LBUTTONUP()
   END_MESSAGE_MAP()

   // -------------------------------- CONSTRUCTION --------------------------------

   ProjectTreeCtrl::ProjectTreeCtrl() 
      : DragIcon(nullptr), 
        fnItemAdded(ProjectDocument::ItemAdded.Register(this, &ProjectTreeCtrl::OnItemAdded)),
        fnItemChanged(ProjectDocument::ItemAdded.Register(this, &ProjectTreeCtrl::OnItemChanged)),
        fnItemRemoved(ProjectDocument::ItemRemoved.Register(this, &ProjectTreeCtrl::OnItemRemoved))
   {
   }

   ProjectTreeCtrl::~ProjectTreeCtrl()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   /// <summary>Gets the item data for the selected item</summary>
   /// <returns></returns>
   ProjectItem*  ProjectTreeCtrl::GetSelectedItemData() const
   {
      // Lookup selected item
      TreeItem selected(GetSelectedItem());
      GetItem(&selected);

      // Get data
      return selected.Data;
   }

   /// <summary>Populates the entire treeview from the active project.</summary>
   void ProjectTreeCtrl::Populate()
   {
      // Clear
      DeleteAllItems();
    
      try
      {
         // Repopulate
         if (auto doc = ProjectDocument::GetActive())
         {
            // Root: Project Name
            auto root = InsertItem(TreeItem(doc), nullptr);

            // Folders: Populate recursively
            for (auto& folder : doc->Project.Items)
               InsertItem(TreeItem(folder.get()), root);
         }
      }
      catch (ExceptionBase& e) {
         Console.Log(HERE, e);
      }
   }

   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Reverse lookup a tree item from project item data</summary>
   /// <param name="item">item data.</param>
   /// <returns></returns>
   HTREEITEM ProjectTreeCtrl::FindItem(const ProjectItem* item) const
   {
      return FindItem(GetRootItem(), item);
   }

   /// <summary>Finds a tree item from data.</summary>
   /// <param name="item">item to search</param>
   /// <param name="data">data</param>
   /// <returns></returns>
   HTREEITEM ProjectTreeCtrl::FindItem(HTREEITEM item, const ProjectItem* data) const
   {
      // Search item
      if (reinterpret_cast<const ProjectItem*>(GetItemData(item)) == data)
         return item;

      // Perform depth first search from input item
      for (item = CTreeCtrl::GetNextItem(item, TVGN_CHILD); item != nullptr; item = CTreeCtrl::GetNextItem(item, TVGN_NEXT))
         if (HTREEITEM match = FindItem(item, data))
            return match;

      // Not found
      return nullptr;
   }

   /// <summary>Inserts an item and it's children</summary>
   /// <param name="item">The item.</param>
   /// <param name="parent">The parent.</param>
   HTREEITEM ProjectTreeCtrl::InsertItem(TreeItem& item, HTREEITEM parent)
   {
      // Insert item/folder
      auto node = CTreeCtrl::InsertItem(item.mask, item.pszText, item.iImage, item.iSelectedImage, item.state, item.stateMask, item.lParam, parent, TVI_SORT);

      // Children: Insert children
      if (item.Data)
         for (auto& ch : item.Data->Children)
            InsertItem(TreeItem(ch.get()), node);

      // Return item
      return node;
   }

   /// <summary>Allow user to reposition items by dragging</summary>
   /// <param name="pNMHDR">The NMHDR.</param>
   /// <param name="pResult">The result.</param>
   void ProjectTreeCtrl::OnDragBegin(NMHDR *pNMHDR, LRESULT *pResult)
   {
      // Get/Store drag source
      DragSource = reinterpret_cast<NMTREEVIEW*>(pNMHDR)->itemNew;
      
      // Ensure draggable
      if (DragSource.IsDragSource())
      {
         try
         {
            // Highlight item
            if (!SelectDropTarget(DragSource.hItem))
               throw Win32Exception(HERE, L"Unable to select drop target");

            // Create drag image
            else if ((DragIcon=CreateDragImage(DragSource.hItem)) == nullptr)
               throw Win32Exception(HERE, L"Unable to create drag image");

            // Init drag'n'drop
            else if (!DragIcon->BeginDrag(0, CPoint(0,0)) || !DragIcon->DragEnter(this, CursorPoint(this)))
               throw Win32Exception(HERE, L"Unable to begin drag n drop");

            // Capture mouse
            SetCapture();
         }
         catch (ExceptionBase& e) {
            Console.Log(HERE, e);

            // Cleanup icon, if any
            if (DragIcon)
               delete DragIcon;
            DragIcon = nullptr;
         }
      }

      *pResult = 0;
   }

   /// <summary>Move item being dragged</summary>
   /// <param name="target">The target.</param>
   void  ProjectTreeCtrl::OnDragEnd(const TreeItem& target)
   {
      // Ensure source != target
      if (DragSource == target)
         return;

      // Feedback
      Console << Cons::UserAction << "Moving project item " << Cons::Yellow << DragSource.Data->Name 
              << Cons::White << " to " << Cons::Yellow << target.Data->Name << ENDL;

      // Move item 
      ProjectDocument::GetActive()->MoveItem(DragSource.Data, dynamic_cast<ProjectFolderItem*>(target.Data));
   }

   /// <summary>Called when item added.</summary>
   /// <param name="item">The item.</param>
   /// <param name="parent">Parent of item</param>
   void  ProjectTreeCtrl::OnItemAdded(ProjectItem* item, ProjectItem* parent)
   {
      InsertItem(TreeItem(item), FindItem(parent));
   }

   /// <summary>Called when item or project renamed.</summary>
   /// <param name="item">The item, or nullptr for project</param>
   /// <param name="parent">Always null</param>
   void  ProjectTreeCtrl::OnItemChanged(ProjectItem* item, ProjectItem* /*parent = nullptr*/)
   {
      if (item)
         SetItemText(FindItem(item), item->Name.c_str());
      else
         SetItemText(GetRootItem(), ProjectDocument::GetActive()->GetTitle());
   }

   /// <summary>Called when item added.</summary>
   /// <param name="item">The item.</param>
   /// <param name="parent">Always null</param>
   void  ProjectTreeCtrl::OnItemRemoved(ProjectItem* item, ProjectItem* /*parent = nullptr*/)
   {
      DeleteItem(FindItem(item));
   }

   /// <summary>Allow user to rename the root and non-fixed items</summary>
   /// <param name="pNMHDR">The NMHDR.</param>
   /// <param name="pResult">The result.</param>
   void ProjectTreeCtrl::OnLabelEditBegin(NMHDR* pNMHDR, LRESULT* pResult)
   {
      static const LRESULT  ENABLE = 0, 
                            DISABLE = 1;
      
      // Get/store item
      LabelItem = reinterpret_cast<NMTVDISPINFO*>(pNMHDR)->item;
    
      // Ensure Root or non-fixed item/folder
      if ((*pResult = LabelItem.IsEditable() ? ENABLE : DISABLE) == ENABLE)
      {
         // Feedback
         /*auto name = (item.Data ? item.Data->Name : (LPCWSTR)ProjectDocument::GetActive()->GetTitle());
         Console << Cons::UserAction << "Renaming project item: " << Cons::Yellow << name << ENDL;*/

         // TODO: Customize/Setup edit
         if (auto edit = GetEditControl())
            edit->SetLimitText(MAX_PATH);
      }
   }
   
   /// <summary>Update project after item renamed</summary>
   /// <param name="pNMHDR">The NMHDR.</param>
   /// <param name="pResult">The result.</param>
   void ProjectTreeCtrl::OnLabelEditEnd(NMHDR *pNMHDR, LRESULT *pResult)
   {
      static const LRESULT  ACCEPT = 1, 
                            REJECT = 0;
      try
      {
         // Get new item text
         const wchar* newText = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR)->item.pszText;

         // Cancelled: Ignore
         if (newText == nullptr)
            *pResult = REJECT;
         else
         {
            // Feedback
            Console << Cons::UserAction << "Renaming project item " << Cons::Yellow << LabelItem.GetDebugName() 
                    << Cons::White << " to " << Cons::Yellow << newText << ENDL;

            // Item: Rename item
            if (LabelItem.Data)
               ProjectDocument::GetActive()->RenameItem(LabelItem.Data, newText);
            else
               // Project: Rename project
               ProjectDocument::GetActive()->Rename(newText);

            // Accept
            *pResult = ACCEPT;
         }  
      }
      catch (ExceptionBase& e) {
         theApp.ShowError(HERE, e);
         *pResult = REJECT;
      }
   }
   

   /// <summary>Invokes the end of a drag'n'drop operation.</summary>
   /// <param name="nFlags">The n flags.</param>
   /// <param name="point">The point.</param>
   void ProjectTreeCtrl::OnLButtonUp(UINT nFlags, CPoint point)
   {
      // Drag'n'Drop
      if (DragIcon)
      {
         try
         {
            // Lookup drag target
            TreeItem item(CTreeCtrl::GetDropHilightItem());
            GetItem(&item);
      
            // Clear drop target
            if (!SelectDropTarget(nullptr))
               throw Win32Exception(HERE, L"Unable to clear drop target");
            if (!ReleaseCapture())
               throw Win32Exception(HERE, L"Unable to release mouse capture");

            // Clear drag
            if (!DragIcon->DragLeave(this))
               throw Win32Exception(HERE, L"Unable to release drag icon");

            // Reset cursor
            SetCursor(LoadCursor(NULL, IDC_ARROW));

            // Validate Target + Raise 'DRAG END'
            if (item.IsDropTarget())
               OnDragEnd(item);
         }
         catch (ExceptionBase& e) {
            Console.Log(HERE, e);
         }
         
         // Cleanup drag icon
         DragIcon->EndDrag();
         delete DragIcon;
         DragIcon = nullptr;

         // Refresh
         RedrawWindow();
      }

      CTreeCtrl::OnLButtonUp(nFlags, point);
   }

   /// <summary>Drags the drag'n'drop icon</summary>
   /// <param name="nFlags">The n flags.</param>
   /// <param name="point">The point.</param>
   void ProjectTreeCtrl::OnMouseMove(UINT nFlags, CPoint point)
   {
      // Drag'n'Drop
      if (DragIcon)
      {
         try
         {
            // Find item
            UINT flags = TVHT_ONITEM;
            auto item = HitTest(point, &flags);
            TVItem data(item, MAX_PATH);

            // Unlock / Highlight item / Lock
            if (!DragIcon->DragShowNolock(FALSE) || !SelectDropTarget(item) || !DragIcon->DragShowNolock(TRUE))
               throw Win32Exception(HERE, L"Unable to highlight drop target");
            
            // Move image
            if (!DragIcon->DragMove(point))
               throw Win32Exception(HERE, L"Unable to move drag'n'drop icon");

            // Set cursor
            if (GetItem(&data))
               SetCursor(LoadCursor(NULL, TreeItem(data).IsDropTarget() ? IDC_ARROW : IDC_NO));
         }
         catch (ExceptionBase& e) {
            Console.Log(HERE, e);
         }
      }

      CTreeCtrl::OnMouseMove(nFlags, point);
   }


   /// <summary>App wizard generated</summary>
   /// <param name="wParam">The parameter.</param>
   /// <param name="lParam">The parameter.</param>
   /// <param name="pResult">The result.</param>
   /// <returns></returns>
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
   
   // ------------------------------- PRIVATE METHODS ------------------------------
   
/// <summary>User interface controls</summary>
NAMESPACE_END2(GUI,Controls)


