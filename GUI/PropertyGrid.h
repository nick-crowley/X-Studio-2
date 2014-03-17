#pragma once

/// <summary>User interface controls</summary>
NAMESPACE_BEGIN2(GUI,Controls)

   /// <summary>Find/Replace terms comboBox</summary>
   class PropertyGrid : public CMFCPropertyGridCtrl
   {
      // ------------------------ TYPES --------------------------
   private:
	  
      // --------------------- CONSTRUCTION ----------------------
      
   public:
      PropertyGrid();
	   virtual ~PropertyGrid();

      // ------------------------ STATIC -------------------------
	   
      DECLARE_MESSAGE_MAP()

      // --------------------- PROPERTIES ------------------------
			
      // ---------------------- ACCESSORS ------------------------	
   public:
      
      // ----------------------- MUTATORS ------------------------
   protected:
      //BOOL ValidateItemData(CMFCPropertyGridProperty* pProp) override;

      // -------------------- REPRESENTATION ---------------------
   protected:
   };


NAMESPACE_END2(GUI,Controls)

