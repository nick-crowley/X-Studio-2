#pragma once
#include "Stream.h"

namespace Library
{
   namespace IO
   {
      class FileStream : public Stream
      {
      public:
         FileStream(Path path, FileMode mode, FileAccess access, FileShare share);
         ~FileStream();
         
         bool  CanRead()   { return Access == FileAccess::Read || Access == FileAccess::ReadWrite; }
         bool  CanSeek()   { return true; }
         bool  CanWrite()  { return Access == FileAccess::Write || Access == FileAccess::ReadWrite; }

         __int64  GetLength();
         __int64  GetPosition();

         void  Close();
         void  Flush();
         void  Seek(__int64  offset, SeekOrigin  mode);
         void  SetLength(__int64  length);

         void  Read(BYTE* buffer, __int64 length);
         BYTE  ReadByte();

         void  Write(BYTE* buffer, __int64 length);
         void  WriteByte(BYTE byte);
      
      private:
         Path       FullPath;
         FileMode   Mode;
         FileAccess Access;
         FileShare  Share;
         HANDLE     Handle;
      };

   }
}