#pragma once

#include "Stream.h"
#include "XFileInfo.h"

namespace Logic
{
   namespace IO
   {

      /// <summary>Provides stream access to the contents of catalog data files</summary>
      class LogicExport DataStream : public StreamDecorator
      {
      const byte  DATAFILE_ENCRYPT_KEY = 0x33;
         
         // --------------------- CONSTRUCTION ----------------------

      public:
         DataStream(const XFileInfo& f);
         ~DataStream();

         // This class LogicExport cannot be moved
         NO_MOVE(DataStream);
         // This class LogicExport cannot be copied
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

using namespace Logic::IO;
