#include "stdafx.h"
#include "GZipStream.h"

namespace Library
{
   namespace IO
   {
      GZipStream::GZipStream(Stream* ps, bool owner, Operation  op) : StreamFacade(ps, owner), Mode(op), Closed(false)
      {
         REQUIRED(ps);

         // Clear structs
         ZeroMemory(&ZStream, sizeof(ZStream));
         ZeroMemory(&ZHeader, sizeof(ZHeader));

         // Init stream + extract header
         if (inflateInit2(&ZStream, WINDOW_SIZE+DETECT_HEADER) != Z_OK) 
            throw GZipException(HERE, ZStream.msg);

         // Allocate + set input buffer
         Input.reset(new byte[ps->GetLength()]);
         ZStream.next_in = Input.get();
      }

      GZipStream::~GZipStream()
      {
         Close();
      }

      void  GZipStream::Close()
      {
         if (!Closed)
         {
            inflateEnd(&ZStream);
            StreamFacade::Close();
            Closed = true;
         }
      }

      DWORD  GZipStream::GetLength() 
      {
         DWORD pos = StreamFacade::GetPosition(),
               size = 0;

         // Extract uncompressed length from last four bytes
         StreamFacade::Seek(-4, SeekOrigin::End);
         StreamFacade::Read((byte*)&size, 4);
         StreamFacade::Seek(pos, SeekOrigin::Begin);
         return size;
      }

      DWORD  GZipStream::GetPosition() const
      {
         return ZStream.total_out;
      }

      void  GZipStream::Seek(DWORD  offset, SeekOrigin  mode)
      {
         throw NotSupportedException(HERE, ERR_NO_SEEK_ACCESS);
      }

      void  GZipStream::SetLength(DWORD  length)
      {
         throw NotSupportedException(HERE, L"Resizing not allowed");
      }

      DWORD  GZipStream::Read(BYTE* output, DWORD length)
      {
         REQUIRED(output);

         // Ensure we can read
         if (!StreamFacade::CanRead())
            throw NotSupportedException(HERE, ERR_NO_READ_ACCESS);

         // Supply output buffer
         ZStream.next_out = output;
         ZStream.avail_out = length;

         // Re-Fill input buffer if necessary
         if (ZStream.avail_in == 0)
            ZStream.avail_in += StreamFacade::Read(ZStream.next_in, length);
         
         // Decompress
         switch (int res = inflate(&ZStream, Z_FINISH))
         {
         // Success/EOF: Return count decompressed
         case Z_STREAM_END:
         case Z_BUF_ERROR:
         case Z_OK:
            return ZStream.avail_out;

         // Error: throw
         default:
            throw GZipException(HERE, ZStream.msg);
         }
      }

      DWORD  GZipStream::Write(const BYTE* buffer, DWORD length)
      {
         REQUIRED(buffer);

         throw NotImplementedException(HERE, L"GZip compression");
      }
   }
}
