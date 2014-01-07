#pragma once
#include "Common.h"
#include "XCatalog.h"
#include "XFileInfo.h"

using namespace Library::FileSystem;

namespace Library
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
         

         /// <summary>Collection of file descriptors, sorted by path (no duplicates)</summary>
         class FileCollection : private set<XFileInfo, UniquePath>
         {
            typedef set<XFileInfo, UniquePath> Base;

         public:
            // ---------------------- PROPERTIES -----------------------

            PROPERTY_GET(DWORD,Count,GetCount);

            // ---------------------- ACCESSORS ------------------------

            DWORD     GetCount()          { return Base::size();                       }

            // ----------------------- MUTATORS ------------------------

            bool      Add(XFileInfo&& f)  { return Base::insert(std::move(f)).second;  }
            void      Clear()             { Base::clear();                             }
         };

      public:
         // --------------------- CONSTRUCTION ----------------------

         XFileSystem(Path folder, GameVersion version);
         virtual ~XFileSystem();

         // Prevent moving/copying
         NO_MOVE(XFileSystem);
         NO_COPY(XFileSystem);

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------

         Path         GetFolder()   { return Folder;  }
         GameVersion  GetVersion()  { return Version; }

			// ----------------------- MUTATORS ------------------------

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
