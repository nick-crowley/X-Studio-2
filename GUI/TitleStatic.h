#pragma once

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Controls)
   
   /// <summary></summary>
   class TitleStatic : public CStatic
   {
      // ------------------------ TYPES --------------------------
   protected:
      static const COLORREF  TEXT_COLOUR = RGB(42,84,153);
      static const UINT      TEXT_SIZE = 14;
	  
      // --------------------- CONSTRUCTION ----------------------
   public:
      TitleStatic();
      virtual ~TitleStatic();
       
      // ------------------------ STATIC -------------------------
      DECLARE_DYNCREATE(TitleStatic)
      DECLARE_MESSAGE_MAP()
	  
      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
   public:
      // ----------------------- MUTATORS ------------------------
   protected:
      void AdjustLayout();
      void DrawItem(LPDRAWITEMSTRUCT draw) override;
	  
	  afx_msg void OnSize(UINT nType, int cx, int cy);
	  
      // -------------------- REPRESENTATION ---------------------
   protected:
      CFont TitleFont;
   };
   

NAMESPACE_END2(GUI,Controls)
