#include "stdafx.h"
#include "LanguageEdit.h"

/// <summary>User interface controls</summary>
NAMESPACE_BEGIN2(GUI,Controls)

   // --------------------------------- CONSTANTS ----------------------------------

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNAMIC(LanguageEdit, RichEditEx)

   BEGIN_MESSAGE_MAP(LanguageEdit, LanguageEdit)
      ON_CONTROL_REFLECT(EN_CHANGE, &LanguageEdit::OnTextChange)
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   LanguageEdit::LanguageEdit() 
   {
   }

   LanguageEdit::~LanguageEdit()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   
   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Updates the highlighting.</summary>
   void  LanguageEdit::UpdateHighlighting()
   {
   }

   /// <summary>Supply tooltip data</summary>
   /// <param name="data">The data.</param>
   void LanguageEdit::OnRequestTooltip(CustomTooltip::TooltipData* data)
   {
      // None: show nothing
      *data = CustomTooltip::NoTooltip;
   }
   

   /// <summary>Performs syntax colouring on the current line</summary>
   void LanguageEdit::OnTextChange()
   {
      // Perform highlighting
      UpdateHighlighting();

      // Reset tootlip
      __super::OnTextChange();
   }
   
   // ------------------------------- PRIVATE METHODS ------------------------------
   


   
NAMESPACE_END2(GUI,Controls)



