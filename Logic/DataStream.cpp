#include "stdafx.h"
#include "DataStream.h"
#include "FileStream.h"
#include "XCatalog.h"
#include <cassert>

//using namespace Logic::FileSystem;

namespace Logic
{
   namespace IO
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Creates a data stream from a file descriptor</summary>
      /// <param name="f">The file to open</param>
      /// <exception cref="Logic::FileNotFoundException">Data file not found</exception>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      DataStream::DataStream(const XFileInfo&  f)
         : StreamDecorator( StreamPtr(new FileStream(f.DataFile, FileMode::OpenExisting, FileAccess::Read)) ),
           File(f)
      {
         // Seek to logical beginning of file
         StreamDecorator::Seek(File.Offset, SeekOrigin::Begin);
         assert(StreamDecorator::GetPosition() == File.Offset);
      }

      /// <summary>Closes the stream without throwing</summary>
      DataStream::~DataStream()
      {
         StreamDecorator::SafeClose();
      }

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Gets the length of the logical file</summary>
      /// <returns></returns>
      DWORD DataStream::GetLength()
      {
         return File.Length;
      }

      /// <summary>Gets the current position within the logical file</summary>
      /// <returns></returns>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      DWORD DataStream::GetPosition() const
      {
         return StreamDecorator::GetPosition() - File.Offset;
      }

      /// <summary>Reads/decodes from the stream into the specified buffer.</summary>
      /// <param name="buffer">The destination buffer</param>
      /// <param name="length">The length of the buffer</param>
      /// <returns>Number of bytes read</returns>
      /// <exception cref="Logic::ArgumentNullException">Buffer is null</exception>
      /// <exception cref="Logic::NotSupportedException">Stream is not readable</exception>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      DWORD  DataStream::Read(BYTE* buffer, DWORD length)
      {
         REQUIRED(buffer);

         // Ensure we don't exceed logical EOF
         if (GetPosition() + length > GetLength())
            length = GetLength() - GetPosition();

         // Read + Decode buffer
         DWORD bytesRead = StreamDecorator::Read(buffer, length);
         Encode(buffer, bytesRead);
         return bytesRead;
      }

      /// <summary>Not supported</summary>
      /// <param name="offset">The offset.</param>
      /// <param name="mode">The mode.</param>
      /// <exception cref="Logic::NotSupportedException">Stream is not seekable</exception>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      void  DataStream::Seek(LONG  offset, SeekOrigin  mode)
      {
         switch (mode)
         {
         case SeekOrigin::Begin:    
            StreamDecorator::Seek((LONG)File.Offset + offset, SeekOrigin::Begin);      
            break;

         case SeekOrigin::Current:  
            StreamDecorator::Seek(offset, mode);                                       
            break;

         case SeekOrigin::End:      
            StreamDecorator::Seek((LONG)File.Offset + (LONG)File.Length + offset, SeekOrigin::Begin);      
            break;
         }
      }

      /// <summary>Writes/encodes the specified buffer to the stream</summary>
      /// <param name="buffer">The buffer.</param>
      /// <param name="length">The length of the buffer.</param>
      /// <returns>Number of bytes written</returns>
      /// <exception cref="Logic::NotImplementedException">Always</exception>
      DWORD  DataStream::Write(const BYTE* buffer, DWORD length)
      {
         throw NotImplementedException(HERE, L"writing .dat files");

         //// Copy buffer so we can encode it
         //ByteArrayPtr copy(new BYTE[length]);
         //memcpy(copy.get(), buffer, length);

         //// Encode + Write
         //Encode(copy.get(), length);
         //return StreamDecorator::Write(copy.get(), length);
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

      /// <summary>Encodes a byte buffer</summary>
      /// <param name="buffer">Buffer to encode</param>
      /// <param name="length">Length of buffer</param>
      /// <exception cref="Logic::ArgumentNullException">Buffer is null</exception>
      void  DataStream::Encode(byte* buffer, DWORD length)
      {
         REQUIRED(buffer);

         // Encode buffer
         for (DWORD i = 0; i < length; i++)
            buffer[i] ^= DATAFILE_ENCRYPT_KEY;
      }

      // -------------------------------- NESTED CLASSES ------------------------------
   }
}