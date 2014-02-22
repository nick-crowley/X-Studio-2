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

   ProjectTreeCtrl::ProjectTreeCtrl() : DragIcon(nullptr), DragSource((HTREEITEM)nullptr)
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

   // ------------------------------- PRIVATE METHODS ------------------------------
   
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
            // DEBUG:
            //Console << "Beginning drag operation" << ENDL;

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
      // Feedback
      Console << Cons::UserAction << "Moving project item " << Cons::Yellow << DragSource.Data->Name 
              << Cons::White << " to " << Cons::Yellow << target.Data->Name << ENDL;

      // Move item in project
      ProjectDocument::GetActive()->MoveItem(DragSource.Data, dynamic_cast<ProjectFolderItem*>(target.Data));

      // Move item in tree
      DeleteItem(DragSource.hItem);
      InsertItem(TreeItem(DragSource.Data), target.hItem);
   }


   /// <summary>Allow user to rename the root and non-fixed items</summary>
   /// <param name="pNMHDR">The NMHDR.</param>
   /// <param name="pResult">The result.</param>
   void ProjectTreeCtrl::OnLabelEditBegin(NMHDR* pNMHDR, LRESULT* pResult)
   {
      static const LRESULT  ENABLE = 0, 
                            DISABLE = 1;
      // Get item
      TreeItem item(reinterpret_cast<NMTVDISPINFO*>(pNMHDR)->item);
    
      // Ensure Root or non-fixed item/folder
      if ((*pResult = item.IsEditable() ? ENABLE : DISABLE) == ENABLE)
      {
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

      // Get item
      TreeItem item(reinterpret_cast<LPNMTVDISPINFO>(pNMHDR)->item);

      // Rejected: Ignore
      if (!item.pszText)
         *pResult = REJECT;
      else
      {
         // TODO: Validate + Update
         *pResult = ACCEPT;
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
            auto item = HitTest(point);
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

/// <summary>User interface controls</summary>
NAMESPACE_END2(GUI,Controls)


