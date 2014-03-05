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
   public:
      BOOL OnInitDialog() override;
      void OnOK() override;

   protected:
      void AdjustLayout();
      void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	   afx_msg void OnSize(UINT nType, int cx, int cy);

      // -------------------- REPRESENTATION ---------------------
   protected:
      CMFCPropertyGridCtrl  Grid;
   
   };
   
NAMESPACE_END2(GUI,Preferences)

