#pragma once
#include "GameDataWnd.h"
#include "Logic/SyntaxLibrary.h"
#include "Logic/DescriptionLibrary.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Windows)
   
   /// <summary>Dockable game data window</summary>
   class CCommandWnd : public CGameDataWnd
   {
      // ------------------------ TYPES --------------------------
   protected:
      /// <summary>Tooltip data for a script command</summary>
      class CommandTooltipData : public CustomTooltip::TooltipData
      {
      public:
         /// <summary>Create script command tooltip</summary>
         /// <param name="cmd">Command.</param>
         CommandTooltipData(CommandSyntaxRef cmd) : TooltipData(IDR_GAME_OBJECTS, 32)
         {
            try
            {
               // Lookup description 
               DescriptionSource = DescriptionLib.Commands.Find(cmd);
               LabelSource = cmd.GetRichText(true);

               // TODO: Supply icon
            }
            // Undocumented:
            catch (ExceptionBase&) {
               ResetTo(CustomTooltip::UndocumentedCommand);
            }
         }
      };
	  
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

      void    OnRequestTooltip(CustomTooltip::TooltipData* data) override;

      // -------------------- REPRESENTATION ---------------------

   protected:
   };
   
NAMESPACE_END2(GUI,Windows)

