#pragma once

#include "FileStream.h"

namespace Logic
{
   namespace IO
   {

      /// <summary>Provides stream access to the contents of catalogs</summary>
      class LogicExport EncryptedX2Stream : public StreamFacade
      {
      private:
         const byte  DECRYPT_KEY = 0x7E; // (byte)(0xB6 ^ 0x8C);

         // --------------------- CONSTRUCTION ----------------------

      public:
         EncryptedX2Stream(StreamPtr  src);
         EncryptedX2Stream(Path path, FileMode mode, FileAccess access, FileShare share = FileShare::AllowRead);
         ~EncryptedX2Stream();

         // Prevent copying/moving
         NO_MOVE(EncryptedX2Stream);
         NO_COPY(EncryptedX2Stream);

         // ------------------------ STATIC -------------------------
      public:
         static bool  IsEncrypted(StreamPtr s);

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
