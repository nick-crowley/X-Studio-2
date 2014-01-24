#pragma once
#include "afxcmn.h"


/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Controls)
   
   /// <summary></summary>
   class SuggestionList : public CListCtrl
   {
      // ------------------------ TYPES --------------------------
   private:
	  
      // --------------------- CONSTRUCTION ----------------------
   protected:
      SuggestionList();    // Protected constructor used by dynamic creation
   public:
      virtual ~SuggestionList();
       
      // ------------------------ STATIC -------------------------
   public:
      DECLARE_DYNCREATE(SuggestionList)
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
   public:
      BOOL Create(CWnd* parent, CPoint& pt);

   protected:
      void AdjustLayout();
	  
      afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	   afx_msg void OnSize(UINT nType, int cx, int cy);
	  
      // -------------------- REPRESENTATION ---------------------
      
   private:
      const CSize DefaultSize = CSize(200,100);
   
      
   };
   

NAMESPACE_END2(GUI,Controls)
