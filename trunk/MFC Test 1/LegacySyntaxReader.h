#pragma once
#include "StringReader.h"

namespace Library
{
   namespace Scripts
   {
         
      /// <summary></summary>
      class LegacySyntaxReader : StringReader
      {
         // --------------------- CONSTRUCTION ----------------------

      public:
         LegacySyntaxReader(StreamPtr src);
         virtual ~LegacySyntaxReader();

         // --------------------- PROPERTIES ------------------------
			
		   // ---------------------- ACCESSORS ------------------------
			
		   // ------------------------ STATIC -------------------------

		   // ----------------------- MUTATORS ------------------------

		   // -------------------- REPRESENTATION ---------------------

      private:
      };

   }
}



