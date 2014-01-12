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
         RtfWriter(const wstring& font, UINT size, list<COLORREF> cols);
         virtual ~RtfWriter();

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
			
		   // ---------------------- ACCESSORS ------------------------			

      public:
         string ToString();

		   // ----------------------- MUTATORS ------------------------

      public:
         void  Close();

         void  Write(WCHAR ch);
         void  Write(const wstring& str);
         void  WriteLn(const wstring& str);
         void  SetColour(COLORREF c);
         void  SetSize(int points);

      private:
         void  Write(const CHAR* str);

         void  WriteColour(COLORREF c);
         void  WriteFooter();
         void  WriteHeader();
         
		   // -------------------- REPRESENTATION ---------------------

      private:
         bool              Closed;
         wstring           Font;
         string            Output;
         vector<COLORREF>  Colours;
      };

   }
}

using namespace Logic::IO;
