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
      private:
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

      public:
         class iterator : public std::iterator<std::forward_iterator_tag, LanguageString>
         {
         public:
            typedef FileCollection::iterator   FileIterator;

         private:
            typedef LanguagePage::StringCollection::iterator  StringIterator;
            typedef LanguageFile::PageCollection::iterator    PageIterator;

            FileCollection* coll;
            FileIterator    file;
            PageIterator    page;
            StringIterator  str;

            /*
            MyIterator(int* x) :p(x) {}
           MyIterator(const MyIterator& mit) : p(mit.p) {}
           MyIterator& operator++() {++p;return *this;}
           MyIterator operator++(int) {MyIterator tmp(*this); operator++(); return tmp;}
           bool operator==(const MyIterator& rhs) {return p==rhs.p;}
           bool operator!=(const MyIterator& rhs) {return p!=rhs.p;}
           int& operator*() {return *p;}
            */
         public:
            iterator(FileCollection& f, FileIterator pos) : coll(&f), file(pos)
            {
               if (file != f.end())
               {
                  page = const_cast<LanguageFile::PageCollection&>(file->Pages).begin();  
                  
                  if (page != file->Pages.end())
                     str = (page->second.Strings).begin();  
               }
            }

            iterator& operator++() 
            { 
               // Advance string
               if (str != page->second.Strings.end())
               {
                  //if (++str == page->second.Strings.end())
                  ++str;
               }
               // Advance page
               else if (page != file->Pages.end())
                  ++page;
               // Advance file
               else if (file != coll->end())
                  ++file;

               return *this;
            }

            iterator operator++(int) 
            {
               iterator tmp(*this); 
               operator++(); 
               return tmp;
            }

            LanguageString& operator*() 
            {
               return str->second;
            }
            
            bool operator==(const iterator& r) const { return coll==r.coll && file==r.file && page==r.page && str==r.str; }
            bool operator!=(const iterator& r) const { return coll!=r.coll || file!=r.file || page!=r.page || str!=r.str; }
         };

         // --------------------- CONSTRUCTION ----------------------

      private:
         StringLibrary();
      public:
         virtual ~StringLibrary();

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
         iterator begin() { return iterator(Files, Files.begin()); }
         iterator end();
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
