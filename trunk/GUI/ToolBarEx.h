#pragma once
#include "afxtoolbar.h"

/// <summary>User interface controls</summary>
NAMESPACE_BEGIN2(GUI,Controls)
   
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
      BOOL Create(CWnd* parent, UINT nID, wstring name, bool routeFrame = true, UINT nDisabledResID = 0);

      // -------------------- REPRESENTATION ---------------------
      
   private:
   };
   

NAMESPACE_END2(GUI,Controls)
