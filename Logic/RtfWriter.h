#pragma once

#include "Stream.h"

namespace Logic
{
   namespace IO
   {
      /// <summary>Generates RTF and writes it into an output stream</summary>
      class LogicExport RtfWriter
      {
      private:
         /// <summary>Unspecified colour sentinel value</summary>
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
         void  Flush();
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

      protected:
         void  Flush(int b);

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
         const static UINT BUFFER_SIZE = 256;

         bool              Closed;        // Whether writer is closed
         vector<COLORREF>  Colours;       // Colours in the RTF colour table
         COLORREF          ForeColour,    // Current forecolour
                           BackColour;    // Current backcolour
         bool              Bold,          // Whether in bold state
                           Italic,        // Whether in italic state
                           Underline,     // Whether in underline state
                           Superscript;   // Whether in superscript state
         UINT              Buffered;      // Number of characters written but not yet flushed
         StreamPtr         Output;        // Output stream
      };

   }
}

using namespace Logic::IO;
