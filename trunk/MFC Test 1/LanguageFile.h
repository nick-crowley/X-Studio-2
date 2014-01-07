#pragma once
#include "Common.h"
#include "LanguagePage.h"

namespace Library
{
   

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
            throw InvalidValueException(HERE, ERR_LANGUAGE_ID_INVALID, id.c_str());
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