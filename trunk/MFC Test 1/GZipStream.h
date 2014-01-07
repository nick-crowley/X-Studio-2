#pragma once
#include "Common.h"
#include "Stream.h"
#include "zlib.h"

namespace Library
{
   namespace IO
   {

      /// <summary>Provides stream access to the contents of GZip files</summary>
      class GZipStream : public StreamFacade
      {
      private:
         const int  WINDOW_SIZE = 15,
                    DETECT_HEADER = 16;
         
      public:
         enum class Operation   { Compression, Decompression };
      
         // --------------------- CONSTRUCTION ----------------------

         GZipStream(StreamPtr  src, Operation  op);
         ~GZipStream();

         // Prevent copying/moving
         NO_MOVE(GZipStream);
         NO_COPY(GZipStream);

         // --------------------- PROPERTIES ------------------------
			
			// ---------------------- ACCESSORS ------------------------

         bool   CanSeek() const   { return false; }

         DWORD  GetLength();
         DWORD  GetPosition() const;

      private:
         bool   IsClosed() const;

         // ----------------------- MUTATORS ------------------------

      public:
         void   Close();
         void   SafeClose();
         void   Seek(DWORD  offset, SeekOrigin  mode);
         void   SetLength(DWORD  length);

         DWORD  Read(BYTE* buffer, DWORD length);
         DWORD  Write(const BYTE* buffer, DWORD length);

         // -------------------- REPRESENTATION ---------------------

         z_stream     ZStream;
         gz_header    ZHeader;
         ByteArrayPtr Input;
         Operation    Mode;
      };

   }
}
