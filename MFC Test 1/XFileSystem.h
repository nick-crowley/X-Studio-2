#pragma once
#include "Common.h"
#include "FileStream.h"
#include "StringReader.h"

//using namespace Library::FileSystem;

namespace Library
{
   namespace FileSystem
   {
      class CatalogReader;
      class XFileInfo;
      class XFileSystem;
      

      class XCatalog
      {
         typedef shared_ptr<FileStream>  FileStreamPtr;

      public:
         XCatalog(XFileSystem& vfs, Path path);
         virtual ~XCatalog();

         CatalogReader*  CreateReader();

      private:
         Path           FullPath;
         FileStreamPtr  FileLock;
         XFileSystem&   FileSystem;
      };

      

      class CatalogReader : protected IO::StringReader
      {
      public:
         CatalogReader(XFileSystem&  vfs, XCatalog&  cat, Stream*  ps, bool  owner);
         ~CatalogReader();

         XFileInfo*  ReadDeclaration();

      private:
         XCatalog&     Catalog;
         XFileSystem&  FileSystem;
      };


      enum class FileSource  { Catalog, Physical };

      class XFileInfo
      {
      public:
         XFileInfo(Path p);
         XFileInfo(XFileSystem& vfs, XCatalog& cat, Path subPath, DWORD pos);
         virtual ~XFileInfo();

         XFileSystem* FileSystem;
         XCatalog*    Catalog;
         Path         FullPath;
         DWORD        Offset;
         FileSource   Source;
      };


      class XFileSystem
      {
      private:
         class CatalogCollection : private list<XCatalog>
         {
            typedef list<XCatalog> Base;

         public:
            CatalogCollection() {};
            ~CatalogCollection() { Clear(); };  // Unlock all catalogs

            __declspec(property(get=GetCount)) DWORD Count;

            void      Add(XCatalog&& c)  { Base::push_front(std::move(c)); }
            void      Clear()            { Base::clear();                  }
            DWORD     GetCount()         { return Base::size();            }
            iterator  Begin()            { return Base::begin();           }
            iterator  End()              { return Base::end();             }
         };

         typedef shared_ptr<XFileInfo>  XFileInfoPtr;

         class UniquePath : public binary_function<XFileInfoPtr, XFileInfoPtr, bool>
         {
         public:
            bool operator()(XFileInfoPtr a, XFileInfoPtr b)
            {
               return a.get()->FullPath < b.get()->FullPath;
            }
         };
         
         class FileCollection : private set<XFileInfoPtr, UniquePath>
         {
            typedef set<XFileInfoPtr, UniquePath> Base;

         public:
            __declspec(property(get=GetCount)) DWORD Count;

            bool      Add(XFileInfoPtr f)  { return Base::insert(f).second; }
            void      Clear()              { Base::clear();                 }
            DWORD     GetCount()           { return Base::size();           }
         };

      public:
         XFileSystem(Path folder, GameVersion version);
         virtual ~XFileSystem();

         DWORD  Enumerate();

         Path         GetFolder()   { return Folder;  }
         GameVersion  GetVersion()  { return Version; }

      private:
         DWORD  EnumerateCatalogs();
         DWORD  EnumerateFiles();

         CatalogCollection  Catalogs;
         FileCollection     Files;
         Path               Folder;
         GameVersion        Version;
      };


      
   }
}
