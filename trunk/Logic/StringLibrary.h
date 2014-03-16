#pragma once

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
      public:
         /// <summary>Collection of language files, sorted with highest priority first</summary>
         class FileCollection : public set<LanguageFile, greater<LanguageFile>>
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
         FileCollection::const_iterator begin() const { return Files.begin(); }
         FileCollection::const_iterator end() const   { return Files.end(); }

         bool              Contains(UINT page, UINT id) const;
         LanguageStringRef Find(UINT page, UINT id) const;
         wstring           Resolve(UINT page, UINT id) const;
         bool              TryFind(UINT page, UINT id, const LanguageString* &str) const;

		   // ----------------------- MUTATORS ------------------------

      public:
         void  Clear();
         UINT  Enumerate(XFileSystem& vfs, GameLanguage lang, WorkerData* data);

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
