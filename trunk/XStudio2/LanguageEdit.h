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

      // --------------------- PROPERTIES ------------------------
	  
      // ---------------------- ACCESSORS ------------------------			
   public:
      EditMode  GetEditMode() const;

   protected:
      bool  HasString() const;

      // ----------------------- MUTATORS ------------------------
   public:
      void  SetEditMode(EditMode m);
      void  SetString(LanguageString* str);

   protected:
      void  DisplayString();
      void  UpdateHighlighting();

      afx_msg void OnCommandEditMode(UINT nID);
      afx_msg void OnQueryEditMode(CCmdUI* pCmd);
      virtual void OnRequestTooltip(CustomTooltip::TooltipData* data);
      virtual void OnTextChange();
      
      // -------------------- REPRESENTATION ---------------------
   public:

   protected:
      EditMode        Mode;
      LanguageString* String;
};
   

NAMESPACE_END2(GUI,Controls)
