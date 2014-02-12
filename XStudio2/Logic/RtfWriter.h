#pragma once

#include "Common.h"

namespace Logic
{
   namespace IO
   {
      
      /// <summary></summary>
      class RtfWriter
      {
      private:
         const COLORREF  COLOUR_NONE = 0xff000000;

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
         void  ResetParagraph();
         void  SetAlignment(Alignment al);
         void  SetBackColour(COLORREF c);
         void  SetBold(bool b);
         void  SetForeColour(COLORREF c);
         void  SetFontSize(UINT size);
         void  SetItalic(bool i);
         void  SetLeftMargin(UINT size);
         void  SetUnderline(bool u);
         void  StartParagraph();

      private:
         void  SetCodePage(UINT cp);
         void  SetDefaultFont(UINT font);
         void  SetLanguage(UINT lang);
         void  Write(UINT i);
         void  Write(const CHAR* str);
         void  WriteColour(COLORREF c);
         void  WriteFooter();
         void  WriteHeader();
         
		   // -------------------- REPRESENTATION ---------------------

      private:
         bool              Closed;
         wstring           Font;
         vector<COLORREF>  Colours;
         COLORREF          ForeColour,
                           BackColour;
         bool              Bold,
                           Italic,
                           Underline,
                           Superscript;

         string&           Output;
      };

   }
}

using namespace Logic::IO;
