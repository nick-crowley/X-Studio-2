#include "stdafx.h"
#include "XCatalog.h"
#include "CatalogStream.h"

namespace Library
{
   namespace FileSystem
   {

      XCatalog::XCatalog(XFileSystem& vfs, Path path) 
         : FullPath(path), FileSystem(vfs), FileLock(new FileStream(FullPath.c_str(), FileMode::OpenExisting, FileAccess::Read)) 
      { 
      }

      XCatalog::~XCatalog() 
      { 
      }


      CatalogReader  XCatalog::GetReader()
      {
         return CatalogReader( StreamPtr(new CatalogStream(FullPath, FileMode::OpenExisting, FileAccess::Read)) );
      }
   }
}
