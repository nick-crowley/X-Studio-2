#pragma once
#include "../Common.h"
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

         // Class cannot be copied 
         NO_COPY(StringReader);
         // Move assign not implemented
         NO_MOVE_ASSIGN(StringReader);

         // --------------------- PROPERTIES ------------------------
			
         PROPERTY_GET(DWORD,LineNumber,GetLineNumber);

			// ---------------------- ACCESSORS ------------------------

         DWORD    GetLineNumber() const     { return LineNum; }

			// ----------------------- MUTATORS ------------------------

         virtual bool   ReadLine(string&  line);

      private:
         bool  PeekByte(BYTE&  b);
         bool  ReadByte(BYTE&  b);

         // -------------------- REPRESENTATION ---------------------

         StreamPtr    Input;
         ByteArrayPtr Buffer;
         DWORD        LineNum,
                      Length,
                      Position;
      };

   }
}