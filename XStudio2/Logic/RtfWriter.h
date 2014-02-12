#pragma once

#include "Common.h"

namespace Logic
{
   namespace IO
   {
         
      /// <summary></summary>
      class RtfWriter
      {
         // --------------------- CONSTRUCTION ----------------------

      public:
         RtfWriter(string& out);
         virtual ~RtfWriter();

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
			
		   // ---------------------- ACCESSORS ------------------------			

		   // ----------------------- MUTATORS ------------------------
      public:
         void  Close();
         void  Open(const wstring& font, UINT size, list<COLORREF> cols);
         void  Write(WCHAR ch);
         void  Write(const wstring& str);
         void  WriteLn(const wstring& str);
         void  SetForeColour(COLORREF c);
         void  SetFontSize(int points);

      private:
         void  Write(const CHAR* str);
         void  WriteColour(COLORREF c);
         void  WriteFooter();
         void  WriteHeader();
         
		   // -------------------- REPRESENTATION ---------------------

      private:
         bool              Closed;
         wstring           Font;
         vector<COLORREF>  Colours;
         COLORREF          Colour;

         string&           Output;
      };

   }
}

using namespace Logic::IO;
