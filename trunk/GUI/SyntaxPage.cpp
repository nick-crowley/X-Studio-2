
#include "stdafx.h"
#include "SyntaxPage.h"
#include "afxdialogex.h"


/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Preferences)

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNAMIC(SyntaxPage, PreferencesPage)

   BEGIN_MESSAGE_MAP(SyntaxPage, PreferencesPage)
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   SyntaxPage::SyntaxPage() 
      : PreferencesPage(SyntaxPage::IDD),
        
        ArgumentColour(nullptr),
        CommentColour(nullptr),
        ConstantColour(nullptr),
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

   SyntaxPage::~SyntaxPage()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   /// <summary>Performs data exchange.</summary>
   /// <param name="pDX">The dx.</param>
   void SyntaxPage::DoDataExchange(CDataExchange* pDX)
   {
	   __super::DoDataExchange(pDX);
   }


   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Save values.</summary>
   void SyntaxPage::Commit()
   {
      // Highlighting
      PrefsLib.ArgumentColour = ArgumentColour->GetColor();
      PrefsLib.CommentColour = CommentColour->GetColor();
      PrefsLib.ConstantColour = ConstantColour->GetColor();
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
   void SyntaxPage::Populate()
   {
      // Highlighting
      auto group = new PropertyBase(*this, L"Syntax Highlighting");
      group->AddSubItem(ArgumentColour = new ArgumentColourProperty(*this));
      group->AddSubItem(CommentColour = new CommentColourProperty(*this));
      group->AddSubItem(ConstantColour = new ConstantColourProperty(*this));
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

