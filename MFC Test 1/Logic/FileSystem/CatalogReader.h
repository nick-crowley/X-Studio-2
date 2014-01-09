#pragma once
#include "../Common.h"
#include "../IO/Stream.h"
#include "../IO/StringReader.h"


namespace Library
{
   namespace FileSystem
   {
      /// <summary>Reads the file declarations from catalogs</summary>
      class CatalogReader : protected IO::StringReader
      {
      public:
         // --------------------- CONSTRUCTION ----------------------

         CatalogReader(StreamPtr src);
         CatalogReader(CatalogReader&& r) : StringReader(std::move(r)) {}
         ~CatalogReader();

         // Prevent copying.  Move assign not impl
         NO_COPY(CatalogReader);
         NO_MOVE_ASSIGN(CatalogReader);

         // --------------------- PROPERTIES ------------------------
			
			// ---------------------- ACCESSORS ------------------------

			// ----------------------- MUTATORS ------------------------

         bool  ReadDeclaration(wstring&  path, DWORD&  size);

         // -------------------- REPRESENTATION ---------------------
      };


   }
}

using namespace Library::FileSystem;
