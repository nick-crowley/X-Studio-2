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
         class PageCollection : public map<UINT, LanguagePage, less<UINT>>
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

            /// <summary>Finds a page by index</summary>
            /// <param name="index">The index</param>
            /// <returns></returns>
            /// <exception cref="Logic::IndexOutOfRangeException">Index does not exist</exception>
            LanguagePage&  FindByIndex(UINT index) 
            {
               UINT i = 0;

               // Linear search
               for (iterator it = begin(); it != end(); ++it)
                  if (index == i++)
                     return it->second;

               // Not found
               throw IndexOutOfRangeException(HERE, index, size());
            }

            /// <summary>Merges a page into the collection</summary>
            /// <param name="p">The page</param>
            void  Merge(LanguagePage& p)
            {
               auto res = insert(value_type(p.ID, p));

               // Exists: Merge strings into existing page
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
