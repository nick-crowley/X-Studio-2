#include "stdafx.h"
#include "XFileSystem.h"
#include "CatalogStream.h"

namespace Library
{
   namespace FileSystem
   {

      XCatalog::XCatalog(XFileSystem& vfs, Path path) 
         : FullPath(path), FileSystem(vfs), FileLock(new FileStream(FullPath.c_str(), FileMode::OpenExisting, FileAccess::Read, FileShare::AllowRead)) 
      { 
      }

      XCatalog::~XCatalog() 
      { 
      }


      CatalogReader*  XCatalog::CreateReader()
      {
         //return new CatalogReader(new CatalogStream(FullPath, FileMode::OpenExisting, FileAccess::Read));
         return nullptr;
      }
   }
}
