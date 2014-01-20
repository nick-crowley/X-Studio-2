#pragma once
#include "Common.h"
#include "Stream.h"

using namespace Logic::IO;

namespace Logic
{
   namespace IO
   {
      enum class TextEncoding { UTF8, UTF16 };

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

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
		public:	
         PROPERTY_GET(DWORD,LineNumber,GetLineNumber);

			// ---------------------- ACCESSORS ------------------------

         DWORD    GetLineNumber() const     { return LineNum; }

			// ----------------------- MUTATORS ------------------------

         virtual bool   ReadLine(wstring&  line);

      private:
         bool  PeekChar(WCHAR&  c);
         bool  ReadChar(WCHAR&  c);

         // -------------------- REPRESENTATION ---------------------

         StreamPtr    Input;
         CharArrayPtr Buffer;
         DWORD        LineNum,
                      Length,
                      Position;
      };

   }
}