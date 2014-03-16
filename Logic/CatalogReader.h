#pragma once

#include "Stream.h"
#include "StringReader.h"


namespace Logic
{
   namespace IO
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

using namespace Logic::IO;
