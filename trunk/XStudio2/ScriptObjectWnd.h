#pragma once
#include "GameDataWnd.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)
   
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
   public:
      void  Create(CWnd* parent);

   protected:
      wstring GetItemText(UINT index) override;
      void    PopulateGroupCombo() override;
      void    PopulateItems(const wstring& searchTerm, UINT selectedGroup) override;

      // -------------------- REPRESENTATION ---------------------

   protected:
   };
   
NAMESPACE_END2(GUI,Windows)

