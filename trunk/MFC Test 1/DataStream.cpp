#include "stdafx.h"
#include "DataStream.h"
#include "FileStream.h"

namespace Library
{
   namespace IO
   {

      DataStream::DataStream(StreamPtr  src) : StreamFacade(src)
      {
      }

      DataStream::DataStream(Path path, FileMode mode, FileAccess access, FileShare share) 
         : StreamFacade( StreamPtr(new FileStream(path, mode, access, share)) )
      {
      }


      DataStream::~DataStream()
      {
         StreamFacade::SafeClose();
      }

      DWORD  DataStream::Read(BYTE* buffer, DWORD length)
      {
         // Read + Decode buffer
         DWORD bytesRead = StreamFacade::Read(buffer, length);
         Encode(buffer, bytesRead);
         return bytesRead;
      }

      DWORD  DataStream::Write(const BYTE* buffer, DWORD length)
      {
         // Copy buffer so we can encode it
         ByteArray copy(new BYTE[length]);
         memcpy(copy.get(), buffer, length);

         // Encode + Write
         Encode(copy.get(), length);
         return StreamFacade::Write(copy.get(), length);
      }

      /// <summary>Encodes a byte array</summary>
      /// <param name="buffer">Buffer to encode</param>
      /// <param name="length">Length of buffer</param>
      void  DataStream::Encode(byte* buffer, DWORD length)
      {
         // Encode buffer
         for (DWORD i = 0; i < length; i++)
            buffer[i] ^= DATAFILE_ENCRYPT_KEY;
      }
   }
}