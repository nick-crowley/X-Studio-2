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
      private:
         const static COLORREF  Black = RGB(0,0,0),   White  = RGB(255,255,255), Cyan   = RGB(150,220,220), Blue   = RGB(0,0,255), 
                                Green = RGB(0,255,0), Grey   = RGB(128,128,128), Orange = RGB(255,128,40),  Purple = RGB(255,0,255), 
                                Red   = RGB(255,0,0), Yellow = RGB(255,255,0);

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
