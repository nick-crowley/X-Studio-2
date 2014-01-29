#include "stdafx.h"
#include "StringReader.h"
#include "FileStream.h"

namespace Logic
{
   namespace IO
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Creates a string reader from an input stream</summary>
      /// <param name="src">The input stream</param>
      /// <exception cref="Logic::ArgumentException">Stream is not readable</exception>
      /// <exception cref="Logic::ArgumentNullException">Stream is null</exception>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      StringReader::StringReader(StreamPtr src) : Input(src), Position(0), Buffer(nullptr), LineNum(1)
      {
         REQUIRED(src);

         // Ensure stream has read access
         if (!Input->CanRead())
            throw ArgumentException(HERE, L"src", GuiString(ERR_NO_READ_ACCESS));

         // Lookup stream length
         Length = Input->GetLength();
      }

      /// <summary>Move-copy from an existing string reader</summary>
      /// <param name="r">The existing reader</param>
      StringReader::StringReader(StringReader&& r)
         : Length(r.Length), Position(r.Position), Buffer(std::move(r.Buffer)), Input(std::move(r.Input)), LineNum(r.LineNum)
      {
      }

      /// <summary>Closes the input stream</summary>
      StringReader::~StringReader()
      {
         if (Input != nullptr)      // Ensure we haven't been moved
            Input->SafeClose();
      }

      // ------------------------------- STATIC  METHODS ------------------------------
      
      // ------------------------------- PUBLIC METHODS -------------------------------
      
      /// <summary>Reads the next character</summary>
      /// <param name="c">Next character</param>
      /// <returns>True if read, false if EOF</returns>
      /// <exception cref="Logic::InvalidOperationException">Stream has been closed (reader has been move-copied)</exception>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      bool  StringReader::ReadChar(WCHAR&  c)
      {
         // Ensure stream has not been moved
         if (Input == nullptr)
            throw InvalidOperationException(HERE, L"Underlying stream has been closed");

         // First Call: Read entire file 
         else if (Buffer == nullptr)
            Buffer = FileStream::ConvertFileBuffer(Input, Length);

         // EOF: Return false
         if (IsEOF())
         {
            c = NULL;
            return false;
         }

         // Increment line number 
         if (c == L'\n')
            LineNum++;

         // Set byte, advance position, return true
         c = Buffer.get()[Position++];
         return true;
      }

      /// <summary>Reads the next line, if any</summary>
      /// <param name="line">The line.</param>
      /// <returns>True if read, false if EOF</returns>
      /// <exception cref="Logic::InvalidOperationException">Stream has been closed (reader has been move-copied)</exception>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      bool  StringReader::ReadLine(wstring&  line)
      {
         DWORD start = Position,    // Start of line
               end   = Length;      // End of characters on line
         WCHAR ch;

         // EOF: Return false
         if (IsEOF())
         {
            line.clear();
            return false;
         }

         // Search for EOF/CRLF/CR
         while (ReadChar(ch))
         {
            // CR/LF/CRLF: Set of chars marker, 
            if (ch == '\r' || ch == '\n')
            {
               // Mark end-of-text
               end = Position-1;

               // Consume entire CRLF if present
               if (ch == '\r' && PeekChar(ch) && ch == '\n')
                  ReadChar(ch);

               // Position marker now at start of new line, end-of-char marker at last character
               break;
            }
         }

         // Return line text without CRLF
         line = wstring(Buffer.get() + start, Buffer.get() + end);
         return true;
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      /// <summary>Peeks the next character</summary>
      /// <param name="c">Next character</param>
      /// <returns>True if read, false if EOF</returns>
      /// <exception cref="Logic::InvalidOperationException">Stream has been closed (reader has been move-copied)</exception>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      bool  StringReader::PeekChar(WCHAR&  c)
      {
         // Ensure stream has not been moved
         if (Input == nullptr)
            throw InvalidOperationException(HERE, L"Underlying stream has been closed");
         
         // First Call: Read entire file 
         else if (Buffer == nullptr)
            Buffer = FileStream::ConvertFileBuffer(Input, Length);

         // EOF: Return false
         if (IsEOF())
         {
            c = NULL;
            return false;
         }

         // Set byte, return true
         c = Buffer.get()[Position];
         return true;
      }

		// ------------------------------- PRIVATE METHODS ------------------------------

   }
}