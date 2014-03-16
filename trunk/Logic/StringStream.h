#pragma once

#include "Stream.h"

namespace Logic
{
   namespace IO
   {
      /// <summary>Provides stream access to/from a string</summary>
      class LogicExport StringStream : public Stream
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Creates the stream from string</summary>
         /// <param name="str">The source/destination string.</param>
         StringStream(string& str);

         /// <summary>Closes the stream without throwing</summary>
         virtual ~StringStream();

         // Prevent copying/moving
         NO_MOVE(StringStream);
         NO_COPY(StringStream);

			// --------------------- PROPERTIES ------------------------
			
			// ---------------------- ACCESSORS ------------------------
      public:
         /// <summary>Queries whether the stream is readable</summary>
         bool  CanRead() const override            { return true;  }

         /// <summary>Queries whether the stream is seekable</summary>
         bool  CanSeek() const override            { return true;  }

         /// <summary>Queries whether the stream is writeable</summary>
         bool  CanWrite() const override           { return true; }

         /// <summary>Gets the file length</summary>
         /// <returns></returns>
         DWORD GetLength() override                { return String.length(); }

         /// <summary>Gets the current seek position.</summary>
         /// <returns></returns>
         DWORD GetPosition() const override        { return Position; }

         // ----------------------- MUTATORS ------------------------
      public:
         /// <summary>Closes the stream.</summary>
         void  Close() override
         { 
         }

         /// <summary>Flushes any data to the stream.</summary>
         void  Flush() override
         {
         }

         /// <summary>Closes the stream without throwing.</summary>
         void  SafeClose() override
         {
         }

         /// <summary>Seeks to the specified offset.</summary>
         /// <param name="offset">The offset in bytes</param>
         /// <param name="mode">The seek origin</param>
         /// <exception cref="Logic::IOException">Invalid offset</exception>
         void  Seek(LONG  offset, SeekOrigin  mode) override;

         /// <summary>Sets the length of the stream</summary>
         /// <param name="length">The length in bytes</param>
         /// <exception cref="Logic::IOException">Length exceeds max string size</exception>
         void  SetLength(DWORD  length) override;

         /// <summary>Reads from the stream into the specified buffer.</summary>
         /// <param name="buffer">The destination buffer</param>
         /// <param name="length">The length of the buffer</param>
         /// <returns>Number of bytes read</returns>
         /// <exception cref="Logic::ArgumentNullException">Buffer is null</exception>
         DWORD Read(BYTE* buffer, DWORD length) override;

         /// <summary>Writes the specified buffer to the stream</summary>
         /// <param name="buffer">The buffer.</param>
         /// <param name="length">The length of the buffer.</param>
         /// <returns>Number of bytes written</returns>
         /// <exception cref="Logic::ArgumentNullException">Buffer is null</exception>
         DWORD Write(const BYTE* buffer, DWORD length) override;

         // -------------------- REPRESENTATION ---------------------
      private:
         string& String;
         DWORD   Position;
      };

   }
}

using namespace Logic::IO;
