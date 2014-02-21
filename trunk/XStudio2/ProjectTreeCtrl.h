#pragma once
#include "Helpers.h"
#include "ProjectDocument.h"
//#include "Logic/ProjectFile.h"

/// <summary>User interface</summary>
//FORWARD_DECLARATION2(Logic,Projects,class ProjectItem)

/// <summary>User interface controls</summary>
NAMESPACE_BEGIN2(GUI,Controls)

   /// <summary>Project window treeview control</summary>
   class ProjectTreeCtrl : public CTreeCtrl
   {
      // ------------------------ TYPES --------------------------
   private:
      /// <summary>Item in the tree</summary>
	   class TreeItem : public TVItem
      {
      public:
         /// <summary>Create from project document</summary>
         /// <param name="item">The item.</param>
         TreeItem(ProjectDocument* doc) : TVItem(MAX_PATH), Data(nullptr)
         {
            // Use document title
            SetText((LPCWSTR)doc->GetTitle());

            // Set icon
            iImage = iSelectedImage = 1;
            
            // Bold + Expanded
            state = stateMask = TVIS_BOLD | TVIS_EXPANDED;
         }

         /// <summary>Create from project item</summary>
         /// <param name="item">The item.</param>
         TreeItem(ProjectItem* item) : TVItem(MAX_PATH), Data(item)
         {
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

      public:
         const ProjectItem*  Data;
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
      void  Populate();

   protected:
      HTREEITEM InsertItem(TreeItem& item, HTREEITEM parent);
      BOOL      OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) override;

      // -------------------- REPRESENTATION ---------------------

   private:
   };
   
/// <summary>User interface controls</summary>
NAMESPACE_END2(GUI,Controls)

