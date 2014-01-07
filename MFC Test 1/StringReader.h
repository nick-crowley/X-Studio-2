#pragma once
#include "Common.h"
#include "Stream.h"

using namespace Library::IO;

namespace Library
{
   namespace IO
   {

      /// <summary>Reads strings from a stream in lines</summary>
      class StringReader
      {
         // --------------------- CONSTRUCTION ----------------------

      public:
         StringReader(StreamPtr src);
         StringReader(StringReader&& r);
         virtual ~StringReader();

         // Prevent copying.  Move assign not impl
         NO_COPY(StringReader);
         StringReader& operator=(StringReader&& r) = delete;

         // --------------------- PROPERTIES ------------------------
			
			// ---------------------- ACCESSORS ------------------------

			// ----------------------- MUTATORS ------------------------

         virtual bool   ReadLine(string&  line);

      private:
         bool  PeekByte(BYTE&  b);
         bool  ReadByte(BYTE&  b);

         // -------------------- REPRESENTATION ---------------------

         StreamPtr    Input;
         ByteArrayPtr Buffer;
         DWORD        Length,
                      Position;
      };

   }
}