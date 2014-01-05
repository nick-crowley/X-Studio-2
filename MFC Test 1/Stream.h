#pragma once
#include "Common.h"
#include <Windows.h>

namespace Library
{
   namespace IO
   {
      enum class FileMode { CreateNew = CREATE_NEW, CreateAlways = CREATE_ALWAYS, OpenAlways = OPEN_ALWAYS, OpenExisting = OPEN_EXISTING, TruncateExisting = TRUNCATE_EXISTING };
      enum class FileAccess : UINT { Read = GENERIC_READ, Write = GENERIC_WRITE, ReadWrite = GENERIC_READ|GENERIC_WRITE };
      enum class FileShare : UINT { Read = FILE_SHARE_READ, Write = FILE_SHARE_WRITE, Delete = FILE_SHARE_DELETE, None = 0 };
      enum class SeekOrigin { Begin = FILE_BEGIN, Current = FILE_CURRENT, End = FILE_END };

      class Stream
      {
      protected:
         Stream() {}
      public:
         virtual ~Stream() {}

         virtual bool  CanRead() PURE;
         virtual bool  CanSeek() PURE;
         virtual bool  CanWrite() PURE;

         virtual DWORD GetLength() PURE;
         virtual DWORD GetPosition() PURE;

         virtual void  Close() PURE;
         virtual void  Flush() PURE;
         virtual void  Seek(DWORD  offset, SeekOrigin  mode) PURE;
         virtual void  SetLength(DWORD  length) PURE;

         virtual DWORD Read(BYTE* buffer, DWORD length) PURE;
         virtual DWORD Write(BYTE* buffer, DWORD length) PURE;
      
         virtual BYTE* ReadAllBytes()
         {
            DWORD count = 0,
                  length = GetLength() - GetPosition();
            BYTE* buffer = new BYTE[length+1];
            
            // Ensure null term
            buffer[length] = NULL;

            do
            {  // Read all bytes until EOF
               count += Read(&buffer[count], length - count);
            } 
            while (count < length);

            // return buffer
            return buffer;
         }
      };

      
   }

}