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
         StringReader(Stream*  s, bool owner = true);
         virtual ~StringReader();

         virtual bool   ReadLine(string&  line);

      private:
         bool  PeekByte(BYTE&  b);
         bool  ReadByte(BYTE&  b);

         bool      DestroyOnClose;
         Stream*   Input;
         ByteArray Buffer;
         DWORD     Length,
                   Position;
      };

   }
}