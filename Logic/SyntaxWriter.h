#pragma once


#include "XmlWriter.h"
#include "SyntaxFile.h"

namespace Logic
{
   namespace IO
   {
         
      /// <summary></summary>
      class LogicExport SyntaxWriter : XmlWriter
      {
         // --------------------- CONSTRUCTION ----------------------

      public:
         SyntaxWriter(StreamPtr out);
         virtual ~SyntaxWriter();

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
			
		   // ---------------------- ACCESSORS ------------------------			

		   // ----------------------- MUTATORS ------------------------

         void  Close();
         void  Write(const SyntaxFile& f);

		   // -------------------- REPRESENTATION ---------------------

      private:
      };

   }
}

using namespace Logic::IO;
