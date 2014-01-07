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

         // Prevent copying/moving
         NO_MOVE(MemoryStream);
         NO_COPY(MemoryStream);
         
         bool  CanRead() const   { return Access == FileAccess::Read || Access == FileAccess::ReadWrite; }
         bool  CanSeek() const   { return true; }
         bool  CanWrite() const  { return Access == FileAccess::Write || Access == FileAccess::ReadWrite; }

         DWORD GetLength() const;
         DWORD GetPosition() const;

         void  Close();
         void  Flush();
         void  SafeClose()  { };
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