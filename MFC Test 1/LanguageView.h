#pragma once


/// <summary>User interface</summary>
NAMESPACE_BEGIN(GUI)

   /// <summary>Language document view</summary>
   class LanguageView : public CFormView
   {
      // ------------------------ TYPES --------------------------
   public:
	   enum { IDD = IDR_LANGUAGEVIEW };
	  
      // --------------------- CONSTRUCTION ----------------------
   protected:
	   LanguageView();           // protected constructor used by dynamic creation
	   virtual ~LanguageView();

      // ------------------------ STATIC -------------------------
   public:
      DECLARE_DYNCREATE(LanguageView)
   protected:
      DECLARE_MESSAGE_MAP()

      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
   public:
   #ifdef _DEBUG
	   virtual void AssertValid() const;
	   virtual void Dump(CDumpContext& dc) const;
   #endif   

      // ----------------------- MUTATORS ------------------------
   protected:
	   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support   

      // -------------------- REPRESENTATION ---------------------
   };


NAMESPACE_END(GUI)

