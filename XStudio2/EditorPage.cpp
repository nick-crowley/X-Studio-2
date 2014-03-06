
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
        BackgroundColour(nullptr),
        LineNumberColour(nullptr),

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
   
   /// <summary>Stub.</summary>
   void EditorPage::Commit()
   {
   }

   /// <summary>Populates this page.</summary>
   void EditorPage::Populate()
   {
      // General
      auto group = new PropertyBase(*this, L"General");
      group->AddSubItem(ShowLineNumbers = new ShowLineNumbersProperty(*this));
      group->AddSubItem(ScriptViewFont = new ScriptViewFontProperty(*this));
      group->AddSubItem(BackgroundColour = new BackgroundColourProperty(*this));
      group->AddSubItem(LineNumberColour = new LineNumberColourProperty(*this));
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

