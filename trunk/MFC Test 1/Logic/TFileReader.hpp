#pragma once

#include "Common.h"
#include "StringReader.h"
#include "TObject.h"
#include "TFile.hpp"

namespace Logic
{
   namespace IO
   {
   
      /// <summary></summary>
      template <typename OBJ>
      class TFileReader : protected StringReader
      {
		 // ------------------------ TYPES --------------------------
      private:
	  
         // --------------------- CONSTRUCTION ----------------------

      public:
         /// <summary>Create a TFile reader from an input stream</summary>
         /// <param name="in">The input stream</param>
         /// <exception cref="Logic::ArgumentException">Stream is not readable</exception>
         /// <exception cref="Logic::ArgumentNullException">Stream is null</exception>
         /// <exception cref="Logic::IOException">An I/O error occurred</exception>
         TFileReader(StreamPtr in) : StringReader(in)
         {}
         virtual ~TFileReader()
         {}
		 
		   NO_COPY(TFileReader);	// No copy semantics
		   NO_MOVE(TFileReader);	// No move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
			
         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------

      public:
         /// <summary>Reads all objects in the file</summary>
         /// <returns></returns>
         /// <exception cref="Logic::FileFormatException">File contains a syntax error</exception>
         /// <exception cref="Logic::IOException">An I/O error occurred</exception>
         TFile<OBJ>  ReadFile()
         {
            // Skip comments
            while (ReadComment())
            {}

            // Parse header
            int  ver   = ReadInt(L"File version");
            int  count = ReadInt(L"Number of objects");

            // TODO: Validate version

            // Parse objects
            TFile<OBJ> file(count);
            for (int i = 0; i < count; ++i)
            {
               OBJ obj;

               // Read header/contents/footer
               ReadHeader(obj);
               ReadObject(obj, GameVersion::TerranConflict);
               ReadFooter(obj);

               // Add to file
               file.Objects.push_back(obj);
            }

            return file;
         }

      protected:
         /// <summary>Reads the next float</summary>
         /// <param name="field">The name of the field</param>
         /// <returns>Value if present.  If missing or End-of-line, zero is returned</returns>
         /// <exception cref="Logic::ArgumentNullException">Field name is nullptr</exception>
         /// <exception cref="Logic::FileFormatException">Value is not decimal -or- unexpected EOF</exception>
         /// <exception cref="Logic::IOException">An I/O error occurred</exception>
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

         /// <summary>Reads the common properties at the end</summary>
         /// <param name="o">Object to read into</param>
         /// <exception cref="Logic::FileFormatException">File contains a syntax error</exception>
         /// <exception cref="Logic::IOException">An I/O error occurred</exception>
	      void  ReadFooter(OBJ& o)
	      {
		      o.volume              = ReadInt(L"volume");
		      o.relativeValue       = ReadInt(L"relativeValue");
		      o.priceModifier1      = ReadInt(L"priceModifier1");
		      o.priceModifier2      = ReadInt(L"priceModifier2");
		      o.size                = (WareSize)ReadInt(L"size");
		      o.relativeValuePlayer = ReadInt(L"relativeValuePlayer");
		      o.minimumNotoriety    = ReadInt(L"minimumNotoriety");
		      o.videoID             = ReadInt(L"videoID");
		      o.skinIndex           = ReadInt(L"skinIndex");
		      o.id                  = ReadString(L"id");
	      }
      
         /// <summary>Reads the common properties at the start</summary>
         /// <param name="o">Object to read into</param>
         /// <exception cref="Logic::FileFormatException">File contains a syntax error</exception>
         /// <exception cref="Logic::IOException">An I/O error occurred</exception>
	      void  ReadHeader(OBJ& o)
	      {
		      o.bodyFile   = ReadString(L"bodyFile");
		      o.pictureID  = ReadInt(L"pictureID");
		      o.rotation.x = ReadFloat(L"rotation.x");
		      o.rotation.y = ReadFloat(L"rotation.y");
		      o.rotation.z = ReadFloat(L"rotation.z");

		      o.subtype    = ReadString(L"subtype");		   
            o.name       = LookupString(ReadInt(L"name"), KnownPage::NAMES_DESCRIPTIONS);
	      }

         /// <summary>Reads the properties unique to this type</summary>
         /// <param name="o">Object to read into</param>
         /// <param name="ver">File version</param>
         /// <exception cref="Logic::FileFormatException">File contains a syntax error</exception>
         /// <exception cref="Logic::IOException">An I/O error occurred</exception>
	      virtual void  ReadObject(OBJ& o, GameVersion ver) PURE;

         /// <summary>Reads the next int</summary>
         /// <param name="field">The name of the field</param>
         /// <returns>Value if present.  If missing or End-of-line, zero is returned</returns>
         /// <exception cref="Logic::ArgumentNullException">Field name is nullptr</exception>
         /// <exception cref="Logic::FileFormatException">Value is not numeric -or- unexpected EOF</exception>
         /// <exception cref="Logic::IOException">An I/O error occurred</exception>
         int  ReadInt(const WCHAR* field)
         {
            REQUIRED(field);

            // Read as string
            wstring sz(ReadString(field));
         
            // Ensure numeric
            for (WCHAR& ch : sz)
               if (!iswdigit(ch) && ch != L'-')
                  throw FileFormatException(HERE, LineNumber, GuiString(L"%s is not an integer : '%s'", field, sz.c_str()));

            // Conver to int
            return _wtoi(sz.c_str());
         }

         /// <summary>Reads the next float</summary>
         /// <param name="field">The name of the field</param>
         /// <returns>Value if present. If missing or End-of-line, an empty string is returned</returns>
         /// <exception cref="Logic::ArgumentNullException">Field name is nullptr</exception>
         /// <exception cref="Logic::FileFormatException">Unexpected EOF</exception>
         /// <exception cref="Logic::IOException">An I/O error occurred</exception>
         wstring  ReadString(const WCHAR* field)
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

      private:
         /// <summary>Skips the comment, if any, on this line</summary>
         /// <returns>True if comment was present, false otherwise</returns>
         bool ReadComment()
         {
            WCHAR ch;
            wstring line;

            // Line starting with '/' indicate a comment
            if (PeekChar(ch) && ch == L'/')
            {
               ReadLine(line);
               return true;
            }

            return false;
         }

         // -------------------- REPRESENTATION ---------------------

      private:
         
      };
   
   }
}

using namespace Logic::IO;
