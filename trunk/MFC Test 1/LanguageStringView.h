#pragma once

#include "afxcview.h"
#include "LanguageDocument.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Views)
   
   /// <summary></summary>
   class LanguageStringView : public CListView
   {
      // ------------------------ TYPES --------------------------
   private:
	  
      // --------------------- CONSTRUCTION ----------------------
   protected:
      LanguageStringView();    // Protected constructor used by dynamic creation
   public:
      virtual ~LanguageStringView();
       
      // ------------------------ STATIC -------------------------
   public:
      DECLARE_DYNCREATE(LanguageStringView)
   protected:
      DECLARE_MESSAGE_MAP()
	  
      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
   public:
   #ifdef _DEBUG
	   virtual void AssertValid() const;
	   virtual void Dump(CDumpContext& dc) const;
   #endif  
      LanguageDocument* GetDocument() const;

      // ----------------------- MUTATORS ------------------------
   protected:
      void AdjustLayout();
	  
      virtual void OnInitialUpdate();
	   afx_msg void OnSize(UINT nType, int cx, int cy);
	  
      // -------------------- REPRESENTATION ---------------------
   private:
   };
   
   #ifndef _DEBUG  
   inline LanguageDocument* LanguageStringView::GetDocument() const
      { return reinterpret_cast<LanguageDocument*>(m_pDocument); }
   #endif

NAMESPACE_END2(GUI,Views)
