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
	  
      // -------------------- REPRESENTATION ---------------------
   protected:
   };
   
NAMESPACE_END2(GUI,Preferences)
