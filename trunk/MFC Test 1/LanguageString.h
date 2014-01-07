#pragma once
#include "Common.h"

namespace Library
{

   class LanguageString
   {
      // --------------------- CONSTRUCTION ----------------------

   public:
      LanguageString() : ID(0) {};
      LanguageString(UINT  id, wstring  txt) : ID(id), Text(txt) {};
      ~LanguageString() {};

      // --------------------- PROPERTIES ------------------------
			
		// ---------------------- ACCESSORS ------------------------

		// ----------------------- MUTATORS ------------------------

		// -------------------- REPRESENTATION ---------------------

      UINT     ID;
      wstring  Text;
   };

}