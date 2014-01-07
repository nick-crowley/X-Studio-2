#include "stdafx.h"
#include "DataStream.h"
#include "FileStream.h"

namespace Library
{
   namespace IO
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Creates a data stream using another stream as input</summary>
      /// <param name="src">The input stream.</param>
      DataStream::DataStream(StreamPtr  src) : StreamFacade(src)
      {
      }

      /// <summary>Creates a data stream using a file as input</summary>
      /// <param name="path">The full path.</param>
      /// <param name="mode">The creation mode.</param>
      /// <param name="access">The file access.</param>
      /// <param name="share">The sharing permitted.</param>
      DataStream::DataStream(Path path, FileMode mode, FileAccess access, FileShare share) 
         : StreamFacade( StreamPtr(new FileStream(path, mode, access, share)) )
      {
      }

      /// <summary>Closes the input stream</summary>
      DataStream::~DataStream()
      {
         StreamFacade::SafeClose();
      }

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Reads/decodes from the stream into the specified buffer.</summary>
      /// <param name="buffer">The destination buffer</param>
      /// <param name="length">The length of the buffer</param>
      /// <returns>Number of bytes read</returns>
      DWORD  DataStream::Read(BYTE* buffer, DWORD length)
      {
         // Read + Decode buffer
         DWORD bytesRead = StreamFacade::Read(buffer, length);
         Encode(buffer, bytesRead);
         return bytesRead;
      }

      /// <summary>Writes/encodes the specified buffer to the stream</summary>
      /// <param name="buffer">The buffer.</param>
      /// <param name="length">The length of the buffer.</param>
      /// <returns>Number of bytes written</returns>
      DWORD  DataStream::Write(const BYTE* buffer, DWORD length)
      {
         // Copy buffer so we can encode it
         ByteArrayPtr copy(new BYTE[length]);
         memcpy(copy.get(), buffer, length);

         // Encode + Write
         Encode(copy.get(), length);
         return StreamFacade::Write(copy.get(), length);
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

      /// <summary>Encodes a byte buffer</summary>
      /// <param name="buffer">Buffer to encode</param>
      /// <param name="length">Length of buffer</param>
      void  DataStream::Encode(byte* buffer, DWORD length)
      {
         // Encode buffer
         for (DWORD i = 0; i < length; i++)
            buffer[i] ^= DATAFILE_ENCRYPT_KEY;
      }

      // -------------------------------- NESTED CLASSES ------------------------------
   }
}