#include "stdafx.h"
#include "FileStream.h"

namespace Logic
{
   namespace IO
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Opens a file stream</summary>
      /// <param name="path">The full path.</param>
      /// <param name="mode">The creation mode.</param>
      /// <param name="access">The file access.</param>
      /// <param name="share">The sharing permitted.</param>
      /// <exception cref="Logic::FileNotFoundException">File not found</exception>
      /// <exception cref="Logic::IOException">Unable to create/open file</exception>
      FileStream::FileStream(Path path, FileMode mode, FileAccess access, FileShare share) : FullPath(path), Mode(mode), Access(access), Share(share) 
      {
         // Open file
         Handle = CreateFile((WCHAR*)path, (DWORD)access, (DWORD)share, NULL, (DWORD)mode, FILE_ATTRIBUTE_NORMAL, NULL);

         if (Handle == INVALID_HANDLE_VALUE) {
            if (GetLastError() == ERROR_FILE_NOT_FOUND)
               throw FileNotFoundException(HERE, path);
            else
               throw IOException(HERE, SysErrorString());
         }
      }

      //FileStream::FileStream(const FileStream& s) : FullPath(s.FullPath), Mode(s.Mode), Access(s.Access), Share(s.Share) 
      //{
      //   // Duplicate handle
      //   if (!DuplicateHandle(GetCurrentProcess(), s.Handle, GetCurrentProcess(), &Handle, NULL, TRUE, DUPLICATE_SAME_ACCESS))
      //      throw IOException(HERE, SysErrorString());
      //}

      /// <summary>Closes the stream without throwing</summary>
      FileStream::~FileStream() 
      {
         SafeClose();
      }

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Closes the stream.</summary>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      void  FileStream::Close()
      {
         if (Handle != INVALID_HANDLE_VALUE) 
         {
            if (!CloseHandle(Handle))
               throw IOException(HERE, SysErrorString());

            Handle = INVALID_HANDLE_VALUE;
         }
      }

      /// <summary>Flushes any data to the stream.</summary>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      void  FileStream::Flush()
      {
         if (!FlushFileBuffers(Handle))
            throw IOException(HERE, SysErrorString());
      }

      
      /// <summary>Gets the file length</summary>
      /// <returns></returns>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      DWORD  FileStream::GetLength()
      { 
         LARGE_INTEGER  size = {0LL};

         // Lookup file size
         if (!GetFileSizeEx(Handle, &size))
            throw IOException(HERE, SysErrorString());

         return (DWORD)size.QuadPart;
      }
      
      /// <summary>Gets the current seek position.</summary>
      /// <returns></returns>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      DWORD  FileStream::GetPosition() const
      {
         DWORD  position = 0;
         
         // Get position from null move
         if ((position=SetFilePointer(Handle, 0, NULL, (DWORD)SeekOrigin::Current)) == INVALID_SET_FILE_POINTER)
            throw IOException(HERE, SysErrorString());

         return position;
      }

      /// <summary>Closes the stream without throwing.</summary>
      void  FileStream::SafeClose()
      {
         if (Handle != INVALID_HANDLE_VALUE) 
         {
            CloseHandle(Handle);
            Handle = INVALID_HANDLE_VALUE;
         }
      }

      /// <summary>Seeks to the specified offset.</summary>
      /// <param name="offset">The offset.</param>
      /// <param name="mode">The seek origin</param>
      /// <exception cref="Logic::NotSupportedException">Stream is not seekable</exception>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      void  FileStream::Seek(LONG  offset, SeekOrigin  mode)
      {
         // Check access
         if (!CanSeek())
            throw NotSupportedException(HERE, GuiString(ERR_NO_SEEK_ACCESS));

         // Set position 
         if (SetFilePointer(Handle, offset, NULL, (DWORD)mode) == INVALID_SET_FILE_POINTER)
            throw IOException(HERE, SysErrorString());
      }

      /// <summary>Sets the length of the file</summary>
      /// <param name="length">The length.</param>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      void  FileStream::SetLength(DWORD  length)
      {
         DWORD  position = GetPosition();

         // Seek to desired position and set EOF
         Seek(length, SeekOrigin::Begin);
         if (!SetEndOfFile(Handle))
            throw IOException(HERE, SysErrorString());

         // Restore original position
         Seek(position, SeekOrigin::Begin);
      }

      /// <summary>Reads from the stream into the specified buffer.</summary>
      /// <param name="buffer">The destination buffer</param>
      /// <param name="length">The length of the buffer</param>
      /// <returns>Number of bytes read</returns>
      /// <exception cref="Logic::ArgumentNullException">Buffer is null</exception>
      /// <exception cref="Logic::NotSupportedException">Stream is not readable</exception>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      DWORD  FileStream::Read(BYTE* buffer, DWORD length)
      {
         REQUIRED(buffer);
         
         // Check access
         if (!CanRead())
            throw NotSupportedException(HERE, GuiString(ERR_NO_READ_ACCESS));

         // Read bytes
         DWORD count = 0;
         if (!ReadFile(Handle, buffer, length, &count, NULL))
            throw IOException(HERE, SysErrorString());

         // Return bytes read
         return count;
      }

      /// <summary>Writes the specified buffer to the stream</summary>
      /// <param name="buffer">The buffer.</param>
      /// <param name="length">The length of the buffer.</param>
      /// <returns>Number of bytes written</returns>
      /// <exception cref="Logic::ArgumentNullException">Buffer is null</exception>
      /// <exception cref="Logic::NotSupportedException">Stream is not writeable</exception>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      DWORD  FileStream::Write(const BYTE* buffer, DWORD length)
      {
         REQUIRED(buffer);
         
         // Check access
         if (!CanWrite())
            throw NotSupportedException(HERE, GuiString(ERR_NO_WRITE_ACCESS));

         // Write bytes
         DWORD count = 0;
         if (!WriteFile(Handle, buffer, length, &count, NULL))
            throw IOException(HERE, SysErrorString());

         // Return bytes written
         return count;
      }

      
		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

		// -------------------------------- NESTED CLASSES ------------------------------

   }
}