#pragma once
#include "Common.h"
#include "LanguageFile.h"
#include "XFileSystem.h"
#include "BackgroundWorker.h"
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
         wstring         Resolve(UINT page, UINT id) const;

		   // ----------------------- MUTATORS ------------------------

      public:
         void  Clear();
         UINT  Enumerate(XFileSystem& vfs, GameLanguage lang, WorkerData* data);

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
