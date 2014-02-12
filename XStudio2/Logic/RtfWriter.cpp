#include "stdafx.h"
#include "RtfWriter.h"
#include <strsafe.h>

namespace Logic
{
   namespace IO
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Creates an RTF Writer</summary>
      /// <param name="out">Output</param>
      RtfWriter::RtfWriter(string& out) 
         : Output(out), Closed(true), ForeColour(COLOUR_NONE), BackColour(COLOUR_NONE), Bold(false), Italic(false), Underline(false)
      {
         
      }

      /// <summary>Closes the writer, if open</summary>
      RtfWriter::~RtfWriter()
      {
         if (!Closed)
            Close();
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Appends the RTF footer and closes the writer</summary>
      /// <exception cref="Logic::InvalidOperationException">Writer has been closed</exception>
      void   RtfWriter::Close()
      { 
         // Check stream is open
         if (Closed)
            throw InvalidOperationException(HERE, L"Writer is closed");

         // Writer footer
         WriteFooter(); 
         Closed = true;
      }

      /// <summary>Initialises the writer</summary>
      /// <param name="font">Font name</param>
      /// <param name="size">Font size in points</param>
      /// <param name="cols">List of colours</param>
      void  RtfWriter::Open(const wstring& font, UINT size, list<COLORREF> cols) 
      {
         // Check stream is closed
         if (!Closed)
            throw InvalidOperationException(HERE, L"Writer is already open");
         Closed = false;

         // Copy colours
         for (COLORREF c : cols)
            Colours.push_back(c);

         // Write header
         WriteHeader();
         SetFontSize(size);
      }

      /// <summary>Resets paragraph properties</summary>
      void  RtfWriter::ResetParagraph()
      {
         // Check stream is open
         if (Closed)
            throw InvalidOperationException(HERE, L"Writer is closed");

         // Write
         Write("\\pard ");
      }
      
      /// <summary>Sets paragraph alignment</summary>
      /// <param name="al">Alignment</param>
      void  RtfWriter::SetAlignment(Alignment al)
      {
         // Check stream is open
         if (Closed)
            throw InvalidOperationException(HERE, L"Writer is closed");

         // set alignment
         switch (al)
         {
         case Alignment::Left:      Write("\\ql ");  break;
         case Alignment::Centre:    Write("\\qc ");  break;
         case Alignment::Right:     Write("\\qr ");  break;
         case Alignment::Justify:   Write("\\qj ");  break;
         }
      }

      
      /// <summary>Sets the background colour</summary>
      /// <param name="c">The colour</param>
      /// <exception cref="Logic::ArgumentException">Unrecognised colour</exception>
      /// <exception cref="Logic::InvalidOperationException">Writer has been closed</exception>
      void   RtfWriter::SetBackColour(COLORREF c)
      {
         CHAR buf[10];

         // Check stream is open
         if (Closed)
            throw InvalidOperationException(HERE, L"Writer is closed");

         // Skip if colour already set
         if (BackColour == c)
            return;

         // Lookup colour ID
         for (UINT id = 0; id < Colours.size(); id++)
            if (Colours[id] == c)
            {  // Write colour change
               StringCchPrintfA(buf, 10, "\\cb%d ", id+1);
               Write(buf);
               BackColour = c;
               return;
            }

         // Unknown colour
         throw ArgumentException(HERE, L"c", L"No such colour in the colour table");
      }

      /// <summary>Sets or clears bold text</summary>
      /// <param name="b">True to set, False to clear</param>
      /// <exception cref="Logic::InvalidOperationException">Writer has been closed</exception>
      void   RtfWriter::SetBold(bool b)
      {
         // Check stream is open
         if (Closed)
            throw InvalidOperationException(HERE, L"Writer is closed");

         // Skip if bold already set
         if (Bold == b)
            return;
         
         // Set/Clear Bold
         Bold = b;
         Write(Bold ? "\\b " : "\\b0 ");
      }

      /// <summary>Sets the text colour</summary>
      /// <param name="c">The colour</param>
      /// <exception cref="Logic::ArgumentException">Unrecognised colour</exception>
      /// <exception cref="Logic::InvalidOperationException">Writer has been closed</exception>
      void   RtfWriter::SetForeColour(COLORREF c)
      {
         CHAR buf[10];

         // Check stream is open
         if (Closed)
            throw InvalidOperationException(HERE, L"Writer is closed");

         // Skip if colour already set
         if (ForeColour == c)
            return;

         // Lookup colour ID
         for (UINT id = 0; id < Colours.size(); id++)
            if (Colours[id] == c)
            {  // Write colour change
               StringCchPrintfA(buf, 10, "\\cf%d ", id+1);
               Write(buf);
               ForeColour = c;
               return;
            }

         // Unknown colour
         throw ArgumentException(HERE, L"c", L"No such colour in the colour table");
      }

      /// <summary>Sets the font size</summary>
      /// <param name="size">The size in points</param>
      /// <exception cref="Logic::InvalidOperationException">Writer has been closed</exception>
      void   RtfWriter::SetFontSize(UINT size)
      {
         CHAR buf[10];

         // Check stream is open
         if (Closed)
            throw InvalidOperationException(HERE, L"Writer is closed");

         // Set size
         StringCchPrintfA(buf, 10, "\\fs%d ", size*2);
         Write(buf);
      }

      /// <summary>Sets or clears italic text</summary>
      /// <param name="i">True to set, False to clear</param>
      /// <exception cref="Logic::InvalidOperationException">Writer has been closed</exception>
      void   RtfWriter::SetItalic(bool i)
      {
         // Check stream is open
         if (Closed)
            throw InvalidOperationException(HERE, L"Writer is closed");

         // Skip if italic already set
         if (Italic == i)
            return;
         
         // Set/Clear Italic
         Italic = i;
         Write(Italic ? "\\i " : "\\i0 ");
      }

      /// <summary>Sets size of left margin for current paragraph</summary>
      /// <param name="size">size in twips</param>
      /// <exception cref="Logic::InvalidOperationException">Writer has been closed</exception>
      void   RtfWriter::SetLeftMargin(UINT twips)
      {
         CHAR buf[10];

         // Check stream is open
         if (Closed)
            throw InvalidOperationException(HERE, L"Writer is closed");

         // Set margin size
         StringCchPrintfA(buf, 10, "\\li%d ", twips);
         Write(buf);
      }
      
      /// <summary>Sets or clears Underline text</summary>
      /// <param name="u">True to set, False to clear</param>
      /// <exception cref="Logic::InvalidOperationException">Writer has been closed</exception>
      void   RtfWriter::SetUnderline(bool u)
      {
         // Check stream is open
         if (Closed)
            throw InvalidOperationException(HERE, L"Writer is closed");

         // Skip if Underline already set
         if (Underline == u)
            return;
         
         // Set/Clear Underline
         Underline = u;
         Write(Underline ? "\\ul " : "\\ul0 ");
      }
      
      /// <summary>Starts a new paragraph</summary>
      void  RtfWriter::StartParagraph()
      {
         // Check stream is open
         if (Closed)
            throw InvalidOperationException(HERE, L"Writer is closed");

         // Write
         Write("\\par ");
      }
      
      /// <summary>Writes UNICODE character to the stream, converting to RTF as necessary</summary>
      /// <param name="ch">The character</param>
      /// <exception cref="Logic::InvalidOperationException">Writer has been closed</exception>
      void   RtfWriter::Write(WCHAR ch)
      {
         CHAR buf[10];

         // Check stream is open
         if (Closed)
            throw InvalidOperationException(HERE, L"Writer is closed");

         // ANSI char:
         if (ch == L'{' || ch == L'}')
         {
            Output.push_back('\\');
            Output.push_back((CHAR)ch);
         }
         else if (ch <= 0x7f)
            Output.push_back((CHAR)ch);
         else
         {  // UNICODE char:
            StringCchPrintfA(buf, 10, "\\u%u?", (UINT)ch);
            Output.append(buf);
         }
      }

      /// <summary>Writes UNICODE text to the stream, converting to RTF as necessary</summary>
      /// <param name="str">The text</param>
      /// <exception cref="Logic::InvalidOperationException">Writer has been closed</exception>
      void   RtfWriter::Write(const wstring& str)
      {
         // Check stream is open
         if (Closed)
            throw InvalidOperationException(HERE, L"Writer is closed");

         // Check string exists
         if (str.length() > 0)
            for (WCHAR ch : str) 
               // Conv ANSI chars to CHAR, and UNICODE chars to '\uNN' RTF tag
               Write(ch);
      }

      /// <summary>Writes UNICODE text to the stream followed by a line break, converting to RTF as necessary</summary>
      /// <param name="str">The text</param>
      /// <exception cref="Logic::InvalidOperationException">Writer has been closed</exception>
      void   RtfWriter::WriteLn(const wstring& str)
      {
         // Check stream is open
         if (Closed)
            throw InvalidOperationException(HERE, L"Writer is closed");

         // Write str + CRLF
         Write(str);
         Write("\\line ");
      }

		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

      /// <summary>Sets the code page.</summary>
      /// <param name="cp">Code page</param>
      void RtfWriter::SetCodePage(UINT cp)
      {
         CHAR buf[10];
         StringCchPrintfA(buf, 10, "\\ansicpg%d ", cp);
         Write(buf);
      }

      /// <summary>Sets default font</summary>
      /// <param name="font">Font table index</param>
      void RtfWriter::SetDefaultFont(UINT font)
      {
         CHAR buf[10];
         StringCchPrintfA(buf, 10, "\\deff%d ", font);
         Write(buf);
      }

      /// <summary>Sets the language.</summary>
      /// <param name="lang">Decimal language ID</param>
      void RtfWriter::SetLanguage(UINT lang)
      {
         CHAR buf[10];
         StringCchPrintfA(buf, 10, "\\deflang%d ", lang);
         Write(buf);
      }

      /// <summary>Writes a number to the stream as ANSI text</summary>
      /// <param name="i">Number</param>
      void  RtfWriter::Write(UINT i) 
      { 
         CHAR buf[16];
         _itoa_s(i, buf, 16, 10);
         Output += buf; 
      }

      /// <summary>Writes ANSI text to the stream verbatim</summary>
      /// <param name="str">The text</param>
      void  RtfWriter::Write(const CHAR* str) 
      { 
         Output += str; 
      }
      
      /// <summary>Writes an RTF colour table colour definition</summary>
      /// <param name="c">The colour to define</param>
      void  RtfWriter::WriteColour(COLORREF c)
      {
         Write(StringResource::Format(L"\\red%d\\green%d\\blue%d;", GetRValue(c), GetGValue(c), GetBValue(c)));
      }

      /// <summary>Writes RTF footer</summary>
      void  RtfWriter::WriteFooter()
      {
         // End paragraph
         Write("\\par\n}");
      }

      /// <summary>Writes RTF header</summary>
      void  RtfWriter::WriteHeader()
      {
         // Header
         Write("{\\rtf1\\ansi");

         // Set codepage + language
         SetCodePage(1250);
         SetDefaultFont(0);
         SetLanguage(2057);      // English UK  (0x0809 == 2057)

         // Font table
         Write("{\\fonttbl{\\f0\\fswiss\\fcharset0 "); 
         Write(Font); 
         Write(";}}\n");

         // Colour table
         Write("{\\colortbl;");
         for (COLORREF c : Colours)
            WriteColour(c);
         Write("}\n");


         // Unknown previous formatting: //\\sa200\\sl276\\slmult1
         // 
         // \saN == spacing after
         // \slN == space between lines
         // \smult == Line spacing multiple
         // \pard == reset paragraph formatting
         
         // Set view: 0==None, 1==Page Layout view, 2==Outline view, 3==Master Document view, 4==Normal view, 5==Online Layout view
         Write("\\viewkind4");

         // Set unicode destination (not sure)
         Write("\\uc1 ");   

         // Reset paragraph
         ResetParagraph();
      }
      
   }
}