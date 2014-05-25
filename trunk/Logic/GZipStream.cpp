#include "stdafx.h"
#include "GZipStream.h"

namespace Logic
{
   namespace IO
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Creates a GZip stream using another stream as input</summary>
      /// <param name="src">The input stream</param>
      /// <param name="op">Whether to compress or decompress</param>
      /// <exception cref="Logic::ArgumentException">Stream is not readable</exception>
      /// <exception cref="Logic::ArgumentNullException">Stream is null</exception>
      /// <exception cref="Logic::GZipException">Unable to inititalise stream</exception>
      GZipStream::GZipStream(StreamPtr  src, Operation  op) : StreamDecorator(src), Mode(op)
      {
         // Clear structs
         ZeroMemory(&ZStream, sizeof(ZStream));
         ZeroMemory(&ZHeader, sizeof(ZHeader));

         if (Mode == Operation::Decompression)
         {
            // Ensure stream has read/write access
            if (!src->CanRead())
               throw ArgumentException(HERE, L"src", GuiString(ERR_NO_READ_ACCESS));

            // Init stream + extract header
            if (inflateInit2(&ZStream, WINDOW_SIZE+DETECT_HEADER) != Z_OK) 
               throw GZipException(HERE, ZStream.msg);

            // Allocate + set input buffer
            Buffer.reset(new byte[src->GetLength()]);
            ZStream.next_in = Buffer.get();
         }
         else
         {
            // Ensure stream has write access
            if (!src->CanWrite())
               throw ArgumentException(HERE, L"src", GuiString(ERR_NO_WRITE_ACCESS));

            // Init stream
            if (deflateInit2(&ZStream, Z_BEST_COMPRESSION, Z_DEFLATED, WINDOW_SIZE+DETECT_HEADER, 9, Z_DEFAULT_STRATEGY) != Z_OK)
               throw GZipException(HERE, ZStream.msg);

            // Allocate + set output buffer
            Buffer.reset(new byte[COMPRESS_BUFFER]);
            ZStream.next_out = Buffer.get();
            ZStream.avail_out = COMPRESS_BUFFER;
         }
      }

      /// <summary>Closes the stream without throwing</summary>
      GZipStream::~GZipStream()
      {
         SafeClose();
      }

      // ------------------------------- PUBLIC METHODS -------------------------------
      
      /// <summary>Stream is not seekable.</summary>
      /// <returns></returns>
      bool  GZipStream::CanSeek() const
      { 
         return false; 
      }

      /// <summary>Closes the stream.</summary>
      /// <exception cref="Logic::GZipException">Unable to close stream</exception>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      void  GZipStream::Close()
      {
         if (!IsClosed())
         {
            try
            {
               // Decompression
               if (Mode == Operation::Decompression && inflateEnd(&ZStream) != Z_OK)
                  throw GZipException(HERE, ZStream.msg);
            
               // Compression: Write internal compression buffer to disc
               else if (Mode == Operation::Compression)
               {
                  ZStream.avail_in = 0;      // No input
                  ZStream.next_in = Z_NULL;

                  // Flush remaining data into the output buffer
                  if (deflate(&ZStream, Z_FINISH) != Z_STREAM_END)
                     throw GZipException(HERE, ZStream.msg);
                  else
                  {
                     // Success: Write GZIP to underlying stream 
                     for (DWORD out = 0; out < ZStream.total_out; )
                        out += StreamDecorator::Write(&Buffer.get()[out], ZStream.total_out - out);

                     // Cleanup zstream
                     if (deflateEnd(&ZStream) != Z_OK)
                        throw GZipException(HERE, ZStream.msg);
                  }
               }
            }
            catch (ExceptionBase&) {
               // Close before rethrowing
               StreamDecorator::Close();
               throw;
            }
            
            // Close stream
            StreamDecorator::Close();
         }
      }

      /// <summary>Gets the uncompressed stream length.</summary>
      /// <returns></returns>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      /// <exception cref="Logic::NotSupportedException">Mode is compression</exception>
      DWORD  GZipStream::GetLength()
      {
         DWORD pos = StreamDecorator::GetPosition(),
               size = 0;

         // Compress: Not supported
         if (Mode == Operation::Compression)
            throw NotSupportedException(HERE, L"Cannot get uncompressed length when compressing");

         // Decompress: Extract uncompressed length from last four bytes
         StreamDecorator::Seek(-4, SeekOrigin::End);
         StreamDecorator::Read((byte*)&size, 4);
         StreamDecorator::Seek(pos, SeekOrigin::Begin);

         // BugFix: Ensure file is not corrupted
         if (!size)
            throw GZipException(HERE, "GZip file corrupted - uncompressed file size is zero");

         return size;
      }

      /// <summary>Gets the current position.</summary>
      /// <returns></returns>
      DWORD  GZipStream::GetPosition() const
      {
         return ZStream.total_out;
      }

      /// <summary>Closes the stream without throwing.</summary>
      void  GZipStream::SafeClose()
      {
         if (!IsClosed())
         {
            // Close ZLib stream
            if (Mode == Operation::Decompression)
               inflateEnd(&ZStream);
            else
               deflateEnd(&ZStream);

            // Close underlying stream
            StreamDecorator::SafeClose();
         }
      }

      /// <summary>Not supported</summary>
      /// <param name="offset">The offset.</param>
      /// <param name="mode">The mode.</param>
      /// <exception cref="Logic::NotSupportedException">Always</exception>
      void  GZipStream::Seek(LONG  offset, SeekOrigin  mode)
      {
         throw NotSupportedException(HERE, GuiString(ERR_NO_SEEK_ACCESS));
      }

      /// <summary>Sets the internal filename.</summary>
      /// <param name="name">name.</param>
      /// <exception cref="Logic::InvalidOperationException">Called after first write</exception>
      /// <exception cref="Logic::GZipException">GZip error</exception>
      void  GZipStream::SetFileName(const wstring& name)
      {
         if (ZStream.total_out > 0)
            throw InvalidOperationException(HERE, L"Cannot set filename after writing");

         // Convert to ANSI
         FileName = GuiString::Convert(name, CP_ACP);
         ZHeader.name = (Byte*)FileName.c_str();

         // Set header
         if (deflateSetHeader(&ZStream, &ZHeader) != Z_OK)
            throw GZipException(HERE, ZStream.msg);
      }

      /// <summary>Not supported</summary>
      /// <param name="offset">The offset.</param>
      /// <param name="mode">The mode.</param>
      /// <exception cref="Logic::NotSupportedException">Always</exception>
      void  GZipStream::SetLength(DWORD  length)
      {
         throw NotSupportedException(HERE, L"Resizing not allowed");
      }

      /// <summary>Reads/decompresses from the stream into the specified buffer.</summary>
      /// <param name="buffer">The destination buffer</param>
      /// <param name="length">The length of the buffer</param>
      /// <returns>Number of bytes read</returns>
      /// <exception cref="Logic::ArgumentNullException">Buffer is null</exception>
      /// <exception cref="Logic::NotSupportedException">Input stream is not readable</exception>
      /// <exception cref="Logic::GZipException">Unable to decompress data</exception>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      DWORD  GZipStream::Read(BYTE* output, DWORD length)
      {
         REQUIRED(output);

         // Ensure we can read
         if (!StreamDecorator::CanRead())
            throw NotSupportedException(HERE, GuiString(ERR_NO_READ_ACCESS));

         // Supply output buffer
         ZStream.next_out = output;
         ZStream.avail_out = length;

         // Re-Fill input buffer if necessary
         if (ZStream.avail_in == 0)
            ZStream.avail_in = StreamDecorator::Read(Buffer.get(), StreamDecorator::GetLength());
         
         // Empty: Return zero
         if (length == 0)
            return 0;

         // Decompress
         switch (int res = inflate(&ZStream, Z_SYNC_FLUSH))
         {
         // Success/EOF: Return count decompressed
         case Z_STREAM_END:
            // Ensure all input consumed
            if (ZStream.avail_in > 0)
               throw GZipException(HERE, VString(L"Unable to decompress entire buffer: %d bytes remaining", ZStream.avail_in));

         // Success: Return count decompressed
         case Z_OK:
            return length - ZStream.avail_out;  // avail_out=='output buffer remaining' not 'output bytes available'

         // Error: throw
         default:
            throw GZipException(HERE, ZStream.msg);
         }
      }

      /// <summary>Writes/compresses the specified buffer to the stream</summary>
      /// <param name="buffer">The buffer.</param>
      /// <param name="length">The length of the buffer.</param>
      /// <returns>Number of bytes written</returns>
      /// <exception cref="Logic::NotImplementedException">Always</exception>
      DWORD  GZipStream::Write(const BYTE* input, DWORD length)
      {
         REQUIRED(input);

         // Ensure we can write
         if (!StreamDecorator::CanWrite())
            throw NotSupportedException(HERE, GuiString(ERR_NO_WRITE_ACCESS));

         // Supply input buffer
         ZStream.next_in = const_cast<BYTE*>(input);
         ZStream.avail_in = length;

         // Empty: Return zero
         if (length == 0)
            return 0;

         // Re-Fill output buffer if necessary
         /*if (ZStream.avail_out == 0)
            ZStream.avail_out = StreamDecorator::Read(ZStream.next_in, StreamDecorator::GetLength());*/

         DWORD start = ZStream.avail_out;
         
         // Compress available input into the output buffer
         switch (int res = deflate(&ZStream, Z_NO_FLUSH))
         {
         // Complete: Write to underlying stream + Return count
         /*case Z_STREAM_END:
            DWORD compressed = COMPRESS_BUFFER - ZStream.avail_out;
            for (DWORD out = 0; out < compressed; )
               out += StreamDecorator::Write(&Buffer.get()[out], compressed - out);
            return compressed;*/

         // Incomplete: Return # of bytes compressed
         case Z_OK:
            return ZStream.avail_out - start;

         // Error: throw
         case Z_BUF_ERROR:
         default:
            throw GZipException(HERE, ZStream.msg);
         }
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

      /// <summary>Determines whether the stream is closed.</summary>
      /// <returns></returns>
      bool   GZipStream::IsClosed() const
      {
         return ZStream.zalloc == Z_NULL;
      }

      // -------------------------------- NESTED CLASSES ------------------------------
   }
}
