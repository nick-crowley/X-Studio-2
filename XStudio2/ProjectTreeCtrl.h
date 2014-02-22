#pragma once
#include "Helpers.h"
#include "ProjectDocument.h"

/// <summary>User interface controls</summary>
NAMESPACE_BEGIN2(GUI,Controls)

   /// <summary>Project window treeview control</summary>
   class ProjectTreeCtrl : public CTreeCtrl
   {
      // ------------------------ TYPES --------------------------
   private:
      /// <summary>Wrapper for Items in the tree</summary>
	   class TreeItem : public TVItem
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create empty item for storing drag'n'drop info</summary>
         TreeItem(HTREEITEM node) : TVItem(node, MAX_PATH)
         {}
         
         /// <summary>Create item wrapper</summary>
         /// <param name="item">The item.</param>
         TreeItem(const TVITEM& item) : TVItem(item)
         {
         }

         /// <summary>Create from project document</summary>
         /// <param name="item">The item.</param>
         /// <exception cref="Logic::ArgumentNullException">Document is null</exception>
         TreeItem(ProjectDocument* doc) : TVItem(nullptr, MAX_PATH)
         {
            REQUIRED(doc);

            // Use document title
            SetText((LPCWSTR)doc->GetTitle());

            // Set icon
            iImage = iSelectedImage = 1;
            
            // Bold + Expanded
            state = stateMask = TVIS_BOLD | TVIS_EXPANDED;
         }

         /// <summary>Create from project item</summary>
         /// <param name="item">The item.</param>
         /// <exception cref="Logic::ArgumentNullException">Item is null</exception>
         TreeItem(ProjectItem* item) : TVItem(nullptr, MAX_PATH)
         {
            REQUIRED(item);

            // Generate name
            if (auto var = dynamic_cast<ProjectVariableItem*>(item))
               SetText(GuiString(L"%s = %d", var->Name.c_str(), var->Value));
            else
               SetText(item->Name);

            // Choose icon
            switch (item->Type)
            {
            case ProjectItemType::Folder:    iImage = 0;  break;
            case ProjectItemType::Variable:  iImage = 6;  break;
            case ProjectItemType::File:      
               switch (dynamic_cast<ProjectFileItem*>(item)->FileType)
               {
               case FileType::Script:   iImage = 2;  break;
               case FileType::Language: iImage = 3;  break;
               case FileType::Mission:  iImage = 4;  break;
               case FileType::Unknown:  iImage = 5;  break;
               }
               break;
            }
            iSelectedImage = iImage;

            // Fixed: Bold + Expanded
            if (item->Fixed)
               state = stateMask = TVIS_BOLD | TVIS_EXPANDED;

            // Item data:
            lParam = reinterpret_cast<LPARAM>(item);
         }

         // --------------------- PROPERTIES ------------------------
      public:
         PROPERTY_GET(ProjectItem*,Data,GetData);

         // ---------------------- ACCESSORS ------------------------			
      public:
         /// <summary>Gets the item data (if any).</summary>
         /// <returns></returns>
         ProjectItem*  GetData() const
         {
            return mask & TVIF_PARAM ? reinterpret_cast<ProjectItem*>(lParam) : nullptr;
         }

         /// <summary>Allow any unfixed file/folder/variable to be dragged</summary>
         /// <returns></returns>
         bool IsDragSource() const
         {
            return Data && !Data->Fixed;
         }

         /// <summary>Allow items to be dropped onto folders</summary>
         /// <returns></returns>
         bool IsDropTarget() const
         {
            return Data && Data->Type == ProjectItemType::Folder;
         }

         /// <summary>Allow root and unfixed files/folder/variables to be renamed</summary>
         /// <returns></returns>
         bool IsEditable() const
         {
            return !Data || !Data->Fixed;
         }

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Assign from tree item</summary>
         /// <param name="r">The r.</param>
         /// <returns></returns>
         TreeItem& operator=(const TVITEM& r)
         {
            TVItem::operator=(r);
            return *this;
         }

         // -------------------- REPRESENTATION ---------------------
      private:
      };


      // --------------------- CONSTRUCTION ----------------------

   public:
      ProjectTreeCtrl();
      virtual ~ProjectTreeCtrl();
		 
		NO_COPY(ProjectTreeCtrl);	// No copy semantics
		NO_MOVE(ProjectTreeCtrl);	// No move semantics

      // ------------------------ STATIC -------------------------
      
      DECLARE_MESSAGE_MAP()

      // --------------------- PROPERTIES ------------------------
			
      // ---------------------- ACCESSORS ------------------------			
   
      // ----------------------- MUTATORS ------------------------
   public:
      void  AddFolder();
      void  Populate();

   protected:
      HTREEITEM     InsertItem(TreeItem& item, HTREEITEM parent);

      afx_msg void  OnDragBegin(NMHDR *pNMHDR, LRESULT *pResult);
      handler void  OnDragEnd(const TreeItem& target);
      afx_msg void  OnLabelEditBegin(NMHDR* pNMHDR, LRESULT* pResult);
      afx_msg void  OnLabelEditEnd(NMHDR *pNMHDR, LRESULT *pResult);
      handler BOOL  OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) override;

      // -------------------- REPRESENTATION ---------------------

   private:
      CImageList*  DragIcon;
      TreeItem     DragSource;
      
      
   public:
      afx_msg void OnMouseMove(UINT nFlags, CPoint point);
      afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};
   
/// <summary>User interface controls</summary>
NAMESPACE_END2(GUI,Controls)

