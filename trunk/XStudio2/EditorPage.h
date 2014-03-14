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

      /// <summary>CommitOnSave property</summary>
      class CommitOnSaveProperty : public PropertyBase
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create 'CommitOnSave' property</summary>
         /// <param name="page">Owner page.</param>
         CommitOnSaveProperty(PreferencesPage& page) 
            : PropertyBase(page, L"Automatic Commit", L"", L"Automatically perform a quick commit when saving a script")
         {
            AddOption(L"Perform quick commit on save", FALSE);
            AddOption(L"Don't quick commit on save", FALSE);
            AllowEdit(FALSE);
            // Set initial value
            SetValue(GetOption(PrefsLib.CommitOnSave ? 0 : 1));
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

      /// <summary>IncrementOnSave property</summary>
      class IncrementOnSaveProperty : public PropertyBase
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create 'IncrementOnSave' property</summary>
         /// <param name="page">Owner page.</param>
         IncrementOnSaveProperty(PreferencesPage& page) 
            : PropertyBase(page, L"Version Increment", L"", L"Automatically increment the version number when saving a script")
         {
            AddOption(L"Increment version number on save", FALSE);
            AddOption(L"Don't increment version number on save", FALSE);
            AllowEdit(FALSE);
            // Set initial value
            SetValue(GetOption(PrefsLib.IncrementOnSave ? 0 : 1));
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
            : PropertyBase(page, L"Tooltip Delay", _variant_t(PrefsLib.ScriptTooltipDelay), L"Length of time (in seconds) before code tooltips are shown")
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

      CommitOnSaveProperty*         CommitOnSave;
      IncrementOnSaveProperty*      IncrementOnSave;

      BackgroundColourProperty*     BackgroundColour;
      LineNumberColourProperty*     LineNumberColour;
      DifferenceHighlightProperty*  DifferenceHighlight;
      RefactorHighlightProperty*    RefactorHighlight;

      
   };
   
NAMESPACE_END2(GUI,Preferences)
