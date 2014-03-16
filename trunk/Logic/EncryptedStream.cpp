#include "stdafx.h"
#include "EncryptedStream.h"

namespace Logic
{
   namespace IO
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Creates an encrypted file stream using another stream as input</summary>
      /// <param name="src">The input stream.</param>
      /// <exception cref="Logic::ArgumentNullException">Stream is null</exception>
      EncryptedStream::EncryptedStream(StreamPtr  src) : StreamFacade(src)
      {
      }

      /// <summary>Creates an encrypted file stream using a file as input</summary>
      /// <param name="path">The full path.</param>
      /// <param name="mode">The creation mode.</param>
      /// <param name="access">The file access.</param>
      /// <param name="share">The sharing permitted.</param>
      /// <exception cref="Logic::FileNotFoundException">File not found</exception>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      EncryptedStream::EncryptedStream(Path path, FileMode mode, FileAccess access, FileShare share)
         : StreamFacade( StreamPtr(new FileStream(path, mode, access, share)) )
      {
      }

      /// <summary>Closes the stream without throwing</summary>
      EncryptedStream::~EncryptedStream()
      {
         StreamFacade::SafeClose();
      }

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Reads/decodes from the stream into the specified buffer.</summary>
      /// <param name="buffer">The destination buffer</param>
      /// <param name="length">The length of the buffer</param>
      /// <returns>Number of bytes read</returns>
      /// <exception cref="Logic::ArgumentNullException">Buffer is null</exception>
      /// <exception cref="Logic::NotSupportedException">Stream is not readable</exception>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      DWORD  EncryptedStream::Read(BYTE* buffer, DWORD length)
      {
         REQUIRED(buffer);

         // Skip first byte
         if (StreamFacade::GetPosition() == 0)
            StreamFacade::Seek(1, SeekOrigin::Begin);

         // Read+decode buffer
         DWORD bytesRead = StreamFacade::Read(buffer, length);
         Encode(buffer, bytesRead);

         // Return length
         return bytesRead;
      }

      /// <summary>Writes/encodes the specified buffer to the stream</summary>
      /// <param name="buffer">The buffer.</param>
      /// <param name="length">The length of the buffer.</param>
      /// <returns>Number of bytes written</returns>
      /// <exception cref="Logic::ArgumentNullException">Buffer is null</exception>
      /// <exception cref="Logic::NotSupportedException">Stream is not writeable</exception>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      DWORD  EncryptedStream::Write(const BYTE* buffer, DWORD length)
      {
         REQUIRED(buffer);

         // Copy buffer so we can encode it
         ByteArrayPtr copy(new BYTE[length]);
         memcpy(copy.get(), buffer, length);

         // Encode + Write
         Encode(copy.get(), length);
         return StreamFacade::Write(copy.get(), length);
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

      /// <summary>Encodes a byte array</summary>
      /// <param name="buffer">Buffer to encode</param>
      /// <param name="length">Length of buffer</param>
      void  EncryptedStream::Encode(byte* buffer, DWORD length)
      {
         // Encode buffer
         for (DWORD i = 0; i < length; i++)
            buffer[i] ^= DECRYPT_KEY;
      }

   }
}