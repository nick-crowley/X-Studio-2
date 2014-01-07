#pragma once
#include "Common.h"
#include "Stream.h"

namespace Library
{
   namespace IO
   {

      class StringReader
      {
      public:
         StringReader(StreamPtr src);
         StringReader(StringReader&& r);
         virtual ~StringReader();

         // Prevent copying.  Move assign not impl
         NO_COPY(StringReader);
         StringReader& operator=(StringReader&& r) = delete;

         virtual bool   ReadLine(string&  line);

      private:
         bool  PeekByte(BYTE&  b);
         bool  ReadByte(BYTE&  b);

         StreamPtr Input;
         ByteArrayPtr Buffer;
         DWORD     Length,
                   Position;
      };

   }
}