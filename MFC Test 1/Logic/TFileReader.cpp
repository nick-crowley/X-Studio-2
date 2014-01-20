#include "stdafx.h"
#include "TFileReader.h"

namespace Logic
{
   namespace IO
   {
   
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Create a TFile reader from an input stream</summary>
      /// <param name="in">The input stream</param>
      TFileReader::TFileReader(StreamPtr in) : StringReader(in)
      {
      }


      TFileReader::~TFileReader()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Reads the next int</summary>
      /// <param name="field">The name of the field</param>
      /// <returns>Value if present.  If missing or End-of-line, zero is returned</returns>
      /// <exception cref="Logic::FileFormatException">Value is not numeric -or- unexpected EOF</exception>
      int  TFileReader::ReadInt(const WCHAR* field)
      {
         REQUIRED(field);

         // Read as string
         wstring sz(ReadString(field));
         
         // Ensure numeric
         for (WCHAR& ch : sz)
            if (!iswdigit(ch))
               throw FileFormatException(HERE, LineNumber, GuiString(L"%s is not an integer : '%s'", field, sz.c_str()));

         // Conver to int
         return _wtoi(sz.c_str());
      }

      /// <summary>Reads the next float</summary>
      /// <param name="field">The name of the field</param>
      /// <returns>Value if present.  If missing or End-of-line, zero is returned</returns>
      /// <exception cref="Logic::FileFormatException">Value is not decimal -or- unexpected EOF</exception>
      float  TFileReader::ReadFloat(const WCHAR* field)
      {
         REQUIRED(field);

         // Read as string
         wstring sz(ReadString(field));
         
         // Ensure decimal
         for (WCHAR& ch : sz)
            if (!iswdigit(ch) && ch != L'.' && ch != L'+' && ch != L'-')
               throw FileFormatException(HERE, LineNumber, GuiString(L"%s is not a decimal : '%s'", field, sz.c_str()));

         // Conver to float
         return (float)_wtof(sz.c_str());
      }

      /// <summary>Reads the next float</summary>
      /// <param name="field">The name of the field</param>
      /// <returns>Value if present. If missing or End-of-line, an empty string is returned</returns>
      /// <exception cref="Logic::FileFormatException">Unexpected EOF</exception>
      wstring  TFileReader::ReadString(const WCHAR* field)
      {
         wstring sz;
         WCHAR   ch;

         // Ensure not EOF
         if (IsEOF())
            throw FileFormatException(HERE, GuiString(L"Unexpected end-of-file while searching for %s", field));

         // Read each char
         while (ReadChar(ch))
         {
            switch (ch)
            {
            // CR: Read entire CRLF
            case '\r':
               if (PeekChar(ch) && ch == L'\n')
                  ReadChar(ch);
               break;

            // LF/Semicolon: End
            case '\n':
            case ';':   
               return sz;
            
            default: 
               sz.push_back(ch); 
               break;
            }
         }

         // Return value
         return sz;
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}

