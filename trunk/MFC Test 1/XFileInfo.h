#pragma once
#include "Common.h"

//using namespace Library::FileSystem;

namespace Library
{
   
   namespace FileSystem
   {
      class XFileSystem;
      class XCatalog;
      
      /// <summary>Identifies whether a file is on disk or within a catalog</summary>
      enum class FileSource  { Catalog, Physical };

      /// <summary>Represents a file descriptor for any physical or catalog based file</summary>
      class XFileInfo
      {
      public:
         // Construction

         XFileInfo(Path p);
         XFileInfo(XFileSystem& vfs, XCatalog& cat, Path subPath, DWORD pos);
         virtual ~XFileInfo();

         // Representation

         XFileSystem* FileSystem;
         XCatalog*    Catalog;
         Path         FullPath;
         DWORD        Offset;
         FileSource   Source;
      };


      
   }
}
