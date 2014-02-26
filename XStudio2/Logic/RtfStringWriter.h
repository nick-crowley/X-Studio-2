#pragma once

#include "Common.h"
#include "RtfWriter.h"
#include "RichString.h"

namespace Logic
{
   namespace IO
   {

      /// <summary></summary>
      class RtfStringWriter : protected RtfWriter
      {
         // ------------------------ TYPES --------------------------
      private:

         // --------------------- CONSTRUCTION ----------------------

      public:
         RtfStringWriter(string& out);
         virtual ~RtfStringWriter();

         DEFAULT_COPY(RtfStringWriter);	// Default copy semantics
         DEFAULT_MOVE(RtfStringWriter);	// Default move semantics

         // ------------------------ STATIC -------------------------
      public:
         static COLORREF ToRGB(Colour c);
         
         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------
      public:
         void  Close();
         void  Write(const RichString& str);

         // -------------------- REPRESENTATION ---------------------

      private:
      };

   }
}

using namespace Logic::IO;
