#pragma once

#include "Common.h"
#include "XmlWriter.h"

namespace Logic
{
   namespace IO
   {
         
      /// <summary></summary>
      class SyntaxWriter : XmlWriter
      {
         // --------------------- CONSTRUCTION ----------------------

      public:
         SyntaxWriter(StreamPtr out);
         virtual ~SyntaxWriter();

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
			
		   // ---------------------- ACCESSORS ------------------------			

		   // ----------------------- MUTATORS ------------------------

		   // -------------------- REPRESENTATION ---------------------

      private:
      };

   }
}

using namespace Logic::IO;
