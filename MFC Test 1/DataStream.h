#pragma once
#include "Common.h"
#include "Stream.h"

namespace Library
{
   namespace IO
   {

      class DataStream : public StreamFacade
      {
      const byte  DATAFILE_ENCRYPT_KEY = 0x33;

      public:
         DataStream(StreamPtr  src);
         DataStream(Path path, FileMode mode, FileAccess access, FileShare share);
         ~DataStream();

         DWORD  Read(BYTE* buffer, DWORD length);
         DWORD  Write(const BYTE* buffer, DWORD length);

      private:
         void   Encode(byte* buffer, DWORD length);
      };

   }
}