#pragma once


#include "Stream.h"

namespace Logic
{
   namespace IO
   {
      
      /// <summary></summary>
      class LogicExport RtfWriter
      {
      private:
         const COLORREF  COLOUR_NONE = 0xff000000;

         // --------------------- CONSTRUCTION ----------------------

      public:
         RtfWriter(StreamPtr out);
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
         void  WriteChar(CHAR chr);
         void  WriteString(const CHAR* str);
         void  WriteColour(COLORREF c);
         void  WriteFooter();
         void  WriteHeader(const wstring& font);
         
		   // -------------------- REPRESENTATION ---------------------

      private:
         bool              Closed;
         vector<COLORREF>  Colours;
         COLORREF          ForeColour,
                           BackColour;
         bool              Bold,
                           Italic,
                           Underline,
                           Superscript;

         StreamPtr         Output;
      };

   }
}

using namespace Logic::IO;
