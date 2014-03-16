#pragma once


#include "LookupString.h"

namespace Logic
{
   namespace Types
   {
      /// <summary>Ship/station race lookup string</summary>
      class RaceLookup : public LookupString
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         RaceLookup() : LookupString(0, KnownPage::RACES)
         {}
         RaceLookup(UINT id) : LookupString(id, KnownPage::RACES)
         {}

         DEFAULT_COPY(RaceLookup);	// Default copy semantics
         DEFAULT_MOVE(RaceLookup);	// Default move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         virtual bool  Defined() const
         {
            return ID != 0 && ID != 34000;
         }

         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------
      };

      /// <summary>Station size lookup string</summary>
      class StationSizeLookup : public LookupString
      {
         enum StationSize { Small=2, Medium=5, Large=10, ExtraLarge=20 };

         // --------------------- CONSTRUCTION ----------------------
      public:
         StationSizeLookup() : LookupString(0, KnownPage::STATION_SERIALS)
         {}
         StationSizeLookup(UINT id) : LookupString(id, KnownPage::STATION_SERIALS)
         {}

         DEFAULT_COPY(StationSizeLookup);	// Default copy semantics
         DEFAULT_MOVE(StationSizeLookup);	// Default move semantics

         // ------------------------ STATIC -------------------------
         
         static UINT  StationSizeToStringID(UINT id)
         {
            switch (id)
            {
            case StationSize::Small:      return 502;
            case StationSize::Medium:     return 503;
            case StationSize::Large:      return 504;
            case StationSize::ExtraLarge: return 505;
            default:                      return 0;
            }
         }

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         /// <summary>Queries whether station size ID is defined</summary>
         /// <returns></returns>
         bool  Defined() const
         {
            return ID != 0;
         }

         /// <summary>Queries whether the station size string is present in the string library</summary>
         /// <returns></returns>
         bool  Exists() const
         {
            return StringLib.Contains(Page, StationSizeToStringID(ID));
         }

         /// <summary>Finds the station size text in the string library</summary>
         /// <returns></returns>
         /// <exception cref="Logic::PageNotFoundException">Page does not exist</exception>
         /// <exception cref="Logic::StringNotFoundException">String does not exist</exception>
         wstring  GetText() const
         {
            return StringLib.Resolve(Page, StationSizeToStringID(ID));
         }

         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------
      };
   
      /// <summary>Ship variation name lookup string</summary>
      class VariationLookup : public LookupString
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         VariationLookup() : LookupString(0, KnownPage::NAMES_DESCRIPTIONS)
         {}
         VariationLookup(UINT id) : LookupString(id, KnownPage::NAMES_DESCRIPTIONS)
         {}

         DEFAULT_COPY(VariationLookup);	// Default copy semantics
         DEFAULT_MOVE(VariationLookup);	// Default move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         /// <summary>Queries whether ship variation ID is defined</summary>
         /// <returns></returns>
         bool  Defined() const
         {
            return ID != 0 && ID != 10000;
         }

         /// <summary>Queries whether the ship variation string is present in the string library</summary>
         /// <returns></returns>
         bool  Exists() const
         {
            return StringLib.Contains(Page, ID+10000);
         }

         /// <summary>Finds the ship variation text in the string library</summary>
         /// <returns></returns>
         /// <exception cref="Logic::PageNotFoundException">Page does not exist</exception>
         /// <exception cref="Logic::StringNotFoundException">String does not exist</exception>
         wstring  GetText() const
         {
            return StringLib.Resolve(Page, ID+10000);
         }

         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------
      };
   }
}

using namespace Logic::Types;
