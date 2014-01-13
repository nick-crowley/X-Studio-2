#pragma once

#include "Common.h"
#include "XmlWriter.h"
#include "SyntaxFile.h"

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

         int  GetSuperScript(const CommandSyntax& cmd, const ParameterSyntax& p);

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
