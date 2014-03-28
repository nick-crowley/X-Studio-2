#pragma once


#include "XmlWriter.h"
#include "SyntaxFile.h"

namespace Logic
{
   namespace IO
   {
         
      /// <summary></summary>
      class LogicExport SyntaxFileWriter : XmlWriter
      {
         // --------------------- CONSTRUCTION ----------------------

      public:
         SyntaxFileWriter(StreamPtr out);
         virtual ~SyntaxFileWriter();

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
			
		   // ---------------------- ACCESSORS ------------------------			

		   // ----------------------- MUTATORS ------------------------

         void  Close();
         void  Write(const SyntaxFile& f, const wstring& title, const wstring& version);

		   // -------------------- REPRESENTATION ---------------------

      private:
      };

   }
}

using namespace Logic::IO;
