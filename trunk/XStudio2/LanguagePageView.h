#pragma once

#include "afxcview.h"
#include "LanguageDocument.h"
#include "Logic/Event.h"
#include "ImageListEx.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Views)

   // ----------------- EVENTS AND DELEGATES ------------------

   typedef Event<>                             SelectionChangedEvent;
   typedef SelectionChangedEvent::DelegatePtr  SelectionChangedHandler;
   
   // ----------------------- CLASSES -------------------------

   /// <summary></summary>
   class LanguagePageView : public CListView
   {
      // ------------------------ TYPES --------------------------
   private:
      typedef LanguageFile::PageCollection PageCollection;
	  
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
      LanguageDocument* GetDocument() const;
      LanguagePage*     GetSelected() const;
      PageCollection&   GetDataSource() const;

      // ----------------------- MUTATORS ------------------------
   protected:
      void AdjustLayout();
      void GenerateLibrary();
      void Populate();
	  
      virtual void OnInitialUpdate();
      afx_msg void OnItemStateChanged(NMHDR *pNMHDR, LRESULT *pResult);
	   afx_msg void OnSize(UINT nType, int cx, int cy);
	  
      // -------------------- REPRESENTATION ---------------------
   public:
      SelectionChangedEvent   SelectionChanged;
      PageCollection          Library;
   
   private:
      ImageListEx   Images;
      
   };
   

NAMESPACE_END2(GUI,Views)
