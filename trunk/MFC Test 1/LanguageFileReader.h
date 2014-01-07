#pragma once
#include "Common.h"
#include "LanguageFile.h"
#include "msxml.h"
#include "Stream.h"

using namespace Library::Language;

namespace Library
{
   namespace IO
   {
      /// <summary>Reads strings and pages of an X3 language xml file</summary>
      class LanguageFileReader
      {
         // --------------------- CONSTRUCTION ----------------------

      public:
         LanguageFileReader() {};
         ~LanguageFileReader() {};

         // --------------------- PROPERTIES ------------------------
			
			// ---------------------- ACCESSORS ------------------------

			// ----------------------- MUTATORS ------------------------

         LanguageFile   ReadFile(IO::Stream&  s);

      private:
         void           ReadElement(XML::IXMLDOMNodePtr&  node, const WCHAR*  name);
         wstring        ReadAttribute(XML::IXMLDOMNodePtr&  node, const WCHAR*  name);
         GameLanguage   ReadLanguageTag(XML::IXMLDOMNodePtr&  element);
         wstring        TryReadAttribute(XML::IXMLDOMNodePtr&  node, const WCHAR*  name);

         LanguagePage   ReadPage(XML::IXMLDOMNodePtr&  element);
         LanguageString ReadString(XML::IXMLDOMNodePtr&  element);

         // -------------------- REPRESENTATION ---------------------
      };
   }
}