#pragma once
#include "PreferencesPage.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Preferences)
   
   /// <summary></summary>
   class CompilerPage : public PreferencesPage
   {
      // ------------------------ TYPES --------------------------
   public:
      enum { IDD = IDD_COMPILER_PAGE };

      // --------------------- CONSTRUCTION ----------------------
   public:
      CompilerPage();    
      virtual ~CompilerPage();
       
      // ------------------------ STATIC -------------------------
      DECLARE_DYNAMIC(CompilerPage)
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

