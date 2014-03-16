#pragma once

#include "Stream.h"

using namespace Logic::IO;

namespace Logic
{
   namespace IO
   {
      class LogicExport MemoryStream : public Stream
      {
         // --------------------- CONSTRUCTION ----------------------

      public:
         MemoryStream(BYTE* buffer, DWORD length, FileAccess access);
         MemoryStream(DWORD length, FileAccess access);
         ~MemoryStream();

         // Prevent copying/moving
         NO_MOVE(MemoryStream);
         NO_COPY(MemoryStream);

         // --------------------- PROPERTIES ------------------------
			
			// ---------------------- ACCESSORS ------------------------

         bool  CanRead() const   { return Access == FileAccess::Read || Access == FileAccess::ReadWrite; }
         bool  CanSeek() const   { return true; }
         bool  CanWrite() const  { return Access == FileAccess::Write || Access == FileAccess::ReadWrite; }

         DWORD GetLength() const;
         DWORD GetPosition() const;

         // ----------------------- MUTATORS ------------------------

         void  Close();
         void  Flush();
         void  SafeClose()  { };
         void  Seek(DWORD  offset, SeekOrigin  mode);
         void  SetLength(DWORD  length);

         DWORD Read(BYTE* buffer, DWORD length);
         DWORD Write(BYTE* buffer, DWORD length);
      
      private:
         BYTE* Alloc(DWORD len);

         // -------------------- REPRESENTATION ---------------------

         FileAccess Access;
         DWORD      Length,
                    Position;
         BYTE*      Buffer;
         bool       DestroyOnClose;
      };

   }
}