
#include "stdafx.h"
#include "EditorPage.h"
#include "afxdialogex.h"


/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Preferences)

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNAMIC(EditorPage, PreferencesPage)

   BEGIN_MESSAGE_MAP(EditorPage, PreferencesPage)
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   EditorPage::EditorPage() 
      : PreferencesPage(EditorPage::IDD),
        
        ScriptViewFont(nullptr),
        ShowLineNumbers(nullptr),
        ShowScriptTooltips(nullptr),
        ScriptTooltipDelay(nullptr),
        ScriptIndentation(nullptr),

        CommitOnSave(nullptr),
        IncrementOnSave(nullptr),

        BackgroundColour(nullptr),
        LineNumberColour(nullptr),
        DifferenceHighlight(nullptr),
        RefactorHighlight(nullptr)
   {
   }

   EditorPage::~EditorPage()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   /// <summary>Performs data exchange.</summary>
   /// <param name="pDX">The dx.</param>
   void EditorPage::DoDataExchange(CDataExchange* pDX)
   {
	   __super::DoDataExchange(pDX);
   }


   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Save values.</summary>
   void EditorPage::Commit()
   {
      // Editor
      PrefsLib.ShowLineNumbers = ShowLineNumbers->GetBool();
      PrefsLib.ShowScriptTooltips = ShowScriptTooltips->GetBool();
      PrefsLib.ScriptViewFont = *ScriptViewFont->GetLogFont();
      PrefsLib.ScriptTooltipDelay = ScriptTooltipDelay->GetInt();
      PrefsLib.ScriptIndentation = ScriptIndentation->GetInt();

      // Backups
      PrefsLib.CommitOnSave = CommitOnSave->GetBool();
      PrefsLib.IncrementOnSave = IncrementOnSave->GetBool();
      
      // Colour
      PrefsLib.BackgroundColour = BackgroundColour->GetColor();
      PrefsLib.LineNumberColour = LineNumberColour->GetColor();
      PrefsLib.DifferenceHighlight = DifferenceHighlight->GetColor();
      PrefsLib.RefactorHighlight = RefactorHighlight->GetColor();
      
   }

   /// <summary>Populates page.</summary>
   void EditorPage::Populate()
   {
      // Editor
      auto group = new PropertyBase(*this, L"Editor");
      group->AddSubItem(ScriptViewFont = new ScriptViewFontProperty(*this));
      group->AddSubItem(ShowLineNumbers = new ShowLineNumbersProperty(*this));
      group->AddSubItem(ShowScriptTooltips = new ShowScriptTooltipsProperty(*this));
      group->AddSubItem(ScriptTooltipDelay = new ScriptTooltipDelayProperty(*this));
      group->AddSubItem(ScriptIndentation = new ScriptIndentationProperty(*this));
      Grid.AddProperty(group);

      // Backup
      group = new PropertyBase(*this, L"Version Control");
      group->AddSubItem(CommitOnSave = new CommitOnSaveProperty(*this));
      group->AddSubItem(IncrementOnSave = new IncrementOnSaveProperty(*this));
      Grid.AddProperty(group);

      // Colours
      group = new PropertyBase(*this, L"Colours");
      group->AddSubItem(BackgroundColour = new BackgroundColourProperty(*this));
      group->AddSubItem(LineNumberColour = new LineNumberColourProperty(*this));
      group->AddSubItem(DifferenceHighlight = new DifferenceHighlightProperty(*this));
      group->AddSubItem(RefactorHighlight = new RefactorHighlightProperty(*this));
      Grid.AddProperty(group);
      
   }
   
   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
NAMESPACE_END2(GUI,Preferences)

