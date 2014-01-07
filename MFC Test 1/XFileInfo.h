#pragma once
#include "Common.h"

//using namespace Library::FileSystem;

namespace Library
{
   namespace FileSystem
   {
      class XFileSystem;
      class XCatalog;
      

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


      
   }
}
