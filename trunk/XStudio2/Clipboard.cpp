#pragma once
#include "stdafx.h"
#include "Clipboard.h"


namespace GUI
{
   namespace Utils
   {
      /// <summary>Singleton instance</summary>
      Clipboard  Clipboard::Instance;

      /// <summary>Clipboard data</summary>
      Clipboard::LanguageStringPtr  Clipboard::StringData;
      Clipboard::LanguagePagePtr    Clipboard::PageData;

      // -------------------------------- CONSTRUCTION --------------------------------

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------

   }
}
