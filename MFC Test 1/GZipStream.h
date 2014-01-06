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
      
         GZipStream(Stream*  ps, bool owner, Operation  op);
         ~GZipStream();

         bool  CanSeek() const   { return false; }

         void  Close();
         DWORD  GetLength() ;
         DWORD  GetPosition() const;

         void  Seek(DWORD  offset, SeekOrigin  mode);
         void  SetLength(DWORD  length);

         DWORD  Read(BYTE* buffer, DWORD length);
         DWORD  Write(const BYTE* buffer, DWORD length);

      private:
         z_stream  ZStream;
         gz_header ZHeader;
         ByteArray Input;
         Operation Mode;
         bool      Closed;
      };

   }
}
