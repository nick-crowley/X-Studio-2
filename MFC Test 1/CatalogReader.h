#pragma once
#include "Common.h"
#include "Stream.h"
#include "StringReader.h"

//using namespace Library::FileSystem;

namespace Library
{
   namespace IO
   {
      

      class CatalogReader : protected IO::StringReader
      {
      public:
         CatalogReader(StreamPtr src);
         CatalogReader(CatalogReader&& r) : StringReader(std::move(r)) {}
         ~CatalogReader();

         // Prevent copying.  Move assign not impl
         NO_COPY(CatalogReader);
         CatalogReader& operator=(CatalogReader&& r) = delete;

         bool  ReadDeclaration(wstring&  path, DWORD&  size);
      };


   }
}
