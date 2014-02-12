#pragma once

#include "afxcmn.h"
#include "LanguageDocument.h"
#include "LanguageStringView.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Views)

   /// <summary>Language document view</summary>
   class LanguageEditView : public CFormView
   {
      // ------------------------ TYPES --------------------------
   public:
	   enum { IDD = IDR_LANGUAGEVIEW };
	  
      // --------------------- CONSTRUCTION ----------------------
   protected:
	   LanguageEditView();           // protected constructor used by dynamic creation
	   virtual ~LanguageEditView();

      // ------------------------ STATIC -------------------------
   public:
      DECLARE_DYNCREATE(LanguageEditView)
   protected:
      DECLARE_MESSAGE_MAP()

      static const COLORREF  MessageBackground = RGB(22,31,46);

      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
   public:
   #ifdef _DEBUG
	   virtual void AssertValid() const;
	   virtual void Dump(CDumpContext& dc) const;
   #endif   
      LanguageDocument* GetDocument() const;
      LanguageStringView* GetStringView() const;

      // ----------------------- MUTATORS ------------------------
   protected:
      void AdjustLayout();
      
      void onStringSelectionChanged();

      virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support   
      afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
      virtual void OnInitialUpdate();
      afx_msg void OnSize(UINT nType, int cx, int cy);
      
      // -------------------- REPRESENTATION ---------------------
   protected:
      SelectionChangedHandler  fnStringSelectionChanged;
      CRichEditCtrl            RichEdit;
      CMFCToolBar              ToolBar;
   public:
      afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
   };

   #ifndef _DEBUG  
   inline LanguageDocument* LanguageEditView::GetDocument() const
      { return reinterpret_cast<LanguageDocument*>(m_pDocument); }
   #endif


NAMESPACE_END2(GUI,Views)

