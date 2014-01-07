#pragma once
#include "Common.h"
#include "LanguagePage.h"

namespace Library
{
   

   /// <summary>Reads and verifies the language id of a language file</summary>
   class GameLanguageReader
   {
      // --------------------- CONSTRUCTION ----------------------

		// --------------------- PROPERTIES ------------------------
			
		// ---------------------- ACCESSORS ------------------------

      // ------------------------ STATIC -------------------------

   public:
      static GameLanguage  Parse(wstring id)
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

      // ----------------------- MUTATORS ------------------------

		// -------------------- REPRESENTATION ---------------------

   };


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