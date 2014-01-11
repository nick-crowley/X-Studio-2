#pragma once
#include "Common.h"
#include "LanguageFile.h"
#include "XFileSystem.h"
#include <algorithm>

namespace Logic
{
   namespace Language
   {
      /// <summary>Defines the game pages containing the definitions for the various game constants</summary>
      enum KnownPage : UINT
      {  
         SECTORS              = 7,
         STATION_SERIALS      = 12,
         SHIPS_STATIONS_WARES = 17,
         RELATIONS            = 35,
         RACES                = 1266,
         TRANSPORT_CLASSES    = 1999,
         PARAMETER_TYPES      = 2000,
         OPERATORS            = 2001,
         CONSTANTS            = 2002,
         COMMAND_SYNTAX       = 2003,
         OBJECT_CLASSES       = 2006,
         CONDITIONALS         = 2007,
         OBJECT_COMMANDS      = 2008,
         FLIGHT_RETURNS       = 2009,
         DATA_TYPES           = 2013,
         WING_COMMANDS        = 2028 
      };


      /// <summary></summary>
      class StringLibrary
      {
         /// <summary>Sorts language files by descending FileID</summary>
         class SortByFileID : binary_function<LanguageFile, LanguageFile, bool>
         {
         public:
            bool operator()(const LanguageFile& a, const LanguageFile& b) const
            { return a.ID > b.ID; }
         };

         /// <summary>Collection of language files</summary>
         class FileCollection : public set<LanguageFile, SortByFileID>
         {
         };

         // --------------------- CONSTRUCTION ----------------------

      private:
         StringLibrary();
      public:
         virtual ~StringLibrary();

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
			
		   // ---------------------- ACCESSORS ------------------------			

      public:
         bool            Contains(UINT page, UINT id) const;
         LanguageString  Find(UINT page, UINT id) const;

		   // ----------------------- MUTATORS ------------------------

      public:
         void  Clear();
         UINT  Enumerate(XFileSystem& vfs, GameLanguage lang);

      private:
         LanguageFile  GetSpecialCases(GameLanguage lang);

		   // -------------------- REPRESENTATION ---------------------

      public:
         static StringLibrary  Instance;

         FileCollection  Files;
      };

      // The string library singleton
      #define StringLib  StringLibrary::Instance
   }
}

using namespace Logic::Language;
