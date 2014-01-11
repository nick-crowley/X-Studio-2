#pragma once
#include "Common.h"
#include "LanguageFile.h"
#include "XFileSystem.h"
#include <algorithm>

namespace Logic
{
   namespace Language
   {
         
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

      public:
         StringLibrary();
         virtual ~StringLibrary();

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
			
		   // ---------------------- ACCESSORS ------------------------			

      public:
         bool  Contains(UINT page, UINT id) const;
         LanguageString  Find(UINT page, UINT id) const;

		   // ----------------------- MUTATORS ------------------------

      public:
         UINT  Enumerate(XFileSystem& vfs, GameLanguage lang);

		   // -------------------- REPRESENTATION ---------------------

      private:
         FileCollection  Files;
      };

   }
}

using namespace Logic::Language;
