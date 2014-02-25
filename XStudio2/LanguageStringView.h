#pragma once

#include "afxcview.h"
#include "LanguageDocument.h"
#include "LanguagePageView.h"
#include "ImageListEx.h"
#include "ListViewCustomDraw.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Views)
   
   /// <summary></summary>
   class LanguageStringView : public CListView
   {
      // ------------------------ TYPES --------------------------
   private:
      /// <summary>Custom draw implementation</summary>
      class StringCustomDraw : public ListViewCustomDraw
      {
      public:
         StringCustomDraw(LanguageStringView* view) : ListViewCustomDraw(view, view->GetListCtrl())
         {}

      protected:
         void  onDrawSubItem(CDC* dc, ItemData& item) override;
      };

      // --------------------- CONSTRUCTION ----------------------
   protected:
      LanguageStringView();    // Protected constructor used by dynamic creation
   public:
      virtual ~LanguageStringView();
       
      // ------------------------ STATIC -------------------------
      DECLARE_DYNCREATE(LanguageStringView)
      DECLARE_MESSAGE_MAP()
	  
      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
   public:
      LanguageDocument* GetDocument() const;
      LanguagePageView* GetPageView() const;
      LanguageString*   GetSelected() const;

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
   

NAMESPACE_END2(GUI,Views)
