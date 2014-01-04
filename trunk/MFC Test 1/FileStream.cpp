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
            throw IOException(HERE, GetLastError());
      }
      FileStream::~FileStream() 
      {
         Close();
      }

      __int64  FileStream::GetLength() { return 0; }
      __int64  FileStream::GetPosition() { return 0; }

      void  FileStream::Close()
      {
         if (Handle != NULL) {
            CloseHandle(Handle);
            Handle = NULL;
         }
      }
      void  FileStream::Flush()
      {
         FlushFileBuffers(Handle);
      }
      void  FileStream::Seek(__int64  offset, SeekOrigin  mode)
      {
      }
      void  FileStream::SetLength(__int64  length)
      {
      }

      void  FileStream::Read(BYTE* buffer, __int64 length)
      {
      }
      BYTE  FileStream::ReadByte()
      {
         return 0;
      }

      void  FileStream::Write(BYTE* buffer, __int64 length)
      {
      }
      void  FileStream::WriteByte(BYTE byte) 
      {
      }

   }
}