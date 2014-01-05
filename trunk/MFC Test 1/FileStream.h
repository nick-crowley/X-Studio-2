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
         FileStream(const FileStream& s);
         ~FileStream();
         
         bool  CanRead() const   { return Access == FileAccess::Read || Access == FileAccess::ReadWrite; }
         bool  CanSeek() const   { return true; }
         bool  CanWrite() const  { return Access == FileAccess::Write || Access == FileAccess::ReadWrite; }

         DWORD GetLength() ;
         DWORD GetPosition() const;

         void  Close();
         void  Flush();
         void  Seek(LONG  offset, SeekOrigin  mode);
         void  SetLength(DWORD  length);

         DWORD Read(BYTE* buffer, DWORD length);
         DWORD Write(const BYTE* buffer, DWORD length);
      
      protected:
         Path       FullPath;
         FileMode   Mode;
         FileAccess Access;
         FileShare  Share;

      private:
         HANDLE     Handle;
      };

   }
}