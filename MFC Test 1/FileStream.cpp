#include "stdafx.h"
#include "FileStream.h"

namespace Library
{
   namespace IO
   {

      FileStream::FileStream(Path path, FileMode mode, FileAccess access, FileShare share) : FullPath(path), Mode(mode), Access(access), Share(share) 
      {
         Handle = CreateFile(path.c_str(), (DWORD)access, (DWORD)share, NULL, (DWORD)mode, FILE_ATTRIBUTE_NORMAL, NULL);

         if (Handle == INVALID_HANDLE_VALUE)
            throw Win32Exception(HERE, GetLastError());
      }

      FileStream::FileStream(const FileStream& s) : FullPath(s.FullPath), Mode(s.Mode), Access(s.Access), Share(s.Share) 
      {
         // Duplicate handle
         if (!DuplicateHandle(GetCurrentProcess(), s.Handle, GetCurrentProcess(), &Handle, NULL, TRUE, DUPLICATE_SAME_ACCESS))
            throw Win32Exception(HERE, GetLastError());
      }

      FileStream::~FileStream() 
      {
         Close();
      }

      DWORD  FileStream::GetLength() const
      { 
         LARGE_INTEGER  size = {0LL};

         // Lookup file size
         if (!GetFileSizeEx(Handle, &size))
            throw Win32Exception(HERE, GetLastError());

         return (DWORD)size.QuadPart;
      }
      
      DWORD  FileStream::GetPosition() const
      {
         DWORD  position = 0;
         
         // Get position from null move
         if (SetFilePointer(Handle, 0, (LONG*)&position, (DWORD)SeekOrigin::Current) == INVALID_SET_FILE_POINTER)
            throw Win32Exception(HERE, GetLastError());

         return position;
      }

      void  FileStream::Close()
      {
         if (Handle != NULL) 
         {
            if (!CloseHandle(Handle))
               throw Win32Exception(HERE, GetLastError());

            Handle = NULL;
         }
      }

      void  FileStream::Flush()
      {
         if (!FlushFileBuffers(Handle))
            throw Win32Exception(HERE, GetLastError());
      }

      void  FileStream::Seek(DWORD  offset, SeekOrigin  mode)
      {
         // Set position 
         if (!SetFilePointer(Handle, offset, NULL, (DWORD)mode))
            throw Win32Exception(HERE, GetLastError());
      }

      void  FileStream::SetLength(DWORD  length)
      {
         DWORD  position = GetPosition();

         // Seek to desired position and set EOF
         Seek(length, SeekOrigin::Begin);
         if (!SetEndOfFile(Handle))
            throw Win32Exception(HERE, GetLastError());

         // Restore original position
         Seek(position, SeekOrigin::Begin);
      }


      DWORD  FileStream::Read(BYTE* buffer, DWORD length)
      {
         REQUIRED(buffer);
         
         // Check access
         if (!CanRead())
            throw InvalidOperationException(HERE, ERR_NO_READ_ACCESS);

         // Read bytes
         DWORD count = 0;
         if (!ReadFile(Handle, buffer, length, &count, NULL))
            throw Win32Exception(HERE, GetLastError());

         // Return bytes read
         return count;
      }


      DWORD  FileStream::Write(const BYTE* buffer, DWORD length)
      {
         REQUIRED(buffer);
         
         // Check access
         if (!CanWrite())
            throw InvalidOperationException(HERE, ERR_NO_WRITE_ACCESS);

         // Write bytes
         DWORD count = 0;
         if (!WriteFile(Handle, buffer, length, &count, NULL))
            throw Win32Exception(HERE, GetLastError());

         // Return bytes written
         return count;
      }


   }
}