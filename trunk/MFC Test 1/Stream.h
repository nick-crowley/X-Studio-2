#pragma once
#include "Stream.h"
#include "Exceptions.h"
#include <Windows.h>

namespace Library
{
   namespace IO
   {
      enum class FileMode { CreateNew = CREATE_NEW, CreateAlways = CREATE_ALWAYS, OpenAlways = OPEN_ALWAYS, OpenExisting = OPEN_EXISTING, TruncateExisting = TRUNCATE_EXISTING };
      enum class FileAccess : UINT { Read = GENERIC_READ, Write = GENERIC_WRITE, ReadWrite = GENERIC_READ|GENERIC_WRITE };
      enum class FileShare : UINT { Read = FILE_SHARE_READ, Write = FILE_SHARE_WRITE, Delete = FILE_SHARE_DELETE, None = 0 };
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

      
   }

}