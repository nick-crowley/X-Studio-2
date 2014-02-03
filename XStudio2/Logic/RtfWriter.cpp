#include "stdafx.h"
#include "RtfWriter.h"
#include <strsafe.h>

namespace Logic
{
   namespace IO
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Creates an RTF Writer</summary>
      /// <param name="font">Font name</param>
      /// <param name="size">Font size in points</param>
      /// <param name="cols">List of colours</param>
      RtfWriter::RtfWriter(const wstring& font, UINT size, list<COLORREF> cols) : Font(font), Closed(false), Colour(0xff000000)
      {
         // Copy colours
         for (COLORREF c : cols)
            Colours.push_back(c);

         // Write header
         WriteHeader();
         SetSize(size);
      }

      RtfWriter::~RtfWriter()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Closes the writer</summary>
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

      
      /// <summary>Sets the text colour</summary>
      /// <param name="c">The colour</param>
      /// <exception cref="Logic::ArgumentException">Unrecognised colour</exception>
      /// <exception cref="Logic::InvalidOperationException">Writer has been closed</exception>
      void   RtfWriter::SetColour(COLORREF c)
      {
         CHAR buf[10];

         // Check stream is open
         if (Closed)
            throw InvalidOperationException(HERE, L"Writer is closed");

         // Skip if colour already set
         if (Colour == c)
            return;

         // Lookup colour ID
         for (UINT id = 0; id < Colours.size(); id++)
            if (Colours[id] == c)
            {  // Write colour change
               StringCchPrintfA(buf, 10, "\\cf%d ", id+1);
               Write(buf);
               Colour = c;
               return;
            }

         // Unknown colour
         throw ArgumentException(HERE, L"c", L"Unknown colour");
      }

      /// <summary>Sets the font size</summary>
      /// <param name="points">The size in points</param>
      /// <exception cref="Logic::InvalidOperationException">Writer has been closed</exception>
      void   RtfWriter::SetSize(int points)
      {
         CHAR buf[10];

         // Check stream is open
         if (Closed)
            throw InvalidOperationException(HERE, L"Writer is closed");

         // Set size
         StringCchPrintfA(buf, 10, "\\fs%d ", points*2);
         Write(buf);
      }

      /// <summary>Get ANSI contents of stream</summary>
      /// <returns>Stream contents</returns>
      string RtfWriter::ToString() 
      { 
         return Output; 
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

      /// <summary>Writes ANSI text to the stream verbatim</summary>
      /// <param name="str">The text</param>
      void  RtfWriter::Write(const CHAR* str) 
      { 
         Output += str; 
      }

      /// <summary>Writes RTF header</summary>
      void  RtfWriter::WriteHeader()
      {
         // Header
         Write("{\\rtf1\\ansi\\ansicpg1252\\deff0\\deflang2057");

         // Font table
         Write("{\\fonttbl{\\f0\\fswiss\\fcharset0 "); 
         Write(Font); 
         Write(";}}\n");

         // Colour table
         Write("{\\colortbl;");
         for (COLORREF c : Colours)
            WriteColour(c);
         Write("}\n");

         // Unknown
         // \saN == spacing after
         // \slN == space between lines
         // \smult == Line spacing multiple
         // \pard == reset paragraph formatting
         // \liN == left indent
         Write("\\viewkind4\\uc1\\pard\\li300\\lang9\\cb1 ");   //\\sa200\\sl276\\slmult1
      }

      /// <summary>Writes RTF footer</summary>
      void  RtfWriter::WriteFooter()
      {
         // Unknown
         Write("\\f1\\par\n}");
      }

      /// <summary>Writes an RTF colour table colour definition</summary>
      /// <param name="c">The colour to define</param>
      void  RtfWriter::WriteColour(COLORREF c)
      {
         Write(StringResource::Format(L"\\red%d\\green%d\\blue%d;", GetRValue(c), GetGValue(c), GetBValue(c)));
      }
   }
}