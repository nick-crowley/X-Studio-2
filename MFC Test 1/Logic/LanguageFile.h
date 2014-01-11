#pragma once
#include "Common.h"
#include "LanguagePage.h"

namespace Logic
{
   namespace Language
   {

      /// <summary>Represents an X3 language file, provides a collection of language pages</summary>
      class LanguageFile
      {
      public:
         /// <summary>Collection of language pages, sorted by ID</summary>
         class PageCollection : private map<UINT, LanguagePage>
         {
         public:
            /// <summary>Merges a page into the collection</summary>
            /// <param name="p">The page</param>
            void  Merge(LanguagePage& p)
            {
               auto res = insert(value_type(p.ID, p));

               // Merge strings if page already present
               if (!res.second)
               {
                  LanguagePage& existing = res.first->second;
                  for (auto s : p.Strings)
                     existing.Strings.Add(s.second);
               }
            }
         };

         // --------------------- CONSTRUCTION ----------------------

      public:
         LanguageFile() {};
         ~LanguageFile() {};

         // --------------------- PROPERTIES ------------------------
			
		   // ---------------------- ACCESSORS ------------------------

		   // ----------------------- MUTATORS ------------------------

		   // -------------------- REPRESENTATION ---------------------

         UINT            ID;
         PageCollection  Pages;
         GameLanguage    Language;
      };

   }

}

using namespace Logic::Language;
