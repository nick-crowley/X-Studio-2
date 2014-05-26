
#pragma once

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)
   
   /// <summary></summary>
   class SplashWnd : public CWnd
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
   public:
      BOOL Create(CWnd* parent, UINT nID);

   protected:
      void AdjustLayout();
	   
      afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
      afx_msg void OnPaint();
      afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	   afx_msg void OnSize(UINT nType, int cx, int cy);
	   afx_msg void OnTimer(UINT_PTR nIDEvent);

      // -------------------- REPRESENTATION ---------------------
   protected:

   };
   

NAMESPACE_END2(GUI,Windows)
