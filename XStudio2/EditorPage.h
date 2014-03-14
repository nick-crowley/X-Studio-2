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
      class ShowLineNumbersProperty : public PropertyBase
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create 'show line numbers' property</summary>
         /// <param name="page">Owner page.</param>
         ShowLineNumbersProperty(PreferencesPage& page) 
            : PropertyBase(page, L"Line Numbers", L"", L"Display line numbers in the editor")
         {
            AddOption(L"Show Line Numbers", FALSE);
            AddOption(L"Don't Show Line Numbers", FALSE);
            AllowEdit(FALSE);
            // Set initial value
            SetValue(GetOption(PrefsLib.ShowLineNumbers ? 0 : 1));
         }

         // ---------------------- ACCESSORS ------------------------
      public:
         /// <summary>Gets value as bool.</summary>
         /// <returns></returns>
         bool GetBool() const
         {
            return GetString() == GetOption(0);
         }
      };

      /// <summary>Show Script Tooltips property</summary>
      class ShowScriptTooltipsProperty : public PropertyBase
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create 'Show Script Tooltips' property</summary>
         /// <param name="page">Owner page.</param>
         ShowScriptTooltipsProperty(PreferencesPage& page) 
            : PropertyBase(page, L"Code Tooltips", L"", L"Display tooltips when hovering over script-code")
         {
            AddOption(L"Show Code Tooltips", FALSE);
            AddOption(L"Don't Show Code Tooltips", FALSE);
            AllowEdit(FALSE);
            // Set initial value
            SetValue(GetOption(PrefsLib.ShowScriptTooltips ? 0 : 1));
         }

         // ---------------------- ACCESSORS ------------------------
      public:
         /// <summary>Gets value as bool.</summary>
         /// <returns></returns>
         bool GetBool() const
         {
            return GetString() == GetOption(0);
         }
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

      /// <summary>ScriptTooltipDelay property</summary>
      class ScriptTooltipDelayProperty : public PropertyBase
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create 'ScriptTooltipDelay' property</summary>
         /// <param name="page">Owner page.</param>
         ScriptTooltipDelayProperty(PreferencesPage& page) 
            : PropertyBase(page, L"Code Tooltips Delay", _variant_t(PrefsLib.ScriptTooltipDelay), L"Length of time (in seconds) before code tooltips are shown")
         {
            // 1 <= delay <= 30
            EnableSpinControl(TRUE, 1, 30);
         }
      };
      
      /// <summary>ScriptIndentation property</summary>
      class ScriptIndentationProperty : public PropertyBase
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create 'ScriptIndentation' property</summary>
         /// <param name="page">Owner page.</param>
         ScriptIndentationProperty(PreferencesPage& page) 
            : PropertyBase(page, L"Indentation Size", _variant_t(PrefsLib.ScriptIndentation), L"Amount of space characters to use to indent code blocks")
         {
            // 1 <= indent <= 6
            EnableSpinControl(TRUE, 1, 6);
         }
      };

      /// <summary>LineNumber colour property</summary>
      class LineNumberColourProperty : public ColourProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create 'LineNumberColour' property</summary>
         /// <param name="page">Owner page.</param>
         LineNumberColourProperty(PreferencesPage& page) 
            : ColourProperty(page, L"Line Number Colour", PrefsLib.LineNumberColour, L"Change the line number colour used in the script editor")
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
            : ColourProperty(page, L"Background Colour", PrefsLib.BackgroundColour, L"Change the background colour used in the script editor")
         {}
      };

      /// <summary>Diff highlighting colour property</summary>
      class DifferenceHighlightProperty : public ColourProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create 'DifferenceHighlight' property</summary>
         /// <param name="page">Owner page.</param>
         DifferenceHighlightProperty(PreferencesPage& page) 
            : ColourProperty(page, L"Diff Highlight", PrefsLib.DifferenceHighlight, L"Change the colour used to highlight differences in the diff viewer")
         {}
      };
      
      /// <summary>Refactor highlighting colour property</summary>
      class RefactorHighlightProperty : public ColourProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create 'RefactorHighlight' property</summary>
         /// <param name="page">Owner page.</param>
         RefactorHighlightProperty(PreferencesPage& page) 
            : ColourProperty(page, L"Refactor Highlight", PrefsLib.RefactorHighlight, L"Change the colour used to highlight symbols when refactoring")
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
      void Commit() override;
      void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
      void Populate() override;
	  
      // -------------------- REPRESENTATION ---------------------
   protected:
      ShowLineNumbersProperty*      ShowLineNumbers;
      ShowScriptTooltipsProperty*   ShowScriptTooltips;
      ScriptViewFontProperty*       ScriptViewFont;
      ScriptTooltipDelayProperty*   ScriptTooltipDelay;
      ScriptIndentationProperty*    ScriptIndentation;

      BackgroundColourProperty*     BackgroundColour;
      LineNumberColourProperty*     LineNumberColour;
      DifferenceHighlightProperty*  DifferenceHighlight;
      RefactorHighlightProperty*    RefactorHighlight;

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
