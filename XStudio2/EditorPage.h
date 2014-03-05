#pragma once
#include "PreferencesPage.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Preferences)
   
   /// <summary></summary>
   class EditorPage : public PreferencesPage
   {
      // ------------------------ TYPES --------------------------
   public:
      enum { IDD = IDD_EDITOR_PAGE };

   protected:
      /// <summary>Show line numbers property</summary>
      class ShowLineNumbersProperty : public BooleanProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create 'show line numbers' property</summary>
         /// <param name="page">Owner page.</param>
         ShowLineNumbersProperty(PreferencesPage& page) 
            : BooleanProperty(page, L"Show Line Numbers", PrefsLib.ShowLineNumbers, L"Display line numbers in the editor")
         {}
      };

      /// <summary>ScriptViewFont property</summary>
      class ScriptViewFontProperty : public FontProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create 'ScriptViewFont' property</summary>
         /// <param name="page">Owner page.</param>
         ScriptViewFontProperty(PreferencesPage& page) 
            : FontProperty(page, L"Window Font", PrefsLib.ScriptViewFont, L"Change the font used in the script editor")
         {}
      };
      
      /// <summary>LineNumber colour property</summary>
      class LineNumberColourProperty : public ColourProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create 'LineNumberColour' property</summary>
         /// <param name="page">Owner page.</param>
         LineNumberColourProperty(PreferencesPage& page) 
            : ColourProperty(page, L"Line Numbers", PrefsLib.LineNumberColour, L"Change the line number colour used in the script editor")
         {}
      };

      /// <summary>Background syntax highlighting colour property</summary>
      class BackgroundColourProperty : public ColourProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create 'BackgroundColour' property</summary>
         /// <param name="page">Owner page.</param>
         BackgroundColourProperty(PreferencesPage& page) 
            : ColourProperty(page, L"Background", PrefsLib.BackgroundColour, L"Change the background colour used in the script editor")
         {}
      };


      /// <summary>Argument syntax highlighting colour property</summary>
      class ArgumentColourProperty : public ColourProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create 'ArgumentColour' property</summary>
         /// <param name="page">Owner page.</param>
         ArgumentColourProperty(PreferencesPage& page) 
            : ColourProperty(page, L"Arguments", PrefsLib.ArgumentColour, L"Change the colour used to display Arguments")
         {}
      };

      /// <summary>Comment syntax highlighting colour property</summary>
      class CommentColourProperty : public ColourProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create 'CommentColour' property</summary>
         /// <param name="page">Owner page.</param>
         CommentColourProperty(PreferencesPage& page) 
            : ColourProperty(page, L"Comments", PrefsLib.CommentColour, L"Change the colour used to display comments")
         {}
      };
      
      /// <summary>GameObject syntax highlighting colour property</summary>
      class GameObjectColourProperty : public ColourProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create 'GameObjectColour' property</summary>
         /// <param name="page">Owner page.</param>
         GameObjectColourProperty(PreferencesPage& page) 
            : ColourProperty(page, L"Game Objects", PrefsLib.GameObjectColour, L"Change the colour used to display Game Objects")
         {}
      };

      /// <summary>Keyword syntax highlighting colour property</summary>
      class KeywordColourProperty : public ColourProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create 'KeywordColour' property</summary>
         /// <param name="page">Owner page.</param>
         KeywordColourProperty(PreferencesPage& page) 
            : ColourProperty(page, L"Keywords", PrefsLib.KeywordColour, L"Change the colour used to display Keywords")
         {}
      };

      /// <summary>Label syntax highlighting colour property</summary>
      class LabelColourProperty : public ColourProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create 'LabelColour' property</summary>
         /// <param name="page">Owner page.</param>
         LabelColourProperty(PreferencesPage& page) 
            : ColourProperty(page, L"Labels", PrefsLib.LabelColour, L"Change the colour used to display Labels")
         {}
      };
      
      /// <summary>Number syntax highlighting colour property</summary>
      class NumberColourProperty : public ColourProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create 'NumberColour' property</summary>
         /// <param name="page">Owner page.</param>
         NumberColourProperty(PreferencesPage& page) 
            : ColourProperty(page, L"Numbers", PrefsLib.NumberColour, L"Change the colour used to display Numbers")
         {}
      };

      /// <summary>Null syntax highlighting colour property</summary>
      class NullColourProperty : public ColourProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create 'NullColour' property</summary>
         /// <param name="page">Owner page.</param>
         NullColourProperty(PreferencesPage& page) 
            : ColourProperty(page, L"Nulls", PrefsLib.NullColour, L"Change the colour used to display Nulls")
         {}
      };

      /// <summary>Operator syntax highlighting colour property</summary>
      class OperatorColourProperty : public ColourProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create 'OperatorColour' property</summary>
         /// <param name="page">Owner page.</param>
         OperatorColourProperty(PreferencesPage& page) 
            : ColourProperty(page, L"Operators", PrefsLib.OperatorColour, L"Change the colour used to display Operators")
         {}
      };
      
      /// <summary>ScriptObject syntax highlighting colour property</summary>
      class ScriptObjectColourProperty : public ColourProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create 'ScriptObjectColour' property</summary>
         /// <param name="page">Owner page.</param>
         ScriptObjectColourProperty(PreferencesPage& page) 
            : ColourProperty(page, L"Script Objects", PrefsLib.ScriptObjectColour, L"Change the colour used to display Script Objects")
         {}
      };

      /// <summary>String syntax highlighting colour property</summary>
      class StringColourProperty : public ColourProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create 'StringColour' property</summary>
         /// <param name="page">Owner page.</param>
         StringColourProperty(PreferencesPage& page) 
            : ColourProperty(page, L"Strings", PrefsLib.StringColour, L"Change the colour used to display Strings")
         {}
      };

      /// <summary>Text syntax highlighting colour property</summary>
      class TextColourProperty : public ColourProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create 'TextColour' property</summary>
         /// <param name="page">Owner page.</param>
         TextColourProperty(PreferencesPage& page) 
            : ColourProperty(page, L"Texts", PrefsLib.TextColour, L"Change the colour used to display command text")
         {}
      };

      /// <summary>Variable syntax highlighting colour property</summary>
      class VariableColourProperty : public ColourProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create 'VariableColour' property</summary>
         /// <param name="page">Owner page.</param>
         VariableColourProperty(PreferencesPage& page) 
            : ColourProperty(page, L"Variables", PrefsLib.VariableColour, L"Change the colour used to display variables")
         {}
      };

      // --------------------- CONSTRUCTION ----------------------
   public:
      EditorPage();    
      virtual ~EditorPage();
       
      // ------------------------ STATIC -------------------------
      DECLARE_DYNAMIC(EditorPage)
      DECLARE_MESSAGE_MAP()
	  
      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
   public:

      // ----------------------- MUTATORS ------------------------
   protected:
      void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
      void Populate() override;
	  
      // -------------------- REPRESENTATION ---------------------
   protected:
      ShowLineNumbersProperty*   ShowLineNumbers;
      ScriptViewFontProperty*    ScriptViewFont;
      LineNumberColourProperty*     LineNumberColour;

      BackgroundColourProperty*     BackgroundColour;
      ArgumentColourProperty*       ArgumentColour;
      CommentColourProperty*        CommentColour;
      GameObjectColourProperty*     GameObjectColour;
      KeywordColourProperty*        KeywordColour;
      LabelColourProperty*          LabelColour;
      NumberColourProperty*         NumberColour;
      OperatorColourProperty*       OperatorColour;
      NullColourProperty*           NullColour;
      ScriptObjectColourProperty*   ScriptObjectColour;
      StringColourProperty*         StringColour;
      TextColourProperty*           TextColour;
      VariableColourProperty*       VariableColour;
   };
   
NAMESPACE_END2(GUI,Preferences)
