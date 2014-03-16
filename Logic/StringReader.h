#pragma once

#include "Stream.h"

using namespace Logic::IO;

namespace Logic
{
   namespace IO
   {
      enum class TextEncoding { UTF8, UTF16 };

      /// <summary>Reads strings from a stream in lines</summary>
      class LogicExport StringReader
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
         /// <summary>Get the current line number</summary>
         PROPERTY_GET(DWORD,LineNumber,GetLineNumber);

			// ---------------------- ACCESSORS ------------------------

      public:
         /// <summary>Get the current line number</summary>
         /// <returns>One based line number</returns>
         DWORD    GetLineNumber() const  { return LineNum; }

         /// <summary>Check for EOF</summary>
         /// <returns></returns>
         bool     IsEOF() const          { return Position >= Length; }

			// ----------------------- MUTATORS ------------------------

         virtual bool  ReadChar(WCHAR&  c);
         virtual bool  ReadLine(wstring&  line);

      protected:
         bool  PeekChar(WCHAR&  c);
         
         // -------------------- REPRESENTATION ---------------------

      private:
         StreamPtr    Input;
         CharArrayPtr Buffer;
         DWORD        LineNum,
                      Length,
                      Position;
      };

   }
}