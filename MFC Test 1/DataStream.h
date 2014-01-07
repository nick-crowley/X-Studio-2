#pragma once
#include "Common.h"
#include "Stream.h"

namespace Library
{
   namespace IO
   {

      /// <summary>Provides stream access to the contents of catalog data files</summary>
      class DataStream : public StreamFacade
      {
      const byte  DATAFILE_ENCRYPT_KEY = 0x33;
         
         // --------------------- CONSTRUCTION ----------------------

      public:
         DataStream(StreamPtr  src);
         DataStream(Path path, FileMode mode, FileAccess access, FileShare share);
         ~DataStream();

         // Prevent copying/moving
         NO_MOVE(DataStream);
         NO_COPY(DataStream);

         // --------------------- PROPERTIES ------------------------
			
			// ---------------------- ACCESSORS ------------------------

			// ----------------------- MUTATORS ------------------------

         DWORD  Read(BYTE* buffer, DWORD length);
         DWORD  Write(const BYTE* buffer, DWORD length);

      private:
         void   Encode(byte* buffer, DWORD length);

         // -------------------- REPRESENTATION ---------------------
      };

   }
}