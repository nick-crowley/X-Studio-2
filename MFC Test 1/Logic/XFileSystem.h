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
         class CatalogCollection : public list<XCatalog>
         {
         public:
            // --------------------- CONSTRUCTION ----------------------

            CatalogCollection() {};
            ~CatalogCollection() { clear(); };  // Unlock all catalogs

            // ---------------------- PROPERTIES -----------------------

            PROPERTY_GET(size_type,Count,size);

            // ----------------------- MUTATORS ------------------------

            void  Add(XCatalog&& c)  { push_front(std::move(c)); }
            
         };

         
         /// <summary>Sorts file descriptors by their key</summary>
         class UniquePath : public binary_function<XFileInfo, XFileInfo, bool>
         {
         public:
            // ---------------------- ACCESSORS ------------------------

            bool operator()(XFileInfo a, XFileInfo b)
            {
               return a.Key < b.Key;
            }
         };
         
         /// <summary>Sorts file descriptors by their precedence</summary>
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
         public:
            // ---------------------- PROPERTIES -----------------------
         
            PROPERTY_GET(size_type,Count,size);

            // ----------------------- MUTATORS ------------------------
         
            /// <summary>Attempts to add a file to the collection, overwriting any of lower precedence</summary>
            /// <param name="f">The file to add</param>
            void  Add(XFileInfo&& f)
            {
               _Pairib res = insert(std::move(f));

               // Exists: Overwrite if higher precendence
               if (!res.second && f.Precedence > res.first->Precedence)
               {
                  erase(res.first);
                  insert(std::move(f));
               }
            }
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
