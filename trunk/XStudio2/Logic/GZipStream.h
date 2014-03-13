#pragma once
#include "Common.h"
#include "Stream.h"
#include "zlib.h"

using namespace Logic::IO;

namespace Logic
{
   namespace IO
   {

      /// <summary>Provides stream access to the contents of GZip files</summary>
      class GZipStream : public StreamFacade
      {
      private:
         const int  WINDOW_SIZE = 15,
                    DETECT_HEADER = 16;
         const int  COMPRESS_BUFFER = 256*1024;
         
      public:
         enum class Operation   { Compression, Decompression };
      
         // --------------------- CONSTRUCTION ----------------------
      public:
         GZipStream(StreamPtr  src, Operation  op);
         ~GZipStream();

         // Prevent copying/moving
         NO_MOVE(GZipStream);
         NO_COPY(GZipStream);

         // --------------------- PROPERTIES ------------------------
			
			// ---------------------- ACCESSORS ------------------------
      public:
         bool   CanSeek() const;
         DWORD  GetPosition() const;

      protected:
         bool   IsClosed() const;

         // ----------------------- MUTATORS ------------------------

      public:
         void   Close();
         DWORD  GetLength();
         void   SafeClose();
         void   Seek(LONG  offset, SeekOrigin  mode);
         void   SetFileName(const wstring& name);
         void   SetLength(DWORD  length);

         DWORD  Read(BYTE* buffer, DWORD length);
         DWORD  Write(const BYTE* buffer, DWORD length);

         // -------------------- REPRESENTATION ---------------------
      protected:
         z_stream     ZStream;
         gz_header    ZHeader;
         ByteArrayPtr Buffer;
         Operation    Mode;
         string       FileName;
      };

   }
}
