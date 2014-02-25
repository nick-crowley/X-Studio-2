#pragma once

#include "Common.h"

namespace Logic
{
   namespace IO
   {

      /// <summary></summary>
      class RichStringWriter
      {
         // ------------------------ TYPES --------------------------
      private:

         // --------------------- CONSTRUCTION ----------------------

      public:
         RichStringWriter(TextDocumentPtr& doc);
         virtual ~RichStringWriter();

         DEFAULT_COPY(RichStringWriter);	// Default copy semantics
         DEFAULT_MOVE(RichStringWriter);	// Default move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------
      public:
         wstring  Write();

      protected:
         void  WriteChar(TextRangePtr chr, TextRangePtr prev);
         void  WriteChar(TextRangePtr chr, bool open);

         // -------------------- REPRESENTATION ---------------------

      protected:
         TextDocumentPtr Input;
         wstring         Output;
      };

   }
}

using namespace Logic::IO;
