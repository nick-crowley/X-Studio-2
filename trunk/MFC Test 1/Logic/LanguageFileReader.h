#pragma once
#include "Common.h"
#include "LanguageFile.h"
#include "XmlReader.h"


namespace Logic
{
   namespace IO
   {
     

      /// <summary>Reads strings and pages of an X3 language xml file</summary>
      class LanguageFileReader : XmlReader
      {
         // --------------------- CONSTRUCTION ----------------------

      public:
         LanguageFileReader(StreamPtr in);
         ~LanguageFileReader();

         // ------------------------ STATIC -------------------------

      private:
         static UINT          ParseFileID(wstring& filename);
         static GameLanguage  ParseLanguageID(wstring id);
         static UINT          ParsePageID(const wstring&  pageid, GameVersion&  v);

         // --------------------- PROPERTIES ------------------------
			
			// ---------------------- ACCESSORS ------------------------

			// ----------------------- MUTATORS ------------------------

      public:
         LanguageFile   ReadFile(wstring filename);

      private:
         GameLanguage   ReadLanguageTag(XML::IXMLDOMNodePtr&  element);
         LanguagePage   ReadPage(XML::IXMLDOMNodePtr&  element);
         LanguageString ReadString(XML::IXMLDOMNodePtr&  element, GameVersion v);

         // -------------------- REPRESENTATION ---------------------
      };

   }
}

using namespace Logic::IO;
