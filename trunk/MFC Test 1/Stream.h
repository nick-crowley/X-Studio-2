#pragma once
#include "Stream.h"
#include <Windows.h>

namespace Library
{
   namespace IO
   {
      enum class FileMode { Create, Open, Append };
      enum class FileAccess { Read = GENERIC_READ, Write = GENERIC_WRITE, ReadWrite = GENERIC_READ|GENERIC_WRITE };
      enum class FileShare { Read = FILE_SHARE_READ, Write = FILE_SHARE_WRITE, Delete = FILE_SHARE_DELETE, None = 0 };
      enum class SeekOrigin { Begin, Current, End };

      class Stream
      {
      protected:
         Stream() {}
      public:
         virtual ~Stream() {}

         virtual bool  CanRead() PURE;
         virtual bool  CanSeek() PURE;
         virtual bool  CanWrite() PURE;

         virtual __int64  GetLength() PURE;
         virtual __int64  GetPosition() PURE;

         virtual void  Close() PURE;
         virtual void  Flush() PURE;
         virtual void  Seek(__int64  offset, SeekOrigin  mode) PURE;
         virtual void  SetLength(__int64  length) PURE;

         virtual void  Read(BYTE* buffer, __int64 length) PURE;
         virtual BYTE  ReadByte() PURE;

         virtual void  Write(BYTE* buffer, __int64 length) PURE;
         virtual void  WriteByte(BYTE byte) PURE;
      
      };

      class FileStream : public Stream
      {
      public:
         FileStream(Path path, FileMode mode, FileAccess access, FileShare share) : FullPath(path), Mode(mode), Access(access), Share(share) 
         {
            Handle = CreateFile(path.c_str(), (DWORD)access, (DWORD)share, NULL, mode, attr, NULL);
            
         }
         virtual ~FileStream() {}

         bool  CanRead()   { return Access == FileAccess::Read || Access == FileAccess::ReadWrite; }
         bool  CanSeek()   { return true; }
         bool  CanWrite()  { return Access == FileAccess::Write || Access == FileAccess::ReadWrite; }

         virtual __int64  GetLength() PURE;
         virtual __int64  GetPosition() PURE;

         virtual void  Close() PURE;
         virtual void  Flush() PURE;
         virtual void  Seek(__int64  offset, SeekOrigin  mode) PURE;
         virtual void  SetLength(__int64  length) PURE;

         virtual void  Read(BYTE* buffer, __int64 length) PURE;
         virtual BYTE  ReadByte() PURE;

         virtual void  Write(BYTE* buffer, __int64 length) PURE;
         virtual void  WriteByte(BYTE byte) PURE;
      
      private:
         Path       FullPath;
         FileMode   Mode;
         FileAccess Access;
         FileShare  Share;
         HANDLE     Handle;
      };
   }

}