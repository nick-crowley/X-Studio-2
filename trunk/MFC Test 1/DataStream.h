#pragma once
#include "Common.h"
#include "Stream.h"
#include "XFileInfo.h"

using namespace Library::IO;

namespace Library
{
   namespace IO
   {

      /// <summary>Provides stream access to the contents of catalog data files</summary>
      class DataStream : public StreamFacade
      {
      const byte  DATAFILE_ENCRYPT_KEY = 0x33;
         
         // --------------------- CONSTRUCTION ----------------------

      public:
         DataStream(const XFileInfo& f);
         ~DataStream();

         // This class cannot be moved
         NO_MOVE(DataStream);
         // This class cannot be copied
         NO_COPY(DataStream);

         // --------------------- PROPERTIES ------------------------
			
			// ---------------------- ACCESSORS ------------------------

         DWORD  GetLength();
         DWORD  GetPosition() const;

			// ----------------------- MUTATORS ------------------------

         DWORD  Read(BYTE* buffer, DWORD length);
         void   Seek(LONG  offset, SeekOrigin  mode);
         DWORD  Write(const BYTE* buffer, DWORD length);

      private:
         void   Encode(byte* buffer, DWORD length);

         // -------------------- REPRESENTATION ---------------------

         const XFileInfo&  File;
      };

   }
}