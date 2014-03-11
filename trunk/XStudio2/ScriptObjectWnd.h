#pragma once
#include "GameDataWnd.h"
#include "Logic/DescriptionLibrary.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)
   
   /// <summary>Dockable game data window</summary>
   class CScriptObjectWnd : public CGameDataWnd
   {
      // ------------------------ TYPES --------------------------
   protected:
      /// <summary>Tooltip data for a script-object</summary>
      class ScriptObjectTooltipData : public CustomTooltip::TooltipData
      {
      public:
         /// <summary>Creates script object tooltip data.</summary>
         /// <param name="obj">object</param>
         ScriptObjectTooltipData(ScriptObjectRef obj) : TooltipData(IDR_SCRIPT_OBJECTS, 32)
         {
            try
            {
               // Lookup description 
               DescriptionSource = DescriptionLib.Constants.Find(obj);
               LabelSource = GuiString(L"[center][b]%s[/b][/center]", obj.Text.c_str());

               // TODO: Supply icon
            }
            // Undocumented:
            catch (ExceptionBase&) {
               ResetTo(CustomTooltip::UndocumentedObject);
            }
         }
      };
	  
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

      void    OnRequestTooltip(CustomTooltip::TooltipData* data) override;

      // -------------------- REPRESENTATION ---------------------

   protected:
   };
   
NAMESPACE_END2(GUI,Windows)

