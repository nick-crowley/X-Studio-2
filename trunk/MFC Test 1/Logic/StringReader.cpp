#include "stdafx.h"
#include "StringReader.h"

namespace Logic
{
   namespace IO
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Creates a string reader from an input stream</summary>
      /// <param name="src">The input stream</param>
      /// <exception cref="Logic::ArgumentException">Stream is not readable</exception>
      /// <exception cref="Logic::ArgumentNullException">Stream is null</exception>
      StringReader::StringReader(StreamPtr src) : Input(src), Position(0), Buffer(nullptr), LineNum(0)
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
      
      /// <summary>Converts a text file into a WCHAR buffer based on the byte ordering mark, if any</summary>
      /// <param name="s">The input stream</param>
      /// <param name="length">Stream length on input, character length on output</param>
      /// <returns>Wide char buffer</returns>
      /// <exception cref="Logic::IOException">Conversion failed</exception>
      CharArrayPtr  StringReader::ConvertFileBuffer(StreamPtr  s, DWORD&  Length)
      {
         BYTE         utf8[3] = { 0xEF, 0xBB, 0xBF },    // UTF-8 byte ordering header
                      utf16[2] = { 0xFF, 0xFE };         // UTF-16 byte ordering header
         UINT         newLength;
         CharArrayPtr output;

         // Read entire file
         auto input = s->ReadAllBytes();

         // UTF16: Convert byte array -> wchar array
         if (Length > 2 && memcmp(input.get(), utf16, 2) == 0)
         {
            // Conv byte length -> char length  (minus 2byte BOM)
            newLength = (Length -= 2) / 2;

            // Copy exluding 2 byte BOM
            output = CharArrayPtr(new WCHAR[newLength+1]);
            memcpy(output.get(), input.get()+2, Length);
         }
         // UTF8: Convert UTF8 to UTF16
         if (Length > 3 && memcmp(input.get(), utf8, 3) == 0)
         {
            // Calculate UTF16 length (minus 3byte BOM)
            newLength = 2 * (Length -= 3);

            // Copy excluding 3 byte BOM
            output = CharArrayPtr(new WCHAR[newLength+1]);
            newLength = MultiByteToWideChar(CP_UTF8, NULL, (char*)input.get()+3, Length, output.get(), newLength);
         }
         // ASCII: Convert ASCII to UTF16
         else
         {
            // Simple double in length
            newLength = 2*Length;
            output = CharArrayPtr(new WCHAR[newLength+1]);
            newLength = MultiByteToWideChar(1250, NULL, (char*)input.get(), Length, output.get(), newLength);
         }

         // Failed: Throw
         if (Length > 0 && (Length=newLength) == 0)
            throw IOException(HERE, SysErrorString());

         // Null terminate + return
         output.get()[newLength] = NULL;
         return output;
      }


      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Reads the next line, if any</summary>
      /// <param name="line">The line.</param>
      /// <returns>True if read, false if EOF</returns>
      /// <exception cref="Logic::InvalidOperationException">Stream has been closed (reader has been move-copied)</exception>
      bool  StringReader::ReadLine(wstring&  line)
      {
         DWORD start = Position,    // Start of line
               end   = Length;      // End of characters on line
         WCHAR ch;

         // Increment line number before start (ensures 1-based)
         LineNum++;

         // Ensure stream has not been moved
         if (Input == nullptr)
            throw InvalidOperationException(HERE, L"Underlying stream has been closed");

         // Read entire file on first call
         if (Buffer == nullptr)
            Buffer = ConvertFileBuffer(Input, Length);

         // EOF: Return false
         if (Position >= Length)
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

		// ------------------------------- PRIVATE METHODS ------------------------------

      
      /// <summary>Reads the next character</summary>
      /// <param name="c">Next character</param>
      /// <returns>True if read, false if EOF</returns>
      bool  StringReader::ReadChar(WCHAR&  c)
      {
         // EOF: Return false
         if (Position >= Length)
         {
            c = NULL;
            return false;
         }

         // Set byte, advance position, return true
         c = Buffer.get()[Position++];
         return true;
      }

      /// <summary>Peeks the next character</summary>
      /// <param name="c">Next character</param>
      /// <returns>True if read, false if EOF</returns>
      bool  StringReader::PeekChar(WCHAR&  c)
      {
         // EOF: Return false
         if (Position >= Length)
         {
            c = NULL;
            return false;
         }

         // Set byte, return true
         c = Buffer.get()[Position];
         return true;
      }


   }
}