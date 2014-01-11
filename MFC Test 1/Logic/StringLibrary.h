#pragma once
#include "Common.h"
#include "LanguageFile.h"

namespace Library
{
   namespace Language
   {
         
      /// <summary></summary>
      class StringLibrary
      {
         // --------------------- CONSTRUCTION ----------------------

      public:
         StringLibrary();
         virtual ~StringLibrary();

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
			
		   // ---------------------- ACCESSORS ------------------------			

		   // ----------------------- MUTATORS ------------------------

      public:
         //void  Enumerate();

		   // -------------------- REPRESENTATION ---------------------

      private:
         list<LanguageFile>  Files;
      };

   }
}

using namespace Library::Language;
