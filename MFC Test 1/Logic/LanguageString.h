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
}