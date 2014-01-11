#include "stdafx.h"
#include "LanguagePage.h"

namespace Logic
{
   namespace Language
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      /*LanguagePage::LanguagePage() : ID(0), Voiced(false)
      {
      }*/

      LanguagePage::LanguagePage(UINT id, wstring title, wstring desc, bool voice) 
         : ID(id), Title(title), Description(desc), Voiced(voice),
           Strings(id)

      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------
   }

}