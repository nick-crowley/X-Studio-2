#pragma once

#include "FileStream.h"

namespace Logic
{
   namespace IO
   {

      /// <summary>Provides stream access to loose files using the X3TC encryption scheme</summary>
      class LogicExport EncryptedX3Stream : public StreamDecorator
      {
      private:
         static const byte  DECRYPT_SEED = 0xC8;
         
         byte DECRYPT_KEY;

         // --------------------- CONSTRUCTION ----------------------

      public:
         EncryptedX3Stream(StreamPtr  src);
         EncryptedX3Stream(Path path, FileMode mode, FileAccess access, FileShare share = FileShare::AllowRead);
         ~EncryptedX3Stream();

         // Prevent copying/moving
         NO_MOVE(EncryptedX3Stream);
         NO_COPY(EncryptedX3Stream);

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
