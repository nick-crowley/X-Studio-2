#pragma once
#include "Common.h"
#include "LanguageFile.h"
#include "msxml.h"
#include "Stream.h"

namespace Library
{
   namespace IO
   {

      class LanguageFileReader
      {
      public:
         LanguageFileReader() {};
         ~LanguageFileReader() {};

         LanguageFile   ReadFile(IO::Stream&  s);

      private:
      
         void           ReadElement(XML::IXMLDOMNodePtr&  node, const WCHAR*  name);
         wstring        ReadAttribute(XML::IXMLDOMNodePtr&  node, const WCHAR*  name);
         GameLanguage   ReadLanguageTag(XML::IXMLDOMNodePtr&  element);
         wstring        TryReadAttribute(XML::IXMLDOMNodePtr&  node, const WCHAR*  name);

         LanguagePage   ReadPage(XML::IXMLDOMNodePtr&  element);
         LanguageString ReadString(XML::IXMLDOMNodePtr&  element);
      };
   }
}