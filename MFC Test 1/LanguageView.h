#pragma once
#include "afxcmn.h"


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
      void AdjustLayout();

	   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support   
      virtual void OnInitialUpdate();
      afx_msg void OnSize(UINT nType, int cx, int cy);

      // -------------------- REPRESENTATION ---------------------
   public:
      CListCtrl PageList;
      CListCtrl StringList;
      CRichEditCtrl StringEdit;
      
   };


NAMESPACE_END(GUI)

