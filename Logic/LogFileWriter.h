#pragma once


#include "RtfWriter.h"

namespace Logic
{
   namespace IO
   {

      /// <summary></summary>
      class LogicExport LogFileWriter : private RtfWriter
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
      protected:
         const static COLORREF Black   = RGB(0,0,0);
         const static COLORREF Blue    = RGB(0,0,210);
         const static COLORREF Cyan    = RGB(0,210,210);
         const static COLORREF Green   = RGB(0,210,0);
         const static COLORREF Magenta = RGB(210,0,210);
         const static COLORREF Red     = RGB(255,0,0);
         const static COLORREF Yellow  = RGB(249,192,0);
         const static COLORREF White   = RGB(255,255,255);

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
