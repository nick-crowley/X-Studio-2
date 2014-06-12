#pragma once


namespace GUI
{
   namespace Controls
   {
      class ScriptEdit;
      enum class Suggestion : UINT;

      /// <summary>Base-class for Visitors performing identification of suggestions</summary>
      interface ISuggestionVisitor
      {
         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------
      public:
         virtual Suggestion Identify(SuggestionDirector* d, wchar ch) const = 0;
      };
      
      /// <summary>Visitor for identifying when to display 'variables' suggestions</summary>
      class LabelVisitor : public SuggestionVisitor
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         LabelVisitor(ScriptEdit* e) : SuggestionVisitor(e)
         {}
         
         // ----------------------- MUTATORS ------------------------
      public:
         Suggestion Identify(wchar ch) const override;

         // -------------------- REPRESENTATION ---------------------
      protected:
      };

      /// <summary>Visitor for identifying when to display 'GameObject' suggestions</summary>
      class GameObjectVisitor : public SuggestionVisitor
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         GameObjectVisitor(ScriptEdit* e) : SuggestionVisitor(e)
         {}
         
         // ----------------------- MUTATORS ------------------------
      public:
         Suggestion Identify(wchar ch) const override;

         // -------------------- REPRESENTATION ---------------------
      protected:
      };

      /// <summary>Visitor for identifying when to display 'ScriptObject' suggestions</summary>
      class ScriptObjectVisitor : public SuggestionVisitor
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         ScriptObjectVisitor(ScriptEdit* e) : SuggestionVisitor(e)
         {}
         
         // ----------------------- MUTATORS ------------------------
      public:
         Suggestion Identify(wchar ch) const override;

         // -------------------- REPRESENTATION ---------------------
      protected:
      };

      /// <summary>Visitor for identifying when to display 'variables' suggestions</summary>
      class VariableVisitor : public SuggestionVisitor
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         VariableVisitor(ScriptEdit* e) : SuggestionVisitor(e)
         {}
         
         // ----------------------- MUTATORS ------------------------
      public:
         Suggestion Identify(wchar ch) const override;

         // -------------------- REPRESENTATION ---------------------
      protected:
      };

   
   }
}

using namespace GUI::Controls;
