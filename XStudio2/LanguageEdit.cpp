#include "stdafx.h"
#include "LanguageEdit.h"
#include "Logic/RtfStringWriter.h"
#include "Logic/LanguagePage.h"

/// <summary>User interface controls</summary>
NAMESPACE_BEGIN2(GUI,Controls)

   // --------------------------------- CONSTANTS ----------------------------------
   
   /// <summary>Matches an opening tag with up to four properties</summary>
   const wregex LanguageEdit::MatchComplexTag
   (
      L"\\[" 
      L"(article|declare|rank|select|text|var)" 
      L"(\\s+(args|cols|colwidth|colspacing|name|neg|script|state|title|type|value)=[\"\'][^\"\']*[\"\'])?"  // Apostrophes or inverted commas
      L"(\\s+(args|cols|colwidth|colspacing|name|neg|script|state|title|type|value)=[\"\'][^\"\']*[\"\'])?" 
      L"(\\s+(args|cols|colwidth|colspacing|name|neg|script|state|title|type|value)=[\"\'][^\"\']*[\"\'])?" 
      L"(\\s+(args|cols|colwidth|colspacing|name|neg|script|state|title|type|value)=[\"\'][^\"\']*[\"\'])?" 
      L"\\s?/?"  // Possibly self closing
      L"\\]"
   );   

   /// <summary>Matches an opening tag without properties or a closing tag</summary>
   const wregex LanguageEdit::MatchSimpleTag
   (
      L"\\[/?" // Possibly closing
      L"(b|i|u|left|right|center|justify" 
      L"|article|author|title|text|select" 
      L"|black|blue|cyan|green|orange|magenta|red|silver|white|yellow)"
      L"\\]"
   );

   /// <summary>Matches an substring reference</summary>
   const wregex LanguageEdit::MatchSubString
   (
      L"\\{"
      L"\\d+" 
      L"(,\\d+)?" 
      L"\\}"
   );

   /// <summary>Matches a mission director variable</summary>
   const wregex LanguageEdit::MatchVariable
   (
      L"\\{"
      L"[a-z\\.]+" 
      L"@" 
      L"[_ \\w\\d\\.]+" 
      L"\\}"
   );
   
   // --------------------------------- APP WIZARD ---------------------------------
  
   IMPLEMENT_DYNAMIC(LanguageEdit, RichEditEx)

   BEGIN_MESSAGE_MAP(LanguageEdit, RichEditEx)
      ON_CONTROL_REFLECT(EN_CHANGE, &LanguageEdit::OnTextChange)
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
      auto prev = GetEditMode();

      try
      {
         // Change mode + redisplay   
         Mode = m;
         DisplayString();
      }
      // Error: Revert mode
      catch (ExceptionBase& e) {
         Mode = prev;
         theApp.ShowError(HERE, e, L"Unable to change edit mode");
         DisplayString();
      }
   }

   /// <summary>Sets the string.</summary>
   /// <param name="str">The string.</param>
   void  LanguageEdit::SetString(LanguageString* str)
   {
      auto prev = String;

      try
      {
         // Change string + redisplay
         String = str;
         DisplayString();
      }
      // Error: Revert string
      catch (ExceptionBase& e) {
         String = prev;
         theApp.ShowError(HERE, e, L"Unable to display string");
         DisplayString();
      }
   }
   
   // ------------------------------ PROTECTED METHODS -----------------------------
   
   /// <summary>Clears contents and resets font to Arial 10pt</summary>
   void  LanguageEdit::Clear()
   {
      CharFormat cf(CFM_FACE|CFM_COLOR|CFM_PROTECTED|CFM_SIZE, CFE_PROTECTED);
      cf.crTextColor = RGB(255,255,255);
      cf.yHeight = 10*20;
      StringCchCopy(cf.szFaceName, LF_FACESIZE, L"Arial");

      // Clear
      __super::Clear();
      SetDefaultCharFormat(cf);
   }

   /// <summary>Displays the current string in the current mode.</summary>
   void  LanguageEdit::DisplayString()
   {
      try
      {
         // Empty:
         if (!HasString())
            Clear();

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
            UpdateHighlighting();
         }
      }
      // Error: Clear contents
      catch (ExceptionBase& ) {
         Clear();
         throw;
      }
   }

   /// <summary>Determines whether this instance has string.</summary>
   /// <returns></returns>
   bool  LanguageEdit::HasString() const
   {
      return String != nullptr;
   }
   
   /// <summary>Highlights the match.</summary>
   /// <param name="pos">position.</param>
   /// <param name="length">length or -1 for entire text.</param>
   /// <param name="cf">formatting.</param>
   void  LanguageEdit::HighlightMatch(UINT pos, UINT length, CharFormat& cf)
   {
      SetSel(pos, (length != -1 ? pos + length : -1));
      SetSelectionCharFormat(cf);
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
      if (GetEditMode() == EditMode::Source)
         UpdateHighlighting();

      // Reset tootlip
      __super::OnTextChange();
   }
   
   /// <summary>Updates the highlighting.</summary>
   void  LanguageEdit::UpdateHighlighting()
   {
      static const COLORREF Tag = RGB(255,174,207),         // Pale Pink
                            Property = RGB(255,174,89),     // Pale Orange
                            SubString = RGB(152,220,235),   // Pale Blue
                            Variable = RGB(152,235,156);    // Pale Green

      // Freeze
      SuspendUndo(true);
      FreezeWindow(true);

      try
      {
         CharFormat cf(CFM_COLOR | CFM_UNDERLINE | CFM_UNDERLINETYPE, NULL);
         auto text = GetAllText();

         // Clear formatting
         cf.crTextColor = RGB(255,255,255);
         HighlightMatch(0, -1, cf);

         // Highlight simple opening/closing tags
         cf.crTextColor = Tag;
         for (wsregex_iterator it(text.cbegin(), text.cend(), MatchSimpleTag), end; it != end; ++it)
            HighlightMatch(it->position(), it->length(), cf);

         // Highlight tags with properties
         for (wsregex_iterator it(text.cbegin(), text.cend(), MatchComplexTag), end; it != end; ++it)
         {
            // Highlight entire tag
            cf.crTextColor = Tag;
            HighlightMatch(it->position(0), it->length(0), cf);

            // Highlight up to four {properties,value} pairs
            cf.crTextColor = Property;
            for (UINT i = 2; (i <= 8) && (i < it->max_size()) && (it->_At(i).matched); i+=2)
               HighlightMatch(it->position(i), it->length(i), cf);
         }

         // Highlight substrings
         cf.crTextColor = SubString;
         for (wsregex_iterator it(text.cbegin(), text.cend(), MatchSubString), end; it != end; ++it)
            HighlightMatch(it->position(), it->length(), cf);

         // Highlight variables
         cf.crTextColor = Variable;
         for (wsregex_iterator it(text.cbegin(), text.cend(), MatchVariable), end; it != end; ++it)
            HighlightMatch(it->position(), it->length(), cf);
      }
      catch (regex_error& e) {
         Console.Log(HERE, RegularExpressionException(HERE, e));
      }

      // UnFreeze
      FreezeWindow(false);
      SuspendUndo(false);
   }


   // ------------------------------- PRIVATE METHODS ------------------------------
   


   
NAMESPACE_END2(GUI,Controls)



