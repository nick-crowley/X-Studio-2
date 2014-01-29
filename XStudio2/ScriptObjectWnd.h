#pragma once
#include "GameDataWnd.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN(GUI)
   
   /// <summary>Dockable game data window</summary>
   class CScriptObjectWnd : public CGameDataWnd
   {
      // ------------------------ TYPES --------------------------
   private:
	  
      // --------------------- CONSTRUCTION ----------------------
      
   public:
      CScriptObjectWnd();
      virtual ~CScriptObjectWnd();
       
      // ------------------------ STATIC -------------------------
   protected:
      DECLARE_MESSAGE_MAP()
      
      // --------------------- PROPERTIES ------------------------
	   
      // ---------------------- ACCESSORS ------------------------			
      
      // ----------------------- MUTATORS ------------------------
   protected:
      virtual void PopulateGroupCombo();
      virtual void PopulateItems(const wstring& searchTerm, UINT selectedGroup);

      // -------------------- REPRESENTATION ---------------------

   protected:
   };
   
NAMESPACE_END(GUI)

