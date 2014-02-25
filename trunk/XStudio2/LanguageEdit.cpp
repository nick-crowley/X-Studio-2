#include "stdafx.h"
#include "LanguageEdit.h"
#include "Logic/RtfStringWriter.h"
#include "Logic/LanguagePage.h"

/// <summary>User interface controls</summary>
NAMESPACE_BEGIN2(GUI,Controls)

   // --------------------------------- CONSTANTS ----------------------------------

   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNAMIC(LanguageEdit, RichEditEx)

   BEGIN_MESSAGE_MAP(LanguageEdit, RichEditEx)
      ON_CONTROL_REFLECT(EN_CHANGE, &LanguageEdit::OnTextChange)
      ON_COMMAND_RANGE(ID_VIEW_SOURCE, ID_VIEW_DISPLAY, &LanguageEdit::OnCommandEditMode)
      ON_UPDATE_COMMAND_UI_REFLECT(&LanguageEdit::OnQueryEditMode)
   END_MESSAGE_MAP()
   
   // -------------------------------- CONSTRUCTION --------------------------------

   LanguageEdit::LanguageEdit() : Mode(EditMode::Edit), String(nullptr)
   {
   }

   LanguageEdit::~LanguageEdit()
   {
   }

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   /// <summary>Gets the edit mode.</summary>
   /// <returns></returns>
   LanguageEdit::EditMode  LanguageEdit::GetEditMode() const
   {
      return Mode;
   }

   /// <summary>Sets the edit mode.</summary>
   /// <param name="m">mode.</param>
   void  LanguageEdit::SetEditMode(EditMode m)
   {
      // Change mode + redisplay   
      Mode = m;
      DisplayString();
   }

   /// <summary>Sets the string.</summary>
   /// <param name="str">The string.</param>
   void  LanguageEdit::SetString(LanguageString* str)
   {
      // Change string + redisplay
      String = str;
      DisplayString();
   }
   
   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Displays the current string in the current mode.</summary>
   void  LanguageEdit::DisplayString()
   {
      try
      {
         // Empty:
         if (!HasString())
            return;

         // Edit:
         else if (Mode == EditMode::Edit)
         {
            string rtf; 
            // Convert string into RTF
            RtfStringWriter w(rtf);
            w.Write(String->RichText);
            w.Close();

            // Replace contents with RTF
            SetRtf(rtf);
         }
         // Display:
         else if (Mode == EditMode::Display)
            throw NotImplementedException(HERE, L"Display mode");

         // Source:
         else if (Mode == EditMode::Source)
         {
            Clear();
            SetWindowText(String->Text.c_str());
         }
      }
      // Error: Clear string and contents
      catch (ExceptionBase& ) {
         Clear();
         String = nullptr;
         throw;
      }
   }

   /// <summary>Determines whether this instance has string.</summary>
   /// <returns></returns>
   bool  LanguageEdit::HasString() const
   {
      return String != nullptr;
   }
   
   /// <summary>Changes the edit mode.</summary>
   /// <param name="nID">The command identifier.</param>
   void LanguageEdit::OnCommandEditMode(UINT nID)
   {
      switch (nID)
      {
      case ID_VIEW_SOURCE:    SetEditMode(EditMode::Source);   break;
      case ID_VIEW_EDITOR:    SetEditMode(EditMode::Edit);     break;
      case ID_VIEW_DISPLAY:   SetEditMode(EditMode::Display);  break;
      }
   }

   /// <summary>Supply tooltip data</summary>
   /// <param name="data">The data.</param>
   void LanguageEdit::OnRequestTooltip(CustomTooltip::TooltipData* data)
   {
      // None: show nothing
      *data = CustomTooltip::NoTooltip;
   }
   
   /// <summary>Called when query edit mode.</summary>
   /// <param name="pCmd">The command.</param>
   void LanguageEdit::OnQueryEditMode(CCmdUI* pCmd)
   {
      switch (pCmd->m_nID)
      {
      case ID_VIEW_SOURCE:    pCmd->Enable(Mode != EditMode::Source);   break;
      case ID_VIEW_EDITOR:    pCmd->Enable(Mode != EditMode::Edit);     break;
      case ID_VIEW_DISPLAY:   pCmd->Enable(Mode != EditMode::Display);  break;
      }
   }

   /// <summary>Performs syntax colouring on the current line</summary>
   void LanguageEdit::OnTextChange()
   {
      // Perform highlighting
      UpdateHighlighting();

      // Reset tootlip
      __super::OnTextChange();
   }
   
   /// <summary>Updates the highlighting.</summary>
   void  LanguageEdit::UpdateHighlighting()
   {
      SuspendUndo(true);
      FreezeWindow(true);

      try
      {
         CharFormat cf(CFM_COLOR | CFM_UNDERLINE | CFM_UNDERLINETYPE, NULL);

         // Clear formatting
         SetSel(0, -1);
         SetSelectionCharFormat(cf);

         // Highlight code...
      }
      catch (regex_error& e) {
         Console.Log(HERE, RegularExpressionException(HERE, e));
      }

      FreezeWindow(false);
      SuspendUndo(false);
   }

   // ------------------------------- PRIVATE METHODS ------------------------------
   


   
NAMESPACE_END2(GUI,Controls)



