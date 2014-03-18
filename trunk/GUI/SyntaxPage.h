#pragma once
#include "PreferencesPage.h"

/// <summary>User interface</summary>
NAMESPACE_BEGIN2(GUI,Preferences)
   
   /// <summary></summary>
   class SyntaxPage : public PreferencesPage
   {
      // ------------------------ TYPES --------------------------
   public:
      enum { IDD = IDD_SYNTAX_PAGE };

   protected:
      
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
      
      /// <summary>Constant syntax highlighting colour property</summary>
      class ConstantColourProperty : public ColourProperty
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create 'ConstantColour' property</summary>
         /// <param name="page">Owner page.</param>
         ConstantColourProperty(PreferencesPage& page) 
            : ColourProperty(page, L"Constants", PrefsLib.ConstantColour, L"Change the colour used to display Constants")
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
      SyntaxPage();    
      virtual ~SyntaxPage();
       
      // ------------------------ STATIC -------------------------
      DECLARE_DYNAMIC(SyntaxPage)
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
      ArgumentColourProperty*       ArgumentColour;
      CommentColourProperty*        CommentColour;
      ConstantColourProperty*       ConstantColour;
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
