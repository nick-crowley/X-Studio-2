#include "stdafx.h"
#include "LanguageFileReader.h"

namespace Library
{
   /// <summary>Reads the value of an attribute</summary>
   /// <param name="node">The element containing the attribute</param>
   /// <param name="name">The attribute name</param>
   /// <returns>Attribute value</returns>
   /// <exception cref="Library.FileFormatException">Missing attribute</exception>
   /// <exception cref="_com_error">COM error</exception>
   wstring  LanguageFileReader::ReadAttribute(XML::IXMLDOMNodePtr&  node, const WCHAR*  name)
   {
      auto attr = node->attributes->getNamedItem(name);

      // Ensure present : "Missing '%s' attribute on '<%s>' element"
      if (attr == NULL)
         throw FileFormatException(HERE, ERR_XML_MISSING_ATTRIBUTE, name, (WCHAR*)node->nodeName);

      // Return text
      return (WCHAR*)attr->text;
   }

   /// <summary>Verifies the name of an element</summary>
   /// <param name="node">The element node</param>
   /// <param name="name">Name of the element</param>
   /// <exception cref="Library.FileFormatException">Missing element</exception>
   /// <exception cref="_com_error">COM error</exception>
   void  LanguageFileReader::ReadElement(XML::IXMLDOMNodePtr&  node, const WCHAR*  name)
   {
      // Ensure present : "Missing '<%s>' element"
      if (node == NULL)
         throw FileFormatException(HERE, ERR_XML_MISSING_NODE, name);

      // Ensure name correct : "Unexpected '<%s>' element while searching for '<%s>' element"
      if (node == NULL || node->nodeName != _bstr_t(name))
         throw FileFormatException(HERE, ERR_XML_UNEXPECTED_NODE, (WCHAR*)node->nodeName, name);
   }

   /// <summary>Reads the language tag and parses the ID</summary>
   /// <param name="element">The language element</param>
   /// <returns>Language ID</returns>
   /// <exception cref="Library.FileFormatException">Missing language element</exception>
   /// <exception cref="Library.InvalidDataException">Invalid language ID</exception>
   /// <exception cref="_com_error">COM error</exception>
   GameLanguage  LanguageFileReader::ReadLanguageTag(XML::IXMLDOMNodePtr&  element)
   {
      // Verify tag
      ReadElement(element, L"language");
      
      // Convert language ID
      return GameLanguageReader::Parse( ReadAttribute(element, L"id") );
   }

   /// <summary>Reads an optional attribute</summary>
   /// <param name="node">The element containing the attribute</param>
   /// <param name="name">Name of attribute</param>
   /// <returns>attribute value, or empty string</returns>
   /// <exception cref="_com_error">COM error</exception>
   wstring  LanguageFileReader::TryReadAttribute(XML::IXMLDOMNodePtr&  node, const WCHAR*  name)
   {
      // Read attribute text or return empty string
      auto attr = node->attributes->getNamedItem(name);
      if (attr == NULL)
         return L"";
      
      auto text = attr->text;
      return (WCHAR*)attr->text;
   }

   /// <summary>Reads a page tag and all it's string tags</summary>
   /// <param name="element">Page element</param>
   /// <returns>New language page</returns>
   /// <exception cref="Library.FileFormatException">Missing element or attributes</exception>
   /// <exception cref="_com_error">COM error</exception>
   LanguagePage  LanguageFileReader::ReadPage(XML::IXMLDOMNodePtr&  element)
   {
      // Verify page tag
      ReadElement(element, L"page");

      // Read properties
      auto id = ReadAttribute(element, L"id");
      auto title = TryReadAttribute(element, L"title");
      auto desc = TryReadAttribute(element, L"desc");
      auto voice = TryReadAttribute(element, L"voice") == L"yes";

      // Create page
      LanguagePage page(_wtoi(id.c_str()), title, desc, voice);

      // Read strings
      for (int i = 0; i < element->childNodes->length; i++)
         page.Strings.insert( ReadString(element->childNodes->item[i]) );

      // Return page
      return page;
   }

   /// <summary>Reads a string tag</summary>
   /// <param name="element">String 't' element</param>
   /// <returns>New language string</returns>
   /// <exception cref="Library.FileFormatException">Missing element or attributes</exception>
   /// <exception cref="_com_error">COM error</exception>
   LanguageString  LanguageFileReader::ReadString(XML::IXMLDOMNodePtr&  element)
   {
      // Verify string tag
      ReadElement(element, L"t");

      // Read ID+text
      auto id = ReadAttribute(element, L"id");
      return LanguageString(_wtoi(id.c_str()), (WCHAR*)element->text);
   }

   /// <summary>Reads the entire language file</summary>
   /// <returns>New language file</returns>
   /// <exception cref="Library.FileFormatException">Missing elements or attributes</exception>
   /// <exception cref="Library.ComException">COM Error</exception>
   /// <exception cref="Library.InvalidDataException">Invalid language ID</exception>
   LanguageFile LanguageFileReader::ReadFile()
   {
      try
      {
         LanguageFile file;

         // Ensure file exists: "file could not be found"
         if (!FullPath.Exists())
            throw IOException(HERE, ERR_FILE_NOT_FOUND);

         // Create DOM parser
         XML::IXMLDOMDocument2Ptr pDoc(CLSID_DOMDocument60);
         pDoc->async = VARIANT_FALSE;

         // Load/Parse file : "%s (line %d, char %d)"
         if (pDoc->load(FullPath.c_str()) == VARIANT_FALSE)    
            throw FileFormatException(HERE, ERR_XML_PARSE_FAILED, (WCHAR*)pDoc->parseError->reason, pDoc->parseError->line, pDoc->parseError->linepos);

         // Read language tag
         XML::IXMLDOMNodePtr languageNode(pDoc->documentElement);
         file.Language = ReadLanguageTag(languageNode);
         
         // Read pages
         for (int i = 0; i < languageNode->childNodes->length; i++)
         {
            file.Pages.push_back( ReadPage(languageNode->childNodes->item[i]) );
         }

         return file;
      } 
      catch (_com_error& ex)
      {
         throw ComException(HERE, ex);
      }
   }

}