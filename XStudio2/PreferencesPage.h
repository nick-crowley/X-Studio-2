#pragma once
#include "PreferencesPage.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Preferences)
   
   /// <summary>Base class for all preference pages</summary>
   class PreferencesPage : public CMFCPropertyPage
   {
      // ------------------------ TYPES --------------------------
   public:

      // --------------------- CONSTRUCTION ----------------------
   public:
      PreferencesPage(UINT nID);    
      virtual ~PreferencesPage();
       
      // ------------------------ STATIC -------------------------
      DECLARE_DYNAMIC(PreferencesPage)
      DECLARE_MESSAGE_MAP()
	  
      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
   public:

      // ----------------------- MUTATORS ------------------------
   protected:
      void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	  
      // -------------------- REPRESENTATION ---------------------
   protected:
   };
   
NAMESPACE_END2(GUI,Preferences)

