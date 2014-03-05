#pragma once
#include "PreferencesPage.h"


/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Preferences)
   
   /// <summary></summary>
   class GeneralPage : public PreferencesPage
   {
      // ------------------------ TYPES --------------------------
   public:
      enum { IDD = IDD_GENERAL_PAGE };

   protected:
      /// <summary>Game data folder property</summary>
      class GameDataFolderProperty : public FolderProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create 'game data folder' property</summary>
         /// <param name="page">Owner page.</param>
         GameDataFolderProperty(PreferencesPage& page) 
            : FolderProperty(page, L"Game Folder", PrefsLib.GameDataFolder, L"Folder containing X3 executable")
         {}
      };

      /// <summary>Game version property</summary>
      class GameVersionProperty : public PropertyBase
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create game version property.</summary>
         /// <param name="page">Owner page.</param>
         GameVersionProperty(PreferencesPage& page)
            : PropertyBase(page, L"Game Version", VersionString(PrefsLib.GameDataVersion).c_str(),  L"Game version")  
         {
            // Populate game versions
            AddOption(VersionString(GameVersion::Threat).c_str(), FALSE);
            AddOption(VersionString(GameVersion::Reunion).c_str(), FALSE);
            AddOption(VersionString(GameVersion::TerranConflict).c_str(), FALSE);
            AddOption(VersionString(GameVersion::AlbionPrelude).c_str(), FALSE);

            // Strict dropdown
            AllowEdit(FALSE);
         }

         // ---------------------- ACCESSORS ------------------------	
      public:
         /// <summary>Gets the selected version.</summary>
         /// <returns></returns>
         GameVersion  GetVersion() const
         {
            return GameVersionIndex(Find(GetString())).Version;
         }
         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------
      protected:
      };

      /// <summary>Large menus property</summary>
      class LargeMenusProperty : public BooleanProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create 'large menus' property</summary>
         /// <param name="page">Owner page.</param>
         LargeMenusProperty(PreferencesPage& page) 
            : BooleanProperty(page, L"Large Menus", PrefsLib.LargeMenus, L"Enlarge all menu items and icons")
         {}
      };

      /// <summary>Large toolbars property</summary>
      class LargeToolbarsProperty : public BooleanProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create 'large toolbars' property</summary>
         /// <param name="page">Owner page.</param>
         LargeToolbarsProperty(PreferencesPage& page) 
            : BooleanProperty(page, L"Large Toolbars", PrefsLib.LargeToolbars, L"Enlarge all toolbar buttons")
         {}
      };

      /// <summary>Tool window font property</summary>
      class ToolWindowFontProperty : public FontProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create 'tool window font' property</summary>
         /// <param name="page">Owner page.</param>
         ToolWindowFontProperty(PreferencesPage& page) 
            : FontProperty(page, L"Tool Window Font", PrefsLib.ToolWindowFont, L"Font used in all tool windows")
         {}
      };

	  
      // --------------------- CONSTRUCTION ----------------------
   public:
      GeneralPage();    
      virtual ~GeneralPage();
       
      // ------------------------ STATIC -------------------------
      DECLARE_DYNAMIC(GeneralPage)
      DECLARE_MESSAGE_MAP()
	  
      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
   public:

      // ----------------------- MUTATORS ------------------------
   public:
      BOOL OnInitDialog() override;
      void OnOK() override;

   protected:
      void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	   void Populate() override;

      // -------------------- REPRESENTATION ---------------------
   protected:
      GameDataFolderProperty* GameDataFolder;
      GameVersionProperty*    GameDataVersion;
      LargeMenusProperty*     LargeMenus;
      LargeToolbarsProperty*  LargeToolbars;
      ToolWindowFontProperty* ToolWindowFont;
      
   public:
      
   };
   
NAMESPACE_END2(GUI,Preferences)

