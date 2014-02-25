#pragma once
#include "RichEditEx.h"
#include <regex>

/// <summary>User interface controls</summary>
NAMESPACE_BEGIN2(GUI,Controls)


   /// <summary>RichEdit extension class</summary>
   class LanguageEdit : public RichEditEx
   {
      // ------------------------ TYPES --------------------------
   public:
      enum class EditMode { Source, Edit, Display };

   protected:

      // --------------------- CONSTRUCTION ----------------------
   public:
      LanguageEdit();
      virtual ~LanguageEdit();
       
      // ------------------------ STATIC -------------------------
      DECLARE_DYNAMIC(LanguageEdit)
      DECLARE_MESSAGE_MAP()

   protected:
      const static wregex   MatchComplexTag,
                            MatchColourCode,
                            MatchSimpleTag,
                            MatchSubString,
                            MatchVariable;

      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
   public:
      EditMode  GetEditMode() const;

   protected:
      bool  HasString() const;

      // ----------------------- MUTATORS ------------------------
   public:
      void  Clear();
      void  SetEditMode(EditMode m);
      void  SetString(LanguageString* str);

   protected:
      void  DisplayString();
      void  HighlightMatch(UINT pos, UINT length, CharFormat& cf);
      void  UpdateHighlighting();

      virtual void OnRequestTooltip(CustomTooltip::TooltipData* data);
      virtual void OnTextChange();
      
      // -------------------- REPRESENTATION ---------------------
   public:

   protected:
      EditMode        Mode;
      LanguageString* String;
};
   

NAMESPACE_END2(GUI,Controls)
