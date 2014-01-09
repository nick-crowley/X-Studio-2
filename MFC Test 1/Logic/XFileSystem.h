#pragma once
#include "Common.h"
#include "XCatalog.h"
#include "XFileInfo.h"

using namespace Logic::FileSystem;

namespace Logic
{
   namespace FileSystem
   {
      
      /// <summary>Contains enumerated catalogs and catalog-based files. Provides access to physical/catalog files</summary>
      class XFileSystem
      {
      private:
         /// <summary>Collection of catalogs, sorted by precedence (highest->lowest)</summary>
         class CatalogCollection : private list<XCatalog>
         {
            typedef list<XCatalog> Base;

         public:
            // --------------------- CONSTRUCTION ----------------------

            CatalogCollection() {};
            ~CatalogCollection() { Clear(); };  // Unlock all catalogs

            // ---------------------- PROPERTIES -----------------------

            PROPERTY_GET(DWORD,Count,GetCount);

            // ---------------------- ACCESSORS ------------------------

            iterator  Begin()            { return Base::begin();           }
            iterator  End()              { return Base::end();             }
            DWORD     GetCount()         { return Base::size();            }

            // ----------------------- MUTATORS ------------------------

            void      Add(XCatalog&& c)  { Base::push_front(std::move(c)); }
            void      Clear()            { Base::clear();                  }
         };

         
         /// <summary>Sorts file descriptors by their path</summary>
         class UniquePath : public binary_function<XFileInfo, XFileInfo, bool>
         {
         public:
            // ---------------------- ACCESSORS ------------------------

            bool operator()(XFileInfo a, XFileInfo b)
            {
               return a.FullPath < b.FullPath;
            }
         };
         
         /// <summary>Sorts file descriptors by their precedence in descending order</summary>
         class DescendingPrecedence : public binary_function<XFileInfo, XFileInfo, bool>
         {
         public:
            // ---------------------- ACCESSORS ------------------------

            bool operator()(XFileInfo a, XFileInfo b)
            {
               return a.Precedence > b.Precedence;
            }
         };

         /// <summary>Collection of file descriptors with custom sort rules</summary>
         template<typename T>
         class XFileInfoCollection : public set<XFileInfo, T>
         {
            typedef set<XFileInfo, T> Base;

         public:
            // ---------------------- PROPERTIES -----------------------

            PROPERTY_GET(DWORD,Count,GetCount);

            // ---------------------- ACCESSORS ------------------------

            DWORD     GetCount()          { return Base::size();                       }

            // ----------------------- MUTATORS ------------------------

            bool      Add(XFileInfo&& f)  { return Base::insert(std::move(f)).second;  }
            void      Clear()             { Base::clear();                             }
         };

         /// <summary>Collection used by file system</summary>
         typedef XFileInfoCollection<UniquePath>            FileCollection;

         /// <summary>Collection used to hold results of a search</summary>
         typedef XFileInfoCollection<DescendingPrecedence>  ResultCollection;

      public:
         // --------------------- CONSTRUCTION ----------------------

         XFileSystem(Path folder, GameVersion version);
         virtual ~XFileSystem();

         // Prevent moving/copying
         NO_MOVE(XFileSystem);
         NO_COPY(XFileSystem);

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------

         bool         Contains(Path  path, bool  matchExt) const;
         XFileInfo    Find(Path  path, bool  matchExt) const;

         Path         GetFolder() const   { return Folder;  }
         GameVersion  GetVersion() const  { return Version; }

      private:
         ResultCollection  Query(Path  path, bool  matchExt) const;

			// ----------------------- MUTATORS ------------------------

      public:
         DWORD        Enumerate();
         
      private:
         DWORD        EnumerateCatalogs();
         DWORD        EnumerateFiles();
         
         // -------------------- REPRESENTATION ---------------------

         CatalogCollection  Catalogs;
         FileCollection     Files;
         Path               Folder;
         GameVersion        Version;
      };


      
   }
}
