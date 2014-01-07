#pragma once
#include "Common.h"
#include "Stream.h"
#include "zlib.h"

namespace Library
{
   namespace IO
   {

      class GZipStream : public StreamFacade
      {
      private:
         const int  WINDOW_SIZE = 15,
                    DETECT_HEADER = 16;

      public:
         enum class Operation   { Compression, Decompression };
      
         GZipStream(StreamPtr  src, Operation  op);
         ~GZipStream();

         // Prevent copying/moving
         NO_MOVE(GZipStream);
         NO_COPY(GZipStream);

         bool   CanSeek() const   { return false; }

         void   Close();
         DWORD  GetLength() ;
         DWORD  GetPosition() const;

         void   SafeClose();
         void   Seek(DWORD  offset, SeekOrigin  mode);
         void   SetLength(DWORD  length);

         DWORD  Read(BYTE* buffer, DWORD length);
         DWORD  Write(const BYTE* buffer, DWORD length);

      private:
         bool   IsClosed() const;

         z_stream  ZStream;
         gz_header ZHeader;
         ByteArrayPtr Input;
         Operation Mode;
      };

   }
}
