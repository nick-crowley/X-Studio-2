#pragma once

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Controls)
   
   /// <summary></summary>
   class BitmapStatic : public CStatic
   {
      // ------------------------ TYPES --------------------------
   private:
	  
      // --------------------- CONSTRUCTION ----------------------
   public:
      BitmapStatic();
      BitmapStatic(UINT id, bool alpha = false);
      virtual ~BitmapStatic();
       
      // ------------------------ STATIC -------------------------
      DECLARE_DYNCREATE(BitmapStatic)
      DECLARE_MESSAGE_MAP()
	  
      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
   public:
      // ----------------------- MUTATORS ------------------------
   public:
      void ShinkToFit();

   protected:
      void AdjustLayout();
      void DrawItem(LPDRAWITEMSTRUCT draw) override;
	  
	  afx_msg void OnSize(UINT nType, int cx, int cy);
	  
      // -------------------- REPRESENTATION ---------------------
   protected:
      UINT    BitmapID;
      CImage  Image;
      bool    Alpha;
   };
   

NAMESPACE_END2(GUI,Controls)
