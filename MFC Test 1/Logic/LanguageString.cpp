#include "stdafx.h"
#include "LanguagePage.h"

namespace Logic
{
   namespace Language
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

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
         case KnownPage::DATA_TYPES:
         case KnownPage::FLIGHT_RETURNS:
         case KnownPage::OBJECT_CLASSES:
         case KnownPage::OBJECT_COMMANDS:
         case KnownPage::PARAMETER_TYPES:
         case KnownPage::WING_COMMANDS:
            break;

         // Exclude 'old' [THIS] from lookup tree
         case KnownPage::CONSTANTS:
            if (ID == 0)
               return false;
            break;

         // Skip 6 digit sector names with IDs 20xxx and 30xxx
         case KnownPage::SECTORS:
            if (ID < 1020000)
               return false;
            break;

         // Include names, exclude initials
         case KnownPage::RACES:
            if (ID >= 200)
               return false;
            break;

         // Exclude the S,M,L,XL,XXL ship/station name modifiers
         case KnownPage::STATION_SERIALS:
            if (ID >= 500)
               return false;
            break;

         // Only include the abbreviated versions
         case KnownPage::TRANSPORT_CLASSES:
            if (ID >= 10)
               return false;
            break;

         // FRIEND/FOE/NEUTRAL
         case KnownPage::RELATIONS:
            switch (ID)
            {
            case 1102422:
            case 1102423:
            case 1102424: 
               break;
            default:  
               return false;
            }
         }

         return true;
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
      
   }

}