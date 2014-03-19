#include "stdafx.h"
#include "LanguagePage.h"
#include "StringResolver.h"
#include "RichStringParser.h"
#include "ParameterTypes.h"

namespace Logic
{
   namespace Language
   {
      /// <summary>Matches any tags that identify whether named colour tags should be used</summary>
      const wregex  LanguageString::MatchMessageTag(L"\\["
                                                    L"(article|author|ranking|text|title"
                                                    L"|black|blue|cyan|green|grey|orange|magenta|red|silver|yellow|white)"
                                                    L"\\]");

      // -------------------------------- CONSTRUCTION --------------------------------
      
      LanguageString::LanguageString(UINT  id, UINT page, wstring  txt, GameVersion v)
         : ID(id), Page(page), Text(txt), Version(v), TagType(ColourTag::Undetermined)
      {
      }

      LanguageString::LanguageString(LanguageString&& r) 
         : ID(r.ID), Page(r.Page), Text(move(r.Text)), Version(r.Version), TagType(r.TagType)
      {
      }

      LanguageString::~LanguageString() 
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Get fully resolved text</summary>
      /// <returns></returns>
      wstring  LanguageString::GetResolvedText() const
      {
         return StringResolver(*this).Text;
      }

      /// <summary>Generate rich-text string from source text</summary>
      /// <returns></returns>
      /// <exception cref="Logic::AlgorithmException">Error in parsing algorithm</exception>
      /// <exception cref="Logic::ArgumentException">Error in parsing algorithm</exception>
      /// <exception cref="Logic::Language::RichTextException">Error in formatting tags</exception>
      RichString  LanguageString::GetRichText() const
      {
         return RichStringParser(Text).Output;
      }
      
      /// <summary>Identifies and sets the type of colour tags used by this string.</summary>
      /// <returns>Tag type, defaults to Unix if undetermined</returns>
      ColourTag  LanguageString::IdentifyColourTags()
      {
         // Find [author], [title], [rank], [article] or any named colour tag
         if (regex_search(Text.begin(), Text.end(), MatchMessageTag))
            return TagType = ColourTag::Message;

         // Assume Unix
         return TagType = ColourTag::Unix;
      }

      /// <summary>Determines whether string can be used as the source of a script object</summary>
      /// <returns></returns>
      bool  LanguageString::IsScriptObject() const
      {
         switch (Page)
         {
         // Exclude all
         default:
            return false;

         // Include all
         case KnownPage::OPERATORS:
         case KnownPage::DATA_TYPES:
         case KnownPage::FLIGHT_RETURNS:
         case KnownPage::OBJECT_CLASSES:
         case KnownPage::OBJECT_COMMANDS:
         case KnownPage::PARAMETER_TYPES:
         case KnownPage::WING_COMMANDS:
            return true;

         // Exclude 'old' [THIS] from lookup tree
         case KnownPage::CONSTANTS:
            return ID != 0;

         // Skip 6 digit sector names with IDs 20xxx and 30xxx
         case KnownPage::SECTORS:
            return ID >= 1020000;

         // Include names, exclude initials
         case KnownPage::RACES:
            return ID < 200;

         // Exclude the S,M,L,XL,XXL ship/station name modifiers
         case KnownPage::STATION_SERIALS:
            return ID < 500;

         // Only include the abbreviated versions
         case KnownPage::TRANSPORT_CLASSES:
            return ID < 10;

         // FRIEND/FOE/NEUTRAL
         case KnownPage::RELATIONS:
            switch (ID)
            {
            case 1102422:
            case 1102423:
            case 1102424: 
               return true;
            default:  
               return false;
            }
         }
      }

      /// <summary>Generate XML necessary to define this string  [Used for copying to clipboard]</summary>
      /// <returns></returns>
      GuiString  LanguageString::ToXML() const
      {
         return VString(L"<t id='%d'>%s</t>", ID, Text.c_str());
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
      
   }

}