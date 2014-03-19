#pragma once
#include "ScriptEdit.h"

/// <summary>User interface controls</summary>
NAMESPACE_BEGIN2(GUI,Controls)

   
   /// <summary>Difference highlighting control</summary>
   class RefactorEdit : public ScriptEdit
   {
      // ------------------------ TYPES --------------------------
   public:
      /// <summary>Defines three highlighting types</summary>
      enum class HighlightType { None, Active, Inactive };

      // --------------------- CONSTRUCTION ----------------------
   public:
      RefactorEdit();
      virtual ~RefactorEdit();
       
      // ------------------------ STATIC -------------------------
      DECLARE_DYNAMIC(RefactorEdit)
      DECLARE_MESSAGE_MAP()


      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
   
      // ----------------------- MUTATORS ------------------------
   public:
      void  Initialize(ScriptDocument* doc);
      void  HighlightSymbol(const Symbol& s, HighlightType ht);
      void  HighlightSymbols(const vector<Symbol> symbols);
      void  ScrollTo(const Symbol& s);
      
   protected:
      
	  
      // -------------------- REPRESENTATION ---------------------
   public:

   protected:
      
   };
   

NAMESPACE_END2(GUI,Controls)
