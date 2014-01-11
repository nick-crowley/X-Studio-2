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
         class PageCollection : public map<UINT, LanguagePage>
         {
         public:
            /// <summary>Query whether a page is present</summary>
            /// <param name="page">The page id</param>
            /// <returns></returns>
            bool  Contains(UINT page) const
            {
               return find(page) != end();
            }

            /// <summary>Queries whether a string is present</summary>
            /// <param name="page">The page id</param>
            /// <param name="id">The string id.</param>
            /// <returns></returns>
            bool  Contains(UINT page, UINT id) const
            {
               const_iterator it = find(page);
               return it != end() && it->second.Strings.Contains(id);
            }

            /// <summary>Finds the specified page.</summary>
            /// <param name="page">The page id</param>
            /// <returns></returns>
            /// <exception cref="Logic::PageNotFoundException">Page does not exist</exception>
            const LanguagePage&  Find(UINT page) const
            {
               const_iterator it;

               if ((it=find(page)) == end())
                  throw PageNotFoundException(HERE, page);

               return it->second;
            }

            /// <summary>Finds the specified string</summary>
            /// <param name="page">The page id</param>
            /// <param name="id">The string id</param>
            /// <returns></returns>
            /// <exception cref="Logic::PageNotFoundException">Page does not exist</exception>
            /// <exception cref="Logic::StringNotFoundException">String does not exist</exception>
            const LanguageString&  Find(UINT page, UINT id) const
            {
               return Find(page).Strings[id]; 
            }

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

            /// <summary>Finds the specified page.</summary>
            /// <param name="page">The page id</param>
            /// <returns></returns>
            /// <exception cref="Logic::PageNotFoundException">Page does not exist</exception>
            const LanguagePage&  operator[](UINT  page) const
            {
               return Find(page);
            }
         };

         // --------------------- CONSTRUCTION ----------------------

      public:
         LanguageFile();
         LanguageFile(UINT id, GameLanguage l) : ID(id), Language(l) {}
         ~LanguageFile();

         // --------------------- PROPERTIES ------------------------
			
		   // ---------------------- ACCESSORS ------------------------

         /// <summary>Queries whether a string is present</summary>
         /// <param name="page">The page id</param>
         /// <param name="id">The string id.</param>
         /// <returns></returns>
         bool  Contains(UINT page, UINT id) const    { return Pages.Contains(page,id); }

         /// <summary>Finds the specified string</summary>
         /// <param name="page">The page id</param>
         /// <param name="id">The string id</param>
         /// <returns></returns>
         /// <exception cref="Logic::PageNotFoundException">Page does not exist</exception>
         /// <exception cref="Logic::StringNotFoundException">String does not exist</exception>
         LanguageString  Find(UINT page, UINT id) const        { return Pages.Find(page,id);     }

		   // ----------------------- MUTATORS ------------------------

		   // -------------------- REPRESENTATION ---------------------

         UINT            ID;
         PageCollection  Pages;
         GameLanguage    Language;
      };

   }

}

using namespace Logic::Language;
