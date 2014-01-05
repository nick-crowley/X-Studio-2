#include "stdafx.h"
#include "GZipStream.h"

namespace Library
{
   namespace IO
   {

      GZipStream::GZipStream(Stream* ps) : StreamFacade(ps)
      {
         // Clear structs
         ZeroMemory(&ZStream, sizeof(ZStream));
         ZeroMemory(&ZHeader, sizeof(ZHeader));

         // Init stream + extract header
         if (inflateInit(&ZStream) != Z_OK) // || inflateGetHeader(&ZStream, &ZHeader) != Z_OK)
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
         inflateEnd(&ZStream);
         StreamFacade::Close();
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
         throw InvalidOperationException(HERE, L"Seeking not allowed");
      }

      void  GZipStream::SetLength(DWORD  length)
      {
         throw InvalidOperationException(HERE, L"Resizing not allowed");
      }

      DWORD  GZipStream::Read(BYTE* output, DWORD length)
      {
         // Ensure we can read
         if (!StreamFacade::CanRead())
            throw InvalidOperationException(HERE, L"No read access");

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
         throw InvalidOperationException(HERE, L"Not implemented");
      }
   }
}
