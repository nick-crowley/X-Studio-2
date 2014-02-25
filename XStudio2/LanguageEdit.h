#pragma once
#include "RichEditEx.h"
#include <regex>

/// <summary>User interface controls</summary>
NAMESPACE_BEGIN2(GUI,Controls)


   /// <summary>RichEdit extension class</summary>
   class LanguageEdit : public RichEditEx
   {
      // ------------------------ TYPES --------------------------
   protected:

      // --------------------- CONSTRUCTION ----------------------
   public:
      LanguageEdit();
      virtual ~LanguageEdit();
       
      // ------------------------ STATIC -------------------------
      DECLARE_DYNAMIC(LanguageEdit)
      DECLARE_MESSAGE_MAP()

      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
   public:

   protected:
      
      // ----------------------- MUTATORS ------------------------
   public:

   protected:
      void  UpdateHighlighting();

      virtual void OnRequestTooltip(CustomTooltip::TooltipData* data);
      virtual void OnTextChange();
      
      // -------------------- REPRESENTATION ---------------------
   public:

};
   

NAMESPACE_END2(GUI,Controls)
