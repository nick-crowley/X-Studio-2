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
      FileStream::FileStream(Path path, FileMode mode, FileAccess access, FileShare share) : FullPath(path), Mode(mode), Access(access), Share(share) 
      {
         // Ensure exists
         if (!path.Exists())
            throw FileNotFoundException(HERE, path);

         // Open file
         Handle = CreateFile(path.c_str(), (DWORD)access, (DWORD)share, NULL, (DWORD)mode, FILE_ATTRIBUTE_NORMAL, NULL);

         if (Handle == INVALID_HANDLE_VALUE)
            throw Win32Exception(HERE);
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
      void  FileStream::Flush()
      {
         if (!FlushFileBuffers(Handle))
            throw Win32Exception(HERE);
      }

      
      /// <summary>Gets the file length</summary>
      /// <returns></returns>
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
      void  FileStream::Seek(LONG  offset, SeekOrigin  mode)
      {
         // Set position 
         if (SetFilePointer(Handle, offset, NULL, (DWORD)mode) == INVALID_SET_FILE_POINTER)
            throw Win32Exception(HERE);
      }

      /// <summary>Sets the length of the file</summary>
      /// <param name="length">The length.</param>
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
      DWORD  FileStream::Read(BYTE* buffer, DWORD length)
      {
         REQUIRED(buffer);
         
         // Check access
         if (!CanRead())
            throw InvalidOperationException(HERE, ERR_NO_READ_ACCESS);

         DWORD debugPos = GetPosition();

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