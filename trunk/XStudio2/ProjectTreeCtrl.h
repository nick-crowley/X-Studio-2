#pragma once

/// <summary>User interface</summary>
FORWARD_DECLARATION2(Logic,Projects,class ProjectItem)

/// <summary>User interface controls</summary>
NAMESPACE_BEGIN2(GUI,Controls)

   /// <summary></summary>
   class ProjectTreeCtrl : public CTreeCtrl
   {
      // ------------------------ TYPES --------------------------
   private:
	  
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
      int       GetItemIcon(Logic::Projects::ProjectItem* item);
      HTREEITEM InsertItem(Logic::Projects::ProjectItem* item, HTREEITEM parent);
      BOOL      OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) override;

      // -------------------- REPRESENTATION ---------------------

   private:
   };
   
/// <summary>User interface controls</summary>
NAMESPACE_END2(GUI,Controls)

