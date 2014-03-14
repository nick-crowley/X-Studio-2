
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
        RefactorHighlight(nullptr),
        
        ArgumentColour(nullptr),
        CommentColour(nullptr),
        GameObjectColour(nullptr),
        KeywordColour(nullptr),
        LabelColour(nullptr),
        NumberColour(nullptr),
        NullColour(nullptr),
        OperatorColour(nullptr),
        ScriptObjectColour(nullptr),
        StringColour(nullptr),
        TextColour(nullptr),
        VariableColour(nullptr)
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
      
      // Highlighting
      PrefsLib.ArgumentColour = ArgumentColour->GetColor();
      PrefsLib.CommentColour = CommentColour->GetColor();
      PrefsLib.GameObjectColour = GameObjectColour->GetColor();
      PrefsLib.KeywordColour = KeywordColour->GetColor();
      PrefsLib.LabelColour = LabelColour->GetColor();
      PrefsLib.NumberColour = NumberColour->GetColor();
      PrefsLib.NullColour = NullColour->GetColor();
      PrefsLib.OperatorColour = OperatorColour->GetColor();
      PrefsLib.ScriptObjectColour = ScriptObjectColour->GetColor();
      PrefsLib.StringColour = StringColour->GetColor();
      PrefsLib.TextColour = TextColour->GetColor();
      PrefsLib.VariableColour = VariableColour->GetColor();
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
      
      // Highlighting
      group = new PropertyBase(*this, L"Syntax Highlighting");
      group->AddSubItem(ArgumentColour = new ArgumentColourProperty(*this));
      group->AddSubItem(CommentColour = new CommentColourProperty(*this));
      group->AddSubItem(GameObjectColour = new GameObjectColourProperty(*this));
      group->AddSubItem(KeywordColour = new KeywordColourProperty(*this));
      group->AddSubItem(LabelColour = new LabelColourProperty(*this));
      group->AddSubItem(NumberColour = new NumberColourProperty(*this));
      group->AddSubItem(NullColour = new NullColourProperty(*this));
      group->AddSubItem(OperatorColour = new OperatorColourProperty(*this));
      group->AddSubItem(ScriptObjectColour = new ScriptObjectColourProperty(*this));
      group->AddSubItem(StringColour = new StringColourProperty(*this));
      group->AddSubItem(TextColour = new TextColourProperty(*this));
      group->AddSubItem(VariableColour = new VariableColourProperty(*this));
      Grid.AddProperty(group);
   }
   
   // ------------------------------- PRIVATE METHODS ------------------------------
   
   
NAMESPACE_END2(GUI,Preferences)

