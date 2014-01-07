#pragma once
#include "Common.h"
#include "FileStream.h"
#include "CatalogReader.h"

//using namespace Library::FileSystem;

namespace Library
{
   namespace FileSystem
   {
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

   }
}
