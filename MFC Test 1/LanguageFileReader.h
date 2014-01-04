#pragma once
#include "stl.h"
#include "LanguageFile.h"
#include "msxml.h"
#include "Exceptions.h"

namespace Library
{

   /*enum class GameVersion : int { Threat, Reunion, TerranConflict, AlbionPrelude, Rebirth };
   
   class GameVersionReader
   {
   public:
      GameVersionReader(const WCHAR*  id);

      GameVersion  Version;
      UINT         ID;
   };*/


   class LanguageFileReader
   {
   public:
      LanguageFileReader(Path szFullPath) : FullPath(szFullPath) {};
      ~LanguageFileReader() {};

      LanguageFile ReadFile();

   private:
      
      void           ReadElement(XML::IXMLDOMNodePtr&  node, const WCHAR*  name);
      wstring        ReadAttribute(XML::IXMLDOMNodePtr&  node, const WCHAR*  name);
      GameLanguage   ReadLanguageTag(XML::IXMLDOMNodePtr&  element);
      wstring        TryReadAttribute(XML::IXMLDOMNodePtr&  node, const WCHAR*  name);

      LanguagePage   ReadPage(XML::IXMLDOMNodePtr&  element);
      LanguageString ReadString(XML::IXMLDOMNodePtr&  element);
      
      Path  FullPath;
   };

}