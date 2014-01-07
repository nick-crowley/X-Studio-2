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

         void  Close();
         virtual bool   ReadLine(string&  line);

      private:
         StringReader(const StringReader& r) {}

         bool  PeekByte(BYTE&  b);
         bool  ReadByte(BYTE&  b);

         StreamPtr Input;
         ByteArray Buffer;
         DWORD     Length,
                   Position;
      };

   }
}