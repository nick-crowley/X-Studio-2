#include "stdafx.h"
#include "CatalogStream.h"

namespace Logic
{
   namespace IO
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Creates a catalog stream using another stream as input</summary>
      /// <param name="src">The input stream.</param>
      /// <exception cref="Logic::ArgumentNullException">Stream is null</exception>
      CatalogStream::CatalogStream(StreamPtr  src) : StreamDecorator(src)
      {
      }

      /// <summary>Creates a catalog stream using a file as input</summary>
      /// <param name="path">The full path.</param>
      /// <param name="mode">The creation mode.</param>
      /// <param name="access">The file access.</param>
      /// <param name="share">The sharing permitted.</param>
      /// <exception cref="Logic::FileNotFoundException">File not found</exception>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      CatalogStream::CatalogStream(Path path, FileMode mode, FileAccess access, FileShare share)
         : StreamDecorator( StreamPtr(new FileStream(path, mode, access, share)) )
      {
      }

      /// <summary>Closes the stream without throwing</summary>
      CatalogStream::~CatalogStream()
      {
         StreamDecorator::SafeClose();
      }

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Reads/decodes from the stream into the specified buffer.</summary>
      /// <param name="buffer">The destination buffer</param>
      /// <param name="length">The length of the buffer</param>
      /// <returns>Number of bytes read</returns>
      /// <exception cref="Logic::ArgumentNullException">Buffer is null</exception>
      /// <exception cref="Logic::NotSupportedException">Stream is not readable</exception>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      DWORD  CatalogStream::Read(BYTE* buffer, DWORD length)
      {
         REQUIRED(buffer);

         // Preserve origin, fill buffer
         DWORD startPos = GetPosition(),
               bytesRead = StreamDecorator::Read(buffer, length);

         // Encode buffer + return
         Encode(buffer, bytesRead, startPos);
         return bytesRead;
      }

      /// <summary>Writes/encodes the specified buffer to the stream</summary>
      /// <param name="buffer">The buffer.</param>
      /// <param name="length">The length of the buffer.</param>
      /// <returns>Number of bytes written</returns>
      /// <exception cref="Logic::ArgumentNullException">Buffer is null</exception>
      /// <exception cref="Logic::NotSupportedException">Stream is not writeable</exception>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      DWORD  CatalogStream::Write(const BYTE* buffer, DWORD length)
      {
         REQUIRED(buffer);

         // Copy buffer so we can encode it
         ByteArrayPtr copy(new BYTE[length]);
         memcpy(copy.get(), buffer, length);

         // Encode + Write
         Encode(copy.get(), length, GetPosition());
         return StreamDecorator::Write(copy.get(), length);
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

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