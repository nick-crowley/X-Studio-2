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

            /// <summary>Finds the specified page.</summary>
            /// <param name="page">The page id</param>
            /// <returns></returns>
            /// <exception cref="Logic::PageNotFoundException">Page does not exist</exception>
            const LanguagePage&  operator[](UINT  page) const
            {
               return Find(page);
            }
         };

         /// <summary>Provides constant access to the strings within a language file</summary>
         class const_iterator : public std::iterator<std::forward_iterator_tag, LanguageString>
         {
            // ------------------------ TYPES --------------------------
         private:
            typedef LanguagePage::StringCollection::const_iterator  StringIterator;
            typedef PageCollection::const_iterator                  PageIterator;
            
            // --------------------- CONSTRUCTION ----------------------
         public:
            const_iterator(const PageCollection& c, PageIterator pos) : Collection(&c), Page(pos), String(STRING_END)
            {
               // Get first string in first non-empty page, if any
               if (Page != Collection->end())
               {
                  String = GetStrings().begin();
                  NextPage();
               }
            }

            // --------------------- PROPERTIES ------------------------
			
		      // ---------------------- ACCESSORS ------------------------
         public:
            const LanguageString& operator*() 
            {
               return String->second;
            }

            bool operator==(const const_iterator& r) const { return Collection==r.Collection && Page==r.Page && String==r.String; }
            bool operator!=(const const_iterator& r) const { return Collection!=r.Collection || Page!=r.Page || String!=r.String; }

         private:
            const LanguagePage::StringCollection& GetStrings()
            {
               return Page->second.Strings;
            }

            // ----------------------- MUTATORS ------------------------
         public:
            const_iterator& operator++() 
            { 
               // Advance string + Advance to next non-empty Page if necessary
               ++String;
               NextPage();
               return *this;
            }

            const_iterator operator++(int) 
            {
               const_iterator tmp(*this); 
               operator++(); 
               return tmp;
            }
            
         private:
            void NextPage()
            {
               // End of Page
               while (String == GetStrings().end())
               {
                  // Advance Page + reset string
                  if (++Page != Collection->end())
                     String = GetStrings().begin();
                  else
                  {  // End of pages
                     String = STRING_END;
                     break;
                  }
               }
            }
            
            // -------------------- REPRESENTATION ---------------------
         private:
            const StringIterator  STRING_END;

            const PageCollection* Collection;
            PageIterator          Page;
            StringIterator        String;
         };

         // --------------------- CONSTRUCTION ----------------------

      public:
         LanguageFile();
         LanguageFile(UINT id, GameLanguage l) : ID(id), Language(l) {}
         ~LanguageFile();

         // --------------------- PROPERTIES ------------------------
			
		   // ---------------------- ACCESSORS ------------------------

         /// <summary>Get constant string iterator</summary>
         /// <returns></returns>
         const_iterator begin() const 
         { 
            return const_iterator(Pages, Pages.begin()); 
         }

         /// <summary>Queries whether a string is present</summary>
         /// <param name="page">The page id</param>
         /// <param name="id">The string id.</param>
         /// <returns></returns>
         bool  Contains(UINT page, UINT id) const
         { 
            return Pages.Contains(page,id); 
         }
         
         /// <summary>Get constant string iterator</summary>
         /// <returns></returns>
         const_iterator end() const
         { 
            return const_iterator(Pages, Pages.end());   
         }

         /// <summary>Finds the specified string</summary>
         /// <param name="page">The page id</param>
         /// <param name="id">The string id</param>
         /// <returns></returns>
         /// <exception cref="Logic::PageNotFoundException">Page does not exist</exception>
         /// <exception cref="Logic::StringNotFoundException">String does not exist</exception>
         LanguageString  Find(UINT page, UINT id) const
         { 
            return Pages.Find(page,id); 
         }

		   // ----------------------- MUTATORS ------------------------

		   // -------------------- REPRESENTATION ---------------------

         UINT            ID;
         PageCollection  Pages;
         GameLanguage    Language;
      };

   }

}

using namespace Logic::Language;
