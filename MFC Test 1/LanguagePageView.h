#pragma once

#include "afxcview.h"
#include "LanguageDocument.h"
#include "Logic/Event.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Views)

   // ----------------- EVENTS AND DELEGATES ------------------

   typedef Event<LanguagePage&>         PageClickEvent;
   typedef PageClickEvent::DelegatePtr  PageClickHandler;
   
   // ----------------------- CLASSES -------------------------

   /// <summary></summary>
   class LanguagePageView : public CListView
   {
      // ------------------------ TYPES --------------------------
   private:
	  
      // --------------------- CONSTRUCTION ----------------------
   protected:
      LanguagePageView();    // Protected constructor used by dynamic creation
   public:
      virtual ~LanguagePageView();
       
      // ------------------------ STATIC -------------------------
   public:
      DECLARE_DYNCREATE(LanguagePageView)
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
   public:
      PageClickEvent    PageClick;

   private:
      
   public:
      afx_msg void OnLvnItemActivate(NMHDR *pNMHDR, LRESULT *pResult);
   };
   
   #ifndef _DEBUG  
   inline LanguageDocument* LanguagePageView::GetDocument() const
      { return reinterpret_cast<LanguageDocument*>(m_pDocument); }
   #endif

NAMESPACE_END2(GUI,Views)
