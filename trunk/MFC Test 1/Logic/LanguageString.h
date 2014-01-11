#pragma once
#include "Common.h"

namespace Logic
{
   namespace Language
   {


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

         UINT        ID;
         wstring     Text;
         GameVersion Version;
      };
   
   }
}