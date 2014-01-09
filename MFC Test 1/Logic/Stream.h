#pragma once
#include "Common.h"


namespace Logic
{
   namespace IO
   {
      /// <summary>Defines how a file is opened or created</summary>
      enum class FileMode { CreateNew = CREATE_NEW, CreateAlways = CREATE_ALWAYS, OpenAlways = OPEN_ALWAYS, OpenExisting = OPEN_EXISTING, TruncateExisting = TRUNCATE_EXISTING };

      /// <summary>Defines read/write access on a stream</summary>
      enum class FileAccess : UINT { Read = GENERIC_READ, Write = GENERIC_WRITE, ReadWrite = GENERIC_READ|GENERIC_WRITE };

      /// <summary>Defines how a stream is shared</summary>
      enum class FileShare : UINT { AllowRead = FILE_SHARE_READ, AllowWrite = FILE_SHARE_WRITE, AllowDelete = FILE_SHARE_DELETE, AllowNone = 0 };

      /// <summary>Defines the origin of a stream seeking operation</summary>
      enum class SeekOrigin { Begin = FILE_BEGIN, Current = FILE_CURRENT, End = FILE_END };

      /// <summary>Represents any stream</summary>
      class Stream
      {
         // --------------------- CONSTRUCTION ----------------------

      protected:
         Stream() {}
      public:
         virtual ~Stream() {  }

         // Prevent copying/moving
         NO_MOVE(Stream);
         NO_COPY(Stream);

         // --------------------- PROPERTIES ------------------------
			
			// ---------------------- ACCESSORS ------------------------

         virtual bool  CanRead() const PURE;
         virtual bool  CanSeek() const PURE;
         virtual bool  CanWrite() const PURE;

         virtual DWORD GetLength()  PURE;
         virtual DWORD GetPosition() const PURE;

         // ----------------------- MUTATORS ------------------------

         virtual void  Close() PURE;
         virtual void  Flush() PURE;
         virtual void  SafeClose() PURE;
         virtual void  Seek(LONG  offset, SeekOrigin  mode) PURE;
         virtual void  SetLength(DWORD  length) PURE;

         virtual DWORD Read(BYTE* buffer, DWORD length) PURE;
         virtual DWORD Write(const BYTE* buffer, DWORD length) PURE;
      
         /// <summary>Reads all remaining bytes from a stream into a buffer</summary>
         /// <returns></returns>
         virtual ByteArrayPtr ReadAllBytes()
         {
            DWORD        count = 0,
                         length = GetLength() - GetPosition();
            ByteArrayPtr buffer( new BYTE[length+1] );
            
            // Ensure null terminated
            buffer.get()[length] = NULL;

            do
            {  // Read all bytes until EOF
               count += Read(buffer.get()+count, length - count);
            } 
            while (count < length);

            // return buffer
            return buffer;
         }

         // -------------------- REPRESENTATION ---------------------
      };

      /// <summary>A shared pointer to any stream</summary>
      typedef shared_ptr<Stream>  StreamPtr;

      /// <summary>Allows for subclassing a stream using the Facade pattern</summary>
      class StreamFacade : public Stream
      {
         // --------------------- CONSTRUCTION ----------------------

      protected:
         StreamFacade(StreamPtr src) : Source(src)
         { 
            REQUIRED(src); 
         }
      public:
         virtual ~StreamFacade() 
         { 
            SafeClose();
         }

         // Prevent copying/moving
         NO_MOVE(StreamFacade);
         NO_COPY(StreamFacade);

			// --------------------- PROPERTIES ------------------------
			
			// ---------------------- ACCESSORS ------------------------

         bool  CanRead() const                         { return Source->CanRead();  }
         bool  CanSeek() const                         { return Source->CanSeek();  }
         bool  CanWrite() const                        { return Source->CanWrite(); }

         DWORD GetLength()                             { return Source->GetLength(); }
         DWORD GetPosition() const                     { return Source->GetPosition(); }

         // ----------------------- MUTATORS ------------------------

         void  Close()                                 { return Source->Close(); }
         void  Flush()                                 { return Source->Flush(); }
         void  SafeClose()                             { Source->SafeClose();    }
         void  Seek(LONG  offset, SeekOrigin  mode)    { return Source->Seek(offset, mode); }
         void  SetLength(DWORD  length)                { return Source->SetLength(length);  }

         DWORD Read(BYTE* buffer, DWORD length)        { return Source->Read(buffer, length); }
         DWORD Write(const BYTE* buffer, DWORD length) { return Source->Write(buffer, length); }

      private:
         // -------------------- REPRESENTATION ---------------------

         StreamPtr  Source;
      };
      
   }

}

using namespace Logic::IO;
