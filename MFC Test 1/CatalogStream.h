#pragma once
#include "Common.h"
#include "FileStream.h"


namespace Library
{
   namespace IO
   {

      class CatalogStream : public StreamFacade
      {
      const byte  CATALOGUE_DECRYPT_KEY = 0xDB;

      public:
         CatalogStream(Stream*  ps, bool  owner);
         CatalogStream(Path path, FileMode mode, FileAccess access, FileShare share = FileShare::AllowRead);
         ~CatalogStream();

         DWORD  Read(BYTE* buffer, DWORD length);
         DWORD  Write(const BYTE* buffer, DWORD length);

      private:
         byte  CalculateKey(DWORD  streamPos);
         void  Encode(byte* buffer, DWORD length, DWORD  streamPos);

         DWORD  Position;
      };


   }
}