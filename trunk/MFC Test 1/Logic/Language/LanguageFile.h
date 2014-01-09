#pragma once
#include "../Common.h"
#include "LanguagePage.h"

using namespace Library::Language;

namespace Library
{
   namespace Language
   {

      /// <summary>Represents an X3 language file, provides a collection of language pages</summary>
      class LanguageFile
      {
         // --------------------- CONSTRUCTION ----------------------

      public:
         LanguageFile() {};
         ~LanguageFile() {};

         // --------------------- PROPERTIES ------------------------
			
		   // ---------------------- ACCESSORS ------------------------

		   // ----------------------- MUTATORS ------------------------

		   // -------------------- REPRESENTATION ---------------------

         std::vector<LanguagePage> Pages;
         GameLanguage   Language;
      };

   }

}