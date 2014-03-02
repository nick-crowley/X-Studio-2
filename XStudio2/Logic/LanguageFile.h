#pragma once
#include "Common.h"
#include "LanguagePage.h"
#include "MapIterator.hpp"
#include "FileBase.h"

namespace Logic
{
   namespace Language
   {

      /// <summary>Represents an X3 language file, provides a collection of language pages</summary>
      class LanguageFile : public FileBase
      {
      public:
         /// <summary>Collection of language pages, sorted by ID</summary>
         class PageCollection : public map<UINT, LanguagePage, less<UINT>>
         {
            // --------------------- CONSTRUCTION ----------------------
         public:
            PageCollection()
            {}
            PageCollection(PageCollection&& r) : map<UINT, LanguagePage, less<UINT>>(move(r))
            {}

            // ---------------------- ACCESSORS ------------------------
         
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

            /// <summary>Read-only access to pages by ID.</summary>
            /// <param name="page">The page id</param>
            /// <returns></returns>
            /// <exception cref="Logic::PageNotFoundException">Page does not exist</exception>
            LanguagePageRef  Find(UINT page) const
            {
               const_iterator it;

               if ((it=find(page)) == end())
                  throw PageNotFoundException(HERE, page);

               return it->second;
            }

            /// <summary>Read-only access to strings by ID</summary>
            /// <param name="page">The page id</param>
            /// <param name="id">The string id</param>
            /// <returns></returns>
            /// <exception cref="Logic::PageNotFoundException">Page does not exist</exception>
            /// <exception cref="Logic::StringNotFoundException">String does not exist</exception>
            LanguageStringRef  Find(UINT page, UINT id) const
            {
               return Find(page).Strings[id]; 
            }

            /// <summary>Find zero-based index of a page</summary>
            /// <param name="page">page id.</param>
            /// <returns></returns>
            /// <exception cref="Logic::PageNotFoundException">Page does not exist</exception>
            UINT  IndexOf(UINT page) const
            {
               const_iterator pos;

               if ((pos=find(page)) == end())
                  throw PageNotFoundException(HERE, page);

               return distance(begin(), pos);
            }
            
            /// <summary>Read-only access to pages by ID.</summary>
            /// <param name="page">The page id</param>
            /// <returns></returns>
            /// <exception cref="Logic::PageNotFoundException">Page does not exist</exception>
            LanguagePageRef  operator[](UINT  page) const
            {
               return Find(page);
            }

		      // ----------------------- MUTATORS ------------------------
            
            /// <summary>Inserts a page and it's strings to the collection. Strings from lower versions are overwritten</summary>
            /// <param name="p">The page</param>
            void  Add(const LanguagePage& p)
            {
               auto it = insert(value_type(p.ID, p));

               // Exists: Merge strings into existing page
               if (!it.second)
               {
                  LanguagePage& page = it.first->second;
                  for (auto pair : p.Strings)
                     page.Strings.Add(pair.second);
               }
            }

            /// <summary>Find a page by index</summary>
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

		      // -------------------- REPRESENTATION ---------------------
            
         };

         /// <summary>Provides constant access to the pages within a language file</summary>
         typedef MapIterator<const LanguagePage, PageCollection, PageCollection::const_iterator> const_iterator;
         
         // --------------------- CONSTRUCTION ----------------------

      public:
         LanguageFile() : ID(0), Language(GameLanguage::English)
         {}
         LanguageFile(IO::Path path) : FileBase(path), ID(0), Language(GameLanguage::English)
         {}
         LanguageFile(LanguageFile&& r) : FileBase(r), ID(r.ID), Language(r.Language), Pages(move(r.Pages))
         {}
         ~LanguageFile()
         {}

         DEFAULT_COPY(LanguageFile);	// Default copy semantics

         // --------------------- PROPERTIES ------------------------
			
		   // ---------------------- ACCESSORS ------------------------

         /// <summary>Get read-only page iterator</summary>
         /// <returns></returns>
         const_iterator begin() const 
         { 
            return const_iterator(Pages, Pages.begin()); 
         }
         
         /// <summary>Get read-only page iterator</summary>
         /// <returns></returns>
         const_iterator end() const
         { 
            return const_iterator(Pages, Pages.end());   
         }

         /// <summary>Queries whether a string is present</summary>
         /// <param name="page">The page id</param>
         /// <param name="id">The string id.</param>
         /// <returns></returns>
         bool  Contains(UINT page, UINT id) const
         { 
            return Pages.Contains(page,id); 
         }
         
         /// <summary>Read-only access to strings by ID</summary>
         /// <param name="page">The page id</param>
         /// <param name="id">The string id</param>
         /// <returns></returns>
         /// <exception cref="Logic::PageNotFoundException">Page does not exist</exception>
         /// <exception cref="Logic::StringNotFoundException">String does not exist</exception>
         LanguageStringRef  Find(UINT page, UINT id) const
         { 
            return Pages.Find(page,id); 
         }

         /// <summary>Find zero-based index of a page</summary>
         /// <param name="page">page id.</param>
         /// <returns></returns>
         /// <exception cref="Logic::PageNotFoundException">Page does not exist</exception>
         UINT  IndexOf(UINT page) const
         {
            return Pages.IndexOf(page);
         }

         /// <summary>Compares precendence of two files</summary>
         /// <param name="r">The other</param>
         /// <returns></returns>
         bool operator<(const LanguageFile& r) const
         { 
            return ID < r.ID; 
         }

         /// <summary>Compares precendence of two files</summary>
         /// <param name="r">The other</param>
         /// <returns></returns>
         bool operator>(const LanguageFile& r) const
         { 
            return ID > r.ID; 
         }

		   // ----------------------- MUTATORS ------------------------

		   // -------------------- REPRESENTATION ---------------------
      public:
         UINT            ID;
         PageCollection  Pages;
         GameLanguage    Language;
      };

   }

}

using namespace Logic::Language;
