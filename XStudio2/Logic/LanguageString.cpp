#include "stdafx.h"
#include "LanguagePage.h"
#include "StringResolver.h"
#include "StringParser.h"
#include "ParameterTypes.h"

namespace Logic
{
   namespace Language
   {
      // -------------------------------- CONSTRUCTION --------------------------------
      
      LanguageString::LanguageString(UINT  id, UINT page, wstring  txt, GameVersion v) : ID(id), Page(page), Text(txt), Version(v) 
      {
      }

      LanguageString::LanguageString(LanguageString&& r) : ID(r.ID), Page(r.Page), Text(move(r.Text)), Version(r.Version) 
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
         return StringParser(Text).Output;
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

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
      
   }

}