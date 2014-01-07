#include "stdafx.h"
#include "StringReader.h"

namespace Library
{
   namespace IO
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Creates a string reader from an input stream</summary>
      /// <param name="src">The input stream</param>
      /// <exception cref="Library.ArgumentException">Stream is not readable</exception>
      /// <exception cref="Library.ArgumentNullException">Stream is null</exception>
      StringReader::StringReader(StreamPtr src) : Input(src), Position(0), Buffer(nullptr)
      {
         REQUIRED(src);

         // Ensure stream has read access
         if (!Input->CanRead())
            throw ArgumentException(HERE, L"src", ERR_NO_READ_ACCESS);

         // Lookup stream length
         Length = Input->GetLength();
      }

      /// <summary>Move-copy from an existing string reader</summary>
      /// <param name="r">The existing reader</param>
      StringReader::StringReader(StringReader&& r)
         : Length(r.Length), Position(r.Position), Buffer(std::move(r.Buffer)), Input(std::move(r.Input))
      {
      }

      /// <summary>Closes the input stream</summary>
      StringReader::~StringReader()
      {
         if (Input != nullptr)
            Input->SafeClose();
      }

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Reads the next line, if any</summary>
      /// <param name="line">The line.</param>
      /// <returns>True if read, false if EOF</returns>
      /// <exception cref="Library.InvalidOperationException">Stream has been closed (reader has been move-copied)</exception>
      bool  StringReader::ReadLine(string&  line)
      {
         DWORD start = Position,    // Start of line
               end   = Length;      // End of characters on line
         BYTE  ch;

         // Ensure stream has not been moved
         if (Input == nullptr)
            throw InvalidOperationException(HERE, L"Underlying stream has been closed");

         // Read entire file on first call
         if (Buffer == nullptr)
            Buffer = Input->ReadAllBytes();

         // EOF: Return false
         if (Position >= Length)
         {
            line.clear();
            return false;
         }

         // Search for EOF/CRLF/CR
         while (ReadByte(ch))
         {
            // CR/LF/CRLF: Set of chars marker, 
            if (ch == '\r' || ch == '\n')
            {
               // Mark end-of-text
               end = Position-1;

               // Consume entire CRLF if present
               if (ch == '\r' && PeekByte(ch) && ch == '\n')
                  ReadByte(ch);

               // Position marker now at start of new line, end-of-char marker at last character
               break;
            }
         }

         // Return line text without CRLF
         line = string(Buffer.get() + start, Buffer.get() + end);
         return true;
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------
      
      /// <summary>Reads the next byte.</summary>
      /// <param name="b">Next byte</param>
      /// <returns>True if read, false if EOF</returns>
      bool  StringReader::ReadByte(BYTE&  b)
      {
         // EOF: Return false
         if (Position >= Length)
         {
            b = NULL;
            return false;
         }

         // Set byte, advance position, return true
         b = Buffer.get()[Position++];
         return true;
      }

      /// <summary>Peeks the next byte.</summary>
      /// <param name="b">Next byte</param>
      /// <returns>True if read, false if EOF</returns>
      bool  StringReader::PeekByte(BYTE&  b)
      {
         // EOF: Return false
         if (Position >= Length)
         {
            b = NULL;
            return false;
         }

         // Set byte, return true
         b = Buffer.get()[Position];
         return true;
      }


   }
}