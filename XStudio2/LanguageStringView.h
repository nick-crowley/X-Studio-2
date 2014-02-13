#pragma once

#include "afxcview.h"
#include "LanguageDocument.h"
#include "LanguagePageView.h"
#include "ImageListEx.h"
#include "CustomDrawImpl.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Views)
   
   /// <summary></summary>
   class LanguageStringView : public CListView
   {
      // ------------------------ TYPES --------------------------
   private:
	   class StringCustomDraw : public CustomDrawImpl
      {
      public:
         StringCustomDraw(LanguageStringView* v) : CustomDrawImpl(v, (UINT)DrawCycle::Paint|(UINT)DrawCycle::Items|(UINT)DrawCycle::SubItems)
         {}

      protected:
         bool  onDrawItem(NMCUSTOMDRAW* pDraw, Stage stage) override;
         bool  onDrawSubItem(NMCUSTOMDRAW* pDraw, Stage stage) override;
      };

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
      LanguagePageView* GetPageView() const;
      LanguageString*   GetSelectedString() const;

      // ----------------------- MUTATORS ------------------------
   protected:
      void AdjustLayout();

      afx_msg void OnCustomDraw(NMHDR *pNMHDR, LRESULT *pResult);
      virtual void OnInitialUpdate();
      afx_msg void OnItemStateChanged(NMHDR *pNMHDR, LRESULT *pResult);
      handler void onPageSelectionChanged();
	   afx_msg void OnSize(UINT nType, int cx, int cy);
	  
      // -------------------- REPRESENTATION ---------------------
   public:
      SelectionChangedEvent    SelectionChanged;

   private:
      SelectionChangedHandler  fnPageSelectionChanged;
      ImageListEx              Images;
      StringCustomDraw         CustomDraw;
   };
   
   #ifndef _DEBUG  
   inline LanguageDocument* LanguageStringView::GetDocument() const
      { return reinterpret_cast<LanguageDocument*>(m_pDocument); }
   #endif

NAMESPACE_END2(GUI,Views)
