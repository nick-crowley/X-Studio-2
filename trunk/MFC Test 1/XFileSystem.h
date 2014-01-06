#pragma once
#include "Common.h"

namespace Library
{
   namespace FileSystem
   {

      class XCatalog
      {
      public:
         XCatalog(Path path);
         ~XCatalog();
      };


      class XFileSystem
      {
      public:
         XFileSystem(Path folder);
         virtual ~XFileSystem();

         //DWORD  Enumerate();

      private:
         list<XCatalog>  Catalogs;
      };

   }
}
