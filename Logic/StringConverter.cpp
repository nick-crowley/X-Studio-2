#include "stdafx.h"
#include "ScriptParameter.h"

namespace Logic
{
   namespace Scripts
   {
   
      // -------------------------------- CONSTRUCTION --------------------------------

      ScriptParameter::StringConverter::StringConverter()
      {
      }


      ScriptParameter::StringConverter::~StringConverter()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Generates a string by un-escaping any apostrophes.</summary>
      /// <param name="str">Translated string.</param>
      /// <returns>Raw string</returns>
      wstring  ScriptParameter::StringConverter::Generate(const wstring& str)
      {
         return GuiString(str).ReplaceAll(L"\\'", L"'");
      }

      /// <summary>Translates a string by escaping any apostrophes.</summary>
      /// <param name="str">Untranslated string.</param>
      /// <returns>Translated string</returns>
      wstring  ScriptParameter::StringConverter::Translate(const wstring& str)
      {
         return GuiString(str).ReplaceAll(L"'", L"\\'");
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   
   }
}

