#include "stdafx.h"
#include "LanguageFileReader.h"

namespace Library
{
   namespace IO
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Creates an xml reader from an input stream</summary>
      /// <param name="src">The input stream</param>
      /// <exception cref="Library::ArgumentException">Stream is not readable</exception>
      /// <exception cref="Library::ArgumentNullException">Stream is null</exception>
      /// <exception cref="Library::ComException">COM Error</exception>
      LanguageFileReader::LanguageFileReader(StreamPtr in) : XmlReader(in)
      {
      }

      LanguageFileReader::~LanguageFileReader()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      /// <summary>Parses the language ID of a language file</summary>
      /// <param name="id">The language identifier</param>
      /// <returns>Game language</returns>
      /// <exception cref="Library::InvalidValueException">Invalid language ID</exception>
      GameLanguage  LanguageFileReader::ParseLanguageID(wstring id)
      {
         switch (_wtoi(id.c_str()))
         {
         case 44:
         case 48:  
            return (GameLanguage)_wtoi(id.c_str()); 

         default:
            // "'%s' is not a valid language ID"
            throw InvalidValueException(HERE, ERR_LANGUAGE_ID_INVALID, id.c_str());
         }
      }

		// ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Reads the entire language file</summary>
      /// <returns>New language file</returns>
      /// <exception cref="Library::ComException">COM Error</exception>
      /// <exception cref="Library::FileFormatException">Corrupt XML / Missing elements / missing attributes</exception>
      /// <exception cref="Library::InvalidDataException">Invalid language ID</exception>
      LanguageFile LanguageFileReader::ReadFile()
      {
         LanguageFile file;

         try
         {
            // Parse document
            LoadDocument();

            // Get root (as node)
            XML::IXMLDOMNodePtr languageNode(Document->documentElement);

            // Read language tag
            file.Language = ReadLanguageTag(languageNode);

            // Read pages
            for (int i = 0; i < languageNode->childNodes->length; i++)
            {
               file.Pages.push_back( ReadPage(languageNode->childNodes->item[i]) );
            }

            return file;
         }
         catch (_com_error& ex) {
            throw ComException(HERE, ex);
         }
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

      /// <summary>Reads the language tag and parses the ID</summary>
      /// <param name="element">The language element</param>
      /// <returns>Language ID</returns>
      /// <exception cref="Library::FileFormatException">Missing language element</exception>
      /// <exception cref="Library::InvalidValueException">Invalid language ID</exception>
      /// <exception cref="Library::ComException">COM Error</exception>
      GameLanguage  LanguageFileReader::ReadLanguageTag(XML::IXMLDOMNodePtr&  element)
      {
         // Ensure present: "Missing '%s' element"
         if (element == nullptr)
            throw FileFormatException(HERE, ERR_XML_MISSING_ELEMENT, L"language");

         // Verify tag
         ReadElement(element, L"language");

         // Convert language ID
         return ParseLanguageID(ReadAttribute(element, L"id"));
      }

      /// <summary>Reads a page tag and all it's string tags</summary>
      /// <param name="element">Page element</param>
      /// <returns>New language page</returns>
      /// <exception cref="Library::FileFormatException">Missing element or attributes</exception>
      /// <exception cref="Library::ComException">COM Error</exception>
      LanguagePage  LanguageFileReader::ReadPage(XML::IXMLDOMNodePtr&  element)
      {
         // Verify page tag
         ReadElement(element, L"page");

         // Read properties
         wstring id(ReadAttribute(element, L"id")),
                 title(TryReadAttribute(element, L"title")),
                 desc(TryReadAttribute(element, L"desc"));
         bool    voice = TryReadAttribute(element, L"voice") == L"yes";

         // Create page
         LanguagePage page(_wtoi(id.c_str()), title, desc, voice);

         // Read strings
         for (int i = 0; i < element->childNodes->length; i++)
            page.Strings.insert(ReadString(element->childNodes->item[i]));

         // Return page
         return page;
      }

      /// <summary>Reads a string tag</summary>
      /// <param name="element">String 't' element</param>
      /// <returns>New language string</returns>
      /// <exception cref="Library::FileFormatException">Missing element or attributes</exception>
      /// <exception cref="Library::ComException">COM Error</exception>
      LanguageString  LanguageFileReader::ReadString(XML::IXMLDOMNodePtr&  element)
      {
         // Verify string tag
         ReadElement(element, L"t");

         // Read ID+text
         return LanguageString(_wtoi(ReadAttribute(element, L"id").c_str()), (WCHAR*)element->text);
      }
   }
}