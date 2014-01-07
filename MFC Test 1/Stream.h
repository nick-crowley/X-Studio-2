#pragma once
#include "Common.h"
#include <Windows.h>

namespace Library
{
   namespace IO
   {
      enum class FileMode { CreateNew = CREATE_NEW, CreateAlways = CREATE_ALWAYS, OpenAlways = OPEN_ALWAYS, OpenExisting = OPEN_EXISTING, TruncateExisting = TRUNCATE_EXISTING };
      enum class FileAccess : UINT { Read = GENERIC_READ, Write = GENERIC_WRITE, ReadWrite = GENERIC_READ|GENERIC_WRITE };
      enum class FileShare : UINT { AllowRead = FILE_SHARE_READ, AllowWrite = FILE_SHARE_WRITE, AllowDelete = FILE_SHARE_DELETE, AllowNone = 0 };
      enum class SeekOrigin { Begin = FILE_BEGIN, Current = FILE_CURRENT, End = FILE_END };

      class Stream
      {
      protected:
         Stream() {}
      public:
         virtual ~Stream() {}

         // Prevent copying/moving
         NO_MOVE(Stream);
         NO_COPY(Stream);

         virtual bool  CanRead() const PURE;
         virtual bool  CanSeek() const PURE;
         virtual bool  CanWrite() const PURE;

         virtual DWORD GetLength()  PURE;
         virtual DWORD GetPosition() const PURE;

         virtual void  Close() PURE;
         virtual void  Flush() PURE;
         virtual void  Seek(LONG  offset, SeekOrigin  mode) PURE;
         virtual void  SetLength(DWORD  length) PURE;

         virtual DWORD Read(BYTE* buffer, DWORD length) PURE;
         virtual DWORD Write(const BYTE* buffer, DWORD length) PURE;
      
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

      typedef shared_ptr<Stream>  StreamPtr;

      class StreamFacade : public Stream
      {
      protected:
         StreamFacade(StreamPtr src) : Source(src)
         { 
            REQUIRED(src); 
         }
      public:
         virtual ~StreamFacade() 
         { 
            Source->Close();
         }

         // Prevent copying/moving
         NO_MOVE(StreamFacade);
         NO_COPY(StreamFacade);

         bool  CanRead() const                         { return Source->CanRead();  }
         bool  CanSeek() const                         { return Source->CanSeek();  }
         bool  CanWrite() const                        { return Source->CanWrite(); }

         DWORD GetLength()                             { return Source->GetLength(); }
         DWORD GetPosition() const                     { return Source->GetPosition(); }

         void  Close()                                 { return Source->Close(); }
         void  Flush()                                 { return Source->Flush(); }
         void  Seek(LONG  offset, SeekOrigin  mode)    { return Source->Seek(offset, mode); }
         void  SetLength(DWORD  length)                { return Source->SetLength(length);  }

         DWORD Read(BYTE* buffer, DWORD length)        { return Source->Read(buffer, length); }
         DWORD Write(const BYTE* buffer, DWORD length) { return Source->Write(buffer, length); }

      private:
         StreamPtr  Source;
      };
      
   }

}