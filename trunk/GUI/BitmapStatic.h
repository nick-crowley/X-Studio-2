#pragma once

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Controls)
   
   /// <summary></summary>
   class BitmapStatic : public CStatic
   {
      // ------------------------ TYPES --------------------------
   private:
	  
      // --------------------- CONSTRUCTION ----------------------
   protected:
      BitmapStatic();    // Protected constructor used by dynamic creation
   public:
      virtual ~BitmapStatic();
       
      // ------------------------ STATIC -------------------------
      DECLARE_DYNCREATE(BitmapStatic)
      DECLARE_MESSAGE_MAP()
	  
      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
   public:
      // ----------------------- MUTATORS ------------------------
   protected:
      void AdjustLayout();
      void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) override;
	  
	  afx_msg void OnSize(UINT nType, int cx, int cy);
	  
      // -------------------- REPRESENTATION ---------------------
      
   private:
   };
   

NAMESPACE_END2(GUI,Controls)
