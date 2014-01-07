#include "stdafx.h"
#include "LanguageFileReader.h"

namespace Library
{
   namespace IO
   {
      // -------------------------------- CONSTRUCTION --------------------------------

		// ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Reads the entire language file</summary>
      /// <returns>New language file</returns>
      /// <exception cref="Library::FileFormatException">Missing elements or attributes</exception>
      /// <exception cref="Library::ComException">COM Error</exception>
      /// <exception cref="Library::InvalidDataException">Invalid language ID</exception>
      LanguageFile LanguageFileReader::ReadFile(IO::Stream&  s)
      {
         try
         {
            // Extract stream
            ByteArrayPtr buffer(s.ReadAllBytes());

            // Create DOM parser
            XML::IXMLDOMDocument2Ptr pDoc(CLSID_DOMDocument60);
            pDoc->async = VARIANT_FALSE;

            // Load/Parse file : "%s (line %d, char %d)"
            if (pDoc->loadXML((char*)buffer.get()) == VARIANT_FALSE)
               throw FileFormatException(HERE, ERR_XML_PARSE_FAILED, (WCHAR*)pDoc->parseError->reason, pDoc->parseError->line, pDoc->parseError->linepos);

            // Get root (as node)
            XML::IXMLDOMNodePtr languageNode(pDoc->documentElement);

            // Read language tag
            LanguageFile file;
            file.Language = ReadLanguageTag(languageNode);

            // Read pages
            for (int i = 0; i < languageNode->childNodes->length; i++)
            {
               file.Pages.push_back(ReadPage(languageNode->childNodes->item[i]));
            }

            return file;
         }
         catch (_com_error& ex)
         {
            throw ComException(HERE, ex);
         }
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

      /// <summary>Reads the value of an attribute</summary>
      /// <param name="node">The element containing the attribute</param>
      /// <param name="name">The attribute name</param>
      /// <returns>Attribute value</returns>
      /// <exception cref="Library::FileFormatException">Missing attribute</exception>
      /// <exception cref="_com_error">COM error</exception>
      wstring  LanguageFileReader::ReadAttribute(XML::IXMLDOMNodePtr&  node, const WCHAR*  name)
      {
         REQUIRED(node);

         XML::IXMLDOMNodePtr attr = node->attributes->getNamedItem(name);

         // Ensure present : "Missing '%s' attribute on '<%s>' element"
         if (attr == NULL)
            throw FileFormatException(HERE, ERR_XML_MISSING_ATTRIBUTE, name, (WCHAR*)node->nodeName);

         // Return text
         return (WCHAR*)attr->text;
      }

      /// <summary>Verifies the name of an element</summary>
      /// <param name="node">The element node</param>
      /// <param name="name">Name of the element</param>
      /// <exception cref="Library::FileFormatException">Missing element</exception>
      /// <exception cref="_com_error">COM error</exception>
      void  LanguageFileReader::ReadElement(XML::IXMLDOMNodePtr&  node, const WCHAR*  name)
      {
         REQUIRED(node);

         // Ensure name correct : "Unexpected '<%s>' element while searching for '<%s>' element"
         if (node->nodeName != _bstr_t(name))
            throw FileFormatException(HERE, ERR_XML_UNEXPECTED_ELEMENT, (WCHAR*)node->nodeName, name);
      }

      /// <summary>Reads the language tag and parses the ID</summary>
      /// <param name="element">The language element</param>
      /// <returns>Language ID</returns>
      /// <exception cref="Library::FileFormatException">Missing language element</exception>
      /// <exception cref="Library::InvalidDataException">Invalid language ID</exception>
      /// <exception cref="_com_error">COM error</exception>
      GameLanguage  LanguageFileReader::ReadLanguageTag(XML::IXMLDOMNodePtr&  element)
      {
         // Ensure present: "Missing '%s' element"
         if (element == NULL)
            throw FileFormatException(HERE, ERR_XML_MISSING_ELEMENT, L"language");

         // Verify tag
         ReadElement(element, L"language");

         // Convert language ID
         return GameLanguageReader::Parse(ReadAttribute(element, L"id"));
      }

      /// <summary>Reads an optional attribute</summary>
      /// <param name="node">The element containing the attribute</param>
      /// <param name="name">Name of attribute</param>
      /// <returns>attribute value, or empty string</returns>
      /// <exception cref="_com_error">COM error</exception>
      wstring  LanguageFileReader::TryReadAttribute(XML::IXMLDOMNodePtr&  node, const WCHAR*  name)
      {
         // Read attribute text or return empty string
         XML::IXMLDOMNodePtr attr = node->attributes->getNamedItem(name);
         return attr == NULL ? L"" : (WCHAR*)attr->text;
      }

      /// <summary>Reads a page tag and all it's string tags</summary>
      /// <param name="element">Page element</param>
      /// <returns>New language page</returns>
      /// <exception cref="Library::FileFormatException">Missing element or attributes</exception>
      /// <exception cref="_com_error">COM error</exception>
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
      /// <exception cref="_com_error">COM error</exception>
      LanguageString  LanguageFileReader::ReadString(XML::IXMLDOMNodePtr&  element)
      {
         // Verify string tag
         ReadElement(element, L"t");

         // Read ID+text
         return LanguageString(_wtoi(ReadAttribute(element, L"id").c_str()), (WCHAR*)element->text);
      }
   }
}