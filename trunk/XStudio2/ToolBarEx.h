#pragma once
#include "afxtoolbar.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN(GUI)
   
   /// <summary></summary>
   class ToolBarEx : public CMFCToolBar
   {
      // ------------------------ TYPES --------------------------
   private:
	  
      // --------------------- CONSTRUCTION ----------------------
   public:
      ToolBarEx();  
      virtual ~ToolBarEx();
       
      // ------------------------ STATIC -------------------------

      DECLARE_DYNCREATE(ToolBarEx)
	  
      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
   public: 
      // ----------------------- MUTATORS ------------------------
   public:
      BOOL Create(CWnd* parent, UINT nID, UINT nColdResID = 0, UINT nDisabledResID = 0);

      // -------------------- REPRESENTATION ---------------------
      
   private:
   };
   

NAMESPACE_END(GUI)
