#pragma once
#include "StringReader.h"

namespace Library
{
   namespace Scripts
   {
         
      /// <summary>Reads the XStudio syntax file</summary>
      class LegacySyntaxReader : StringReader
      {
         const wstring END_BLOCK = L"-------------- END DEFINITION ---------------";

         // --------------------- CONSTRUCTION ----------------------

      public:
         LegacySyntaxReader(StreamPtr src);
         LegacySyntaxReader(LegacySyntaxReader&& r);
         virtual ~LegacySyntaxReader();

         // Class cannot be copied
         NO_COPY(LegacySyntaxReader);
         // Move assign not implemented
         NO_MOVE_ASSIGN(LegacySyntaxReader);

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
			
		   // ---------------------- ACCESSORS ------------------------
			
		   // ----------------------- MUTATORS ------------------------
      public:
         SyntaxFile   ReadFile();

      private:
         bool  RequireLine(wstring& line, const WCHAR* content);
         bool  ReadSyntax(wstring& group, UINT& versions, UINT& id, wstring& url, wstring& syntax, list<wstring>& params);
         bool  TryReadLine(wstring& line);

		   // -------------------- REPRESENTATION ---------------------

      private:
      };

   }
}



