#include "stdafx.h"
#include "FileStream.h"

namespace Library
{
   namespace IO
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Opens a file stream</summary>
      /// <param name="path">The full path.</param>
      /// <param name="mode">The creation mode.</param>
      /// <param name="access">The file access.</param>
      /// <param name="share">The sharing permitted.</param>
      /// <exception cref="Library.FileNotFoundException">File not found</exception>
      /// <exception cref="Library.Win32Exception">Unable to create/open file</exception>
      FileStream::FileStream(Path path, FileMode mode, FileAccess access, FileShare share) : FullPath(path), Mode(mode), Access(access), Share(share) 
      {
         // Open file
         Handle = CreateFile((WCHAR*)path, (DWORD)access, (DWORD)share, NULL, (DWORD)mode, FILE_ATTRIBUTE_NORMAL, NULL);

         if (Handle == INVALID_HANDLE_VALUE) {
            if (GetLastError() == ERROR_FILE_NOT_FOUND)
               throw FileNotFoundException(HERE, path);
            else
               throw Win32Exception(HERE);
         }
      }

      //FileStream::FileStream(const FileStream& s) : FullPath(s.FullPath), Mode(s.Mode), Access(s.Access), Share(s.Share) 
      //{
      //   // Duplicate handle
      //   if (!DuplicateHandle(GetCurrentProcess(), s.Handle, GetCurrentProcess(), &Handle, NULL, TRUE, DUPLICATE_SAME_ACCESS))
      //      throw Win32Exception(HERE);
      //}

      /// <summary>Closes the stream</summary>
      FileStream::~FileStream() 
      {
         SafeClose();
      }

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Closes the stream.</summary>
      /// <exception cref="Library.Win32Exception">Unable to close stream</exception>
      void  FileStream::Close()
      {
         if (Handle != INVALID_HANDLE_VALUE) 
         {
            if (!CloseHandle(Handle))
               throw Win32Exception(HERE);

            Handle = INVALID_HANDLE_VALUE;
         }
      }

      /// <summary>Flushes any data to the stream.</summary>
      /// <exception cref="Library.Win32Exception">Unable to flush stream</exception>
      void  FileStream::Flush()
      {
         if (!FlushFileBuffers(Handle))
            throw Win32Exception(HERE);
      }

      
      /// <summary>Gets the file length</summary>
      /// <returns></returns>
      /// <exception cref="Library.Win32Exception">Unable to get length</exception>
      DWORD  FileStream::GetLength()
      { 
         LARGE_INTEGER  size = {0LL};

         // Lookup file size
         if (!GetFileSizeEx(Handle, &size))
            throw Win32Exception(HERE);

         return (DWORD)size.QuadPart;
      }
      
      /// <summary>Gets the current seek position.</summary>
      /// <returns></returns>
      /// <exception cref="Library.Win32Exception">Unable to get position</exception>
      DWORD  FileStream::GetPosition() const
      {
         DWORD  position = 0;
         
         // Get position from null move
         if (SetFilePointer(Handle, 0, (LONG*)&position, (DWORD)SeekOrigin::Current) == INVALID_SET_FILE_POINTER)
            throw Win32Exception(HERE);

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
      /// <exception cref="Library.Win32Exception">Unable to seek</exception>
      void  FileStream::Seek(LONG  offset, SeekOrigin  mode)
      {
         // Set position 
         if (SetFilePointer(Handle, offset, NULL, (DWORD)mode) == INVALID_SET_FILE_POINTER)
            throw Win32Exception(HERE);
      }

      /// <summary>Sets the length of the file</summary>
      /// <param name="length">The length.</param>
      /// <exception cref="Library.Win32Exception">Unable to set length</exception>
      void  FileStream::SetLength(DWORD  length)
      {
         DWORD  position = GetPosition();

         // Seek to desired position and set EOF
         Seek(length, SeekOrigin::Begin);
         if (!SetEndOfFile(Handle))
            throw Win32Exception(HERE);

         // Restore original position
         Seek(position, SeekOrigin::Begin);
      }

      /// <summary>Reads from the stream into the specified buffer.</summary>
      /// <param name="buffer">The destination buffer</param>
      /// <param name="length">The length of the buffer</param>
      /// <returns>Number of bytes read</returns>
      /// <exception cref="Library.InvalidOperationException">Stream is not readable</exception>
      /// <exception cref="Library.Win32Exception">An I/O error occurred</exception>
      DWORD  FileStream::Read(BYTE* buffer, DWORD length)
      {
         REQUIRED(buffer);
         
         // Check access
         if (!CanRead())
            throw InvalidOperationException(HERE, ERR_NO_READ_ACCESS);

         // Read bytes
         DWORD count = 0;
         if (!ReadFile(Handle, buffer, length, &count, NULL))
            throw Win32Exception(HERE);

         // Return bytes read
         return count;
      }

      /// <summary>Writes the specified buffer to the stream</summary>
      /// <param name="buffer">The buffer.</param>
      /// <param name="length">The length of the buffer.</param>
      /// <returns>Number of bytes written</returns>
      /// <exception cref="Library.InvalidOperationException">Stream is not writeable</exception>
      /// <exception cref="Library.Win32Exception">An I/O error occurred</exception>
      DWORD  FileStream::Write(const BYTE* buffer, DWORD length)
      {
         REQUIRED(buffer);
         
         // Check access
         if (!CanWrite())
            throw InvalidOperationException(HERE, ERR_NO_WRITE_ACCESS);

         // Write bytes
         DWORD count = 0;
         if (!WriteFile(Handle, buffer, length, &count, NULL))
            throw Win32Exception(HERE);

         // Return bytes written
         return count;
      }

      
		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

		// -------------------------------- NESTED CLASSES ------------------------------

   }
}