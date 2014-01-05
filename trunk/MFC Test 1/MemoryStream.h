#pragma once
#include "Common.h"
#include "Stream.h"

namespace Library
{
   namespace IO
   {
      class MemoryStream : public Stream
      {
      public:
         MemoryStream(BYTE* buffer, DWORD length, FileAccess access);
         MemoryStream(DWORD length, FileAccess access);
         ~MemoryStream();
         
         bool  CanRead()   { return Access == FileAccess::Read || Access == FileAccess::ReadWrite; }
         bool  CanSeek()   { return true; }
         bool  CanWrite()  { return Access == FileAccess::Write || Access == FileAccess::ReadWrite; }

         DWORD GetLength();
         DWORD GetPosition();

         void  Close();
         void  Flush();
         void  Seek(DWORD  offset, SeekOrigin  mode);
         void  SetLength(DWORD  length);

         DWORD Read(BYTE* buffer, DWORD length);
         DWORD Write(BYTE* buffer, DWORD length);
      
      private:
         BYTE* Alloc(DWORD len);

         FileAccess Access;
         DWORD      Length,
                    Position;
         BYTE*      Buffer;
         bool       DestroyOnClose;
      };

   }
}