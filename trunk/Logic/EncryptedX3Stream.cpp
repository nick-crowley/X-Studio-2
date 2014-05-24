#include "stdafx.h"
#include "EncryptedX3Stream.h"

namespace Logic
{
   namespace IO
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Creates an encrypted file stream using another stream as input</summary>
      /// <param name="src">The input stream.</param>
      /// <exception cref="Logic::ArgumentNullException">Stream is null</exception>
      EncryptedX3Stream::EncryptedX3Stream(StreamPtr  src) : StreamFacade(src), DECRYPT_KEY(0)
      {
      }

      /// <summary>Creates an encrypted file stream using a file as input</summary>
      /// <param name="path">The full path.</param>
      /// <param name="mode">The creation mode.</param>
      /// <param name="access">The file access.</param>
      /// <param name="share">The sharing permitted.</param>
      /// <exception cref="Logic::FileNotFoundException">File not found</exception>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      EncryptedX3Stream::EncryptedX3Stream(Path path, FileMode mode, FileAccess access, FileShare share)
         : StreamFacade( StreamPtr(new FileStream(path, mode, access, share)) ), DECRYPT_KEY(0)
      {
      }

      /// <summary>Closes the stream without throwing</summary>
      EncryptedX3Stream::~EncryptedX3Stream()
      {
         StreamFacade::SafeClose();
      }

      // ------------------------------- STATIC METHODS -------------------------------
      
      /// <summary>Determines whether a stream uses X3:TC file encryption.</summary>
      /// <param name="s">stream.</param>
      /// <returns></returns>
      bool  EncryptedX3Stream::IsEncrypted(StreamPtr s)
      {
         // Verify length
         if (s->GetLength() < 3)
            return false;
         
         // Prepare
         DWORD origin = s->GetPosition();
         WORD  header, key;

         // Generate key: XOR first byte with 0xC8
         s->Seek(0, SeekOrigin::Begin);
         s->Read(reinterpret_cast<byte*>(&key), 1);
         key ^= DECRYPT_SEED;

         // Generate WORD from first byte
         reinterpret_cast<byte*>(&key)[1] = reinterpret_cast<byte*>(&key)[0];

         // Read encrypted 2-byte header. reset position
         s->Read(reinterpret_cast<BYTE*>(&header), 2);
         s->Seek(origin, SeekOrigin::Begin);

         // Check for encrypted GZip header 
         return (header ^ key) == 0x8b1f;
      }

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Reads/decodes from the stream into the specified buffer.</summary>
      /// <param name="buffer">The destination buffer</param>
      /// <param name="length">The length of the buffer</param>
      /// <returns>Number of bytes read</returns>
      /// <exception cref="Logic::ArgumentNullException">Buffer is null</exception>
      /// <exception cref="Logic::NotSupportedException">Stream is not readable</exception>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      DWORD  EncryptedX3Stream::Read(BYTE* buffer, DWORD length)
      {
         REQUIRED(buffer);

         // Init: Generate key from 1st byte
         if (DECRYPT_KEY == 0)
         {
            // Read first byte
            DWORD origin = StreamFacade::GetPosition();
            StreamFacade::Seek(0, SeekOrigin::Begin);
            StreamFacade::Read(&DECRYPT_KEY, 1);
            StreamFacade::Seek(origin, SeekOrigin::Begin);

            // Generate key
            DECRYPT_KEY ^= DECRYPT_SEED;
         }

         // Skip reading first byte
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
      DWORD  EncryptedX3Stream::Write(const BYTE* buffer, DWORD length)
      {
         REQUIRED(buffer);
         DWORD keylen = 0;

         // Init: Generate key and write to first byte
         if (DECRYPT_KEY == 0 && length > 0)
         {
            // Verify position
            if (StreamFacade::GetPosition() != 0)
               throw IOException(HERE, L"First write operation must be positioned at start of stream");

            // Generate/write key
            DECRYPT_KEY = buffer[0] ^ DECRYPT_SEED;
            StreamFacade::Write(&DECRYPT_KEY, 1);
            keylen = 1;
         }

         // Copy buffer so we can encode it
         ByteArrayPtr copy(new BYTE[length]);
         memcpy(copy.get(), buffer, length);

         // Encode + Write
         Encode(copy.get(), length);
         return StreamFacade::Write(copy.get(), length) + keylen;
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

      /// <summary>Encodes a byte array</summary>
      /// <param name="buffer">Buffer to encode</param>
      /// <param name="length">Length of buffer</param>
      void  EncryptedX3Stream::Encode(byte* buffer, DWORD length)
      {
         // Encode buffer
         for (DWORD i = 0; i < length; i++)
            buffer[i] ^= DECRYPT_KEY;
      }

   }
}