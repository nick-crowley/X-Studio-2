#pragma once

#include "Common.h"
#include "RtfWriter.h"

namespace Logic
{
   namespace IO
   {

      /// <summary></summary>
      class LogFileWriter : private RtfWriter
      {
         // ------------------------ TYPES --------------------------
      protected:

         // --------------------- CONSTRUCTION ----------------------

      public:
         LogFileWriter(StreamPtr out);
         virtual ~LogFileWriter();

         NO_COPY(LogFileWriter);	// No copy semantics
         NO_MOVE(LogFileWriter);	// No move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------
      public:
         void  Close();
         void  Write(const wstring& txt, WORD attr);

         // -------------------- REPRESENTATION ---------------------

      protected:
      };

   }
}

using namespace Logic::IO;
