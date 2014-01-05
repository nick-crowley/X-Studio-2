#include "stdafx.h"
#include "CatalogStream.h"

namespace Library
{
   namespace IO
   {
      CatalogStream::CatalogStream(Stream*  ps) 
         : StreamFacade(ps)
      {
      }

      CatalogStream::CatalogStream(Path path, FileMode mode, FileAccess access, FileShare share) 
         : StreamFacade(new FileStream(path, mode, access, share))
      {
      }


      CatalogStream::~CatalogStream()
      {
      }

      DWORD  CatalogStream::Read(BYTE* buffer, DWORD length)
      {
         // Preserve origin, fill buffer
         DWORD startPos = GetPosition(),
               bytesRead = StreamFacade::Read(buffer, length);

         // Encode buffer + return
         Encode(buffer, bytesRead, startPos);
         return bytesRead;
      }

      DWORD  CatalogStream::Write(const BYTE* buffer, DWORD length)
      {
         // Copy buffer so we can encode it
         ByteArray copy(new BYTE[length]);
         memcpy(copy.get(), buffer, length);

         // Encode + Write
         Encode(copy.get(), length, GetPosition());
         return StreamFacade::Write(copy.get(), length);
      }

      /// <summary>Calculates the encoding key for a given position in the stream</summary>
      /// <param name="streamPos">Position in stream</param>
      /// <returns></returns>
      byte  CatalogStream::CalculateKey(DWORD  streamPos)
      {
         return (byte)((streamPos + CATALOGUE_DECRYPT_KEY) % 256);
      }

      /// <summary>Encodes a byte array</summary>
      /// <param name="buffer">Buffer to encode</param>
      /// <param name="length">Length of buffer</param>
      /// <param name="streamPos">Stream position matching the first buffer element to be encoded</param>
      void  CatalogStream::Encode(byte* buffer, DWORD length, DWORD  streamPos)
      {
         byte key = CalculateKey(streamPos);

         // Encode buffer
         for (DWORD i = 0; i < length; i++)
            buffer[i] ^= key++;
      }

   }
}