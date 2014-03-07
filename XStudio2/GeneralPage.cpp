#include "stdafx.h"
#include "GeneralPage.h"
#include "afxdialogex.h"


/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Preferences)

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNAMIC(GeneralPage, PreferencesPage)

   BEGIN_MESSAGE_MAP(GeneralPage, PreferencesPage)
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   GeneralPage::GeneralPage() 
      : PreferencesPage(GeneralPage::IDD),
        LargeMenus(nullptr), 
        LargeToolbars(nullptr), 
        ToolWindowFont(nullptr), 
        TooltipFont(nullptr),
        GameDataFolder(nullptr), 
        GameDataLanguage(nullptr),
        GameDataVersion(nullptr), 
        SkipBrokenFiles(nullptr)
   {
   }

   GeneralPage::~GeneralPage()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   /// <summary>Performs data exchange.</summary>
   /// <param name="pDX">The dx.</param>
   void GeneralPage::DoDataExchange(CDataExchange* pDX)
   {
	   __super::DoDataExchange(pDX);
   }
   
   void GeneralPage::OnOK()
   {
      Console << "GeneralPage::OnOK() LargeToolbars=" << LargeToolbars->GetBool() << ENDL;

      __super::OnOK();
   }


   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Commits values to preferences library.</summary>
   void GeneralPage::Commit()
   {
      // General
      PrefsLib.LargeMenus = LargeMenus->GetBool();
      PrefsLib.LargeToolbars = LargeToolbars->GetBool();
      PrefsLib.ToolWindowFont = *ToolWindowFont->GetLogFont();
      PrefsLib.TooltipFont = *TooltipFont->GetLogFont();

      // Game Data
      PrefsLib.GameDataFolder = GameDataFolder->GetFolder();
      PrefsLib.SkipBrokenFiles = SkipBrokenFiles->GetBool();
      PrefsLib.GameDataLanguage = GameDataLanguage->GetLanguage();
      PrefsLib.GameDataVersion = GameDataVersion->GetVersion();

   }

   /// <summary>Populates this page.</summary>
   void GeneralPage::Populate()
   {
      // General
      auto group = new PropertyBase(*this, L"General");
      group->AddSubItem(LargeMenus = new LargeMenusProperty(*this));
      group->AddSubItem(LargeToolbars = new LargeToolbarsProperty(*this));
      group->AddSubItem(ToolWindowFont = new ToolWindowFontProperty(*this));
      group->AddSubItem(TooltipFont = new TooltipFontProperty(*this));
      Grid.AddProperty(group);

      // Game Data
      group = new PropertyBase(*this, L"Game Data");
      group->AddSubItem(GameDataFolder = new GameDataFolderProperty(*this));
      group->AddSubItem(SkipBrokenFiles = new SkipBrokenFilesProperty(*this));
      group->AddSubItem(GameDataLanguage = new GameLanguageProperty(*this));
      group->AddSubItem(GameDataVersion = new GameVersionProperty(*this));
      Grid.AddProperty(group);
   }

   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
NAMESPACE_END2(GUI,Preferences)


