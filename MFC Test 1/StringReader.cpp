#include "stdafx.h"
#include "StringReader.h"

namespace Library
{
   namespace IO
   {

      StringReader::StringReader(Stream* s, bool owner) : Input(s), Position(0), Buffer(nullptr), DestroyOnClose(owner)
      {
         REQUIRED(s);

         if (!s->CanRead())
            throw ArgumentException(HERE, L"s", ERR_NO_READ_ACCESS);

         Length = s->GetLength();
      }


      StringReader::~StringReader()
      {
         if (DestroyOnClose)
         {
            delete Input;
            Input = nullptr;
         }
      }


      bool  StringReader::ReadLine(string&  line)
      {
         DWORD start = Position,    // Start of line
               end   = Length;      // End of characters on line
         BYTE  ch;

         // Read entire file on first call
         if (Buffer == nullptr)
            Buffer.reset(Input->ReadAllBytes());

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