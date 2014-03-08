#pragma once
#include "GameDataWnd.h"
#include "Logic/SyntaxLibrary.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)
   
   /// <summary>Dockable game data window</summary>
   class CCommandWnd : public CGameDataWnd
   {
      // ------------------------ TYPES --------------------------
   private:
	  
      // --------------------- CONSTRUCTION ----------------------
      
   public:
      CCommandWnd();
      virtual ~CCommandWnd();
       
      // ------------------------ STATIC -------------------------
   protected:
      DECLARE_MESSAGE_MAP()
      
      // --------------------- PROPERTIES ------------------------
	   
      // ---------------------- ACCESSORS ------------------------			
      
      // ----------------------- MUTATORS ------------------------
   public:
      void Create(CWnd* parent);

   protected:
      wstring GetItemText(UINT index) override;
      void    PopulateGroupCombo() override;
      void    PopulateItems(const wstring& searchTerm, UINT selectedGroup) override;

      // -------------------- REPRESENTATION ---------------------

   protected:
   };
   
NAMESPACE_END2(GUI,Windows)

