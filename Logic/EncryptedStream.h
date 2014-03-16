#pragma once

#include "FileStream.h"

namespace Logic
{
   namespace IO
   {

      /// <summary>Provides stream access to the contents of catalogs</summary>
      class LogicExport EncryptedStream : public StreamFacade
      {
      private:
         const byte  DECRYPT_KEY = 0x7E; // (byte)(0xB6 ^ 0x8C);

         // --------------------- CONSTRUCTION ----------------------

      public:
         EncryptedStream(StreamPtr  src);
         EncryptedStream(Path path, FileMode mode, FileAccess access, FileShare share = FileShare::AllowRead);
         ~EncryptedStream();

         // Prevent copying/moving
         NO_MOVE(EncryptedStream);
         NO_COPY(EncryptedStream);

         // --------------------- PROPERTIES ------------------------
			
			// ---------------------- ACCESSORS ------------------------

			// ----------------------- MUTATORS ------------------------
      public:
         DWORD  Read(BYTE* buffer, DWORD length);
         DWORD  Write(const BYTE* buffer, DWORD length);

      private:
         void  Encode(byte* buffer, DWORD length);

         // -------------------- REPRESENTATION ---------------------
      private:
         DWORD  Position;
      };


   }
}

using namespace Logic::IO;
