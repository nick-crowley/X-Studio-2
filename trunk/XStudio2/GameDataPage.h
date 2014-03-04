#pragma once
#include "PreferencesPage.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Preferences)
   
   /// <summary></summary>
   class GameDataPage : public PreferencesPage
   {
      // ------------------------ TYPES --------------------------
   public:
      enum { IDD = IDD_GAMEDATA_PAGE };

      // --------------------- CONSTRUCTION ----------------------
   public:
      GameDataPage();    
      virtual ~GameDataPage();
       
      // ------------------------ STATIC -------------------------
      DECLARE_DYNAMIC(GameDataPage)
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
