#pragma once
#include "Common.h"
#include "FileStream.h"

using namespace Library::IO;

namespace Library
{
   namespace IO
   {

      /// <summary>Provides stream access to the contents of catalogs</summary>
      class CatalogStream : public StreamFacade
      {
      const byte  CATALOGUE_DECRYPT_KEY = 0xDB;

         // --------------------- CONSTRUCTION ----------------------

      public:
         CatalogStream(StreamPtr  src);
         CatalogStream(Path path, FileMode mode, FileAccess access, FileShare share = FileShare::AllowRead);
         ~CatalogStream();

         // Prevent copying/moving
         NO_MOVE(CatalogStream);
         NO_COPY(CatalogStream);

         // --------------------- PROPERTIES ------------------------
			
			// ---------------------- ACCESSORS ------------------------

			// ----------------------- MUTATORS ------------------------

         DWORD  Read(BYTE* buffer, DWORD length);
         DWORD  Write(const BYTE* buffer, DWORD length);

      private:
         byte  CalculateKey(DWORD  streamPos);
         void  Encode(byte* buffer, DWORD length, DWORD  streamPos);

         // -------------------- REPRESENTATION ---------------------

         DWORD  Position;
      };


   }
}