#pragma once
#include "PreferencesPage.h"


/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Preferences)
   
   /// <summary></summary>
   class GeneralPage : public PreferencesPage
   {
      // ------------------------ TYPES --------------------------
   public:
      enum { IDD = IDD_GENERAL_PAGE };

   protected:
      /// <summary></summary>
      class LargeToolbarsProperty : public BooleanProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create property</summary>
         /// <param name="page">Owner page.</param>
         LargeToolbarsProperty(PreferencesPage& page) 
            : BooleanProperty(page, L"LargeToolbars", PrefsLib.LargeToolbars, L"Show large toolbars")
         {}
      };

      /// <summary></summary>
      class ToolWindowFontProperty : public FontProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create property</summary>
         /// <param name="page">Owner page.</param>
         ToolWindowFontProperty(PreferencesPage& page) 
            : FontProperty(page, L"Tool Window Font", PrefsLib.ToolWindowFont, L"Font used in all tool windows")
         {}
      };

	  
      // --------------------- CONSTRUCTION ----------------------
   public:
      GeneralPage();    
      virtual ~GeneralPage();
       
      // ------------------------ STATIC -------------------------
      DECLARE_DYNAMIC(GeneralPage)
      DECLARE_MESSAGE_MAP()
	  
      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
   public:

      // ----------------------- MUTATORS ------------------------
   public:
      BOOL OnInitDialog() override;
      void OnOK() override;

   protected:
      void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	   void Populate() override;

      // -------------------- REPRESENTATION ---------------------
   protected:
      LargeToolbarsProperty*  LargeToolbars;
      ToolWindowFontProperty* ToolWindowFont;
      
   public:
      
   };
   
NAMESPACE_END2(GUI,Preferences)

