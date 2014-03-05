#pragma once
#include "PreferencesPage.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Preferences)
   
   /// <summary></summary>
   class EditorPage : public PreferencesPage
   {
      // ------------------------ TYPES --------------------------
   public:
      enum { IDD = IDD_EDITOR_PAGE };

   protected:
      /// <summary>Show line numbers property</summary>
      class ShowLineNumbersProperty : public BooleanProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create 'show line numbers' property</summary>
         /// <param name="page">Owner page.</param>
         ShowLineNumbersProperty(PreferencesPage& page) 
            : BooleanProperty(page, L"Show Line Numbers", PrefsLib.ShowLineNumbers, L"Display line numbers in the editor")
         {}
      };

      // --------------------- CONSTRUCTION ----------------------
   public:
      EditorPage();    
      virtual ~EditorPage();
       
      // ------------------------ STATIC -------------------------
      DECLARE_DYNAMIC(EditorPage)
      DECLARE_MESSAGE_MAP()
	  
      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
   public:

      // ----------------------- MUTATORS ------------------------
   protected:
      void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
      void Populate() override;
	  
      // -------------------- REPRESENTATION ---------------------
   protected:
      ShowLineNumbersProperty*   ShowLineNumbers;
   };
   
NAMESPACE_END2(GUI,Preferences)
