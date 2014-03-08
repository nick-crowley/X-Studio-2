
#pragma once

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)
   
   /// <summary></summary>
   class SplashWnd : public CDialog
   {
      // ------------------------ TYPES --------------------------
   protected:
      enum { IDD = IDD_SPLASH };
	  
      // --------------------- CONSTRUCTION ----------------------
   public:
      SplashWnd();
      virtual ~SplashWnd();
       
      // ------------------------ STATIC -------------------------
      DECLARE_DYNCREATE(SplashWnd)
      DECLARE_MESSAGE_MAP()
	  
      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
   public:

      // ----------------------- MUTATORS ------------------------
   protected:
      void AdjustLayout();
	  
	  afx_msg void OnSize(UINT nType, int cx, int cy);
	  
      // -------------------- REPRESENTATION ---------------------
      
   protected:
   public:
      afx_msg void OnPaint();
      afx_msg BOOL OnEraseBkgnd(CDC* pDC);
   };
   

NAMESPACE_END2(GUI,Windows)
