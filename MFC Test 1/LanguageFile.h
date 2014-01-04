#pragma once
#include "stl.h"
#include "LanguagePage.h"
#include "Exceptions.h"
#include "Resource.h"

namespace Library
{
   enum class GameLanguage : int { English, French, German, Russian };

   class GameLanguageReader
   {
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
            throw InvalidValueException(ERR_LANGUAGE_ID_INVALID, id.c_str());
         }
      }

   };


   
   class LanguageFile
   {
   public:
      LanguageFile() {};
      ~LanguageFile() {};

      std::vector<LanguagePage> Pages;
      GameLanguage   Language;
   };



}