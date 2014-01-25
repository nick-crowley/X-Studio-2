#pragma once
#include "Common.h"

namespace Logic
{
   namespace Language
   {

      /// <summary>Language string with immutable ID</summary>
      class LanguageString
      {
         // --------------------- CONSTRUCTION ----------------------

      public:
         LanguageString(UINT  id, wstring  txt, GameVersion v) : ID(id), Text(txt), Version(v) {};
         ~LanguageString() {};

         // --------------------- PROPERTIES ------------------------
			
		   // ---------------------- ACCESSORS ------------------------

		   // ----------------------- MUTATORS ------------------------

		   // -------------------- REPRESENTATION ---------------------

         const UINT  ID;
         wstring     Text;
         GameVersion Version;
      };
   
   }
}