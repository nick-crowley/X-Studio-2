#pragma once
#include "Stream.h"

using namespace Logic::IO;

namespace Logic
{
   namespace IO
   {
      /// <summary>Provides stream access to files on disc</summary>
      class FileStream : public Stream
      {
      public:
         // --------------------- CONSTRUCTION ----------------------

         FileStream(Path path, FileMode mode, FileAccess access, FileShare share = FileShare::AllowRead);
         ~FileStream();

         NO_MOVE(FileStream);
         NO_COPY(FileStream);

         // ------------------------ STATIC -------------------------

         static CharArrayPtr  ConvertFileBuffer(StreamPtr  s, DWORD&  Length);

         // --------------------- PROPERTIES ------------------------
			
			// ---------------------- ACCESSORS ------------------------

         bool  CanRead() const   { return Access == FileAccess::Read || Access == FileAccess::ReadWrite; }
         bool  CanSeek() const   { return true; }
         bool  CanWrite() const  { return Access == FileAccess::Write || Access == FileAccess::ReadWrite; }

         DWORD GetLength();
         DWORD GetPosition() const;

         // ----------------------- MUTATORS ------------------------

         void  Close();
         void  Flush();
         void  SafeClose();
         void  Seek(LONG  offset, SeekOrigin  mode);
         void  SetLength(DWORD  length);

         DWORD Read(BYTE* buffer, DWORD length);
         DWORD Write(const BYTE* buffer, DWORD length);
      
         // -------------------- REPRESENTATION ---------------------

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