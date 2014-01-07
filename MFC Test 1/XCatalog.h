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
      class XFileSystem;
      

      class XCatalog
      {
         typedef shared_ptr<FileStream>  FileStreamPtr;

      public:
         XCatalog(XFileSystem& vfs, Path path);
         virtual ~XCatalog();

         CatalogReader  GetReader();
         Path           GetFullPath()  { return FullPath; }
         
      private:
         Path           FullPath;
         FileStreamPtr  FileLock;
         XFileSystem&   FileSystem;
      };

      

      class CatalogReader : protected IO::StringReader
      {
      public:
         CatalogReader(StreamPtr src);
         CatalogReader(CatalogReader&& r) : StringReader(std::move(r)) {}
         ~CatalogReader();

         // Prevent copying & assignment
         CatalogReader(const CatalogReader&) = delete;
         CatalogReader& operator=(const CatalogReader& r) = delete;
         CatalogReader& operator=(CatalogReader&& r) = delete;

         bool  ReadDeclaration(wstring&  path, DWORD&  size);
      };


   }
}
