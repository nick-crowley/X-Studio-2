#pragma once

#include "Common.h"
#include "ScriptFile.h"

namespace Logic
{
   namespace Scripts
   {
      class ScriptFile;
   }
   namespace IO
   {
         
      /// <summary></summary>
      class ScriptWriter
      {
         // --------------------- CONSTRUCTION ----------------------

      public:
         ScriptWriter();
         virtual ~ScriptWriter();

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
			
		   // ---------------------- ACCESSORS ------------------------			

		   // ----------------------- MUTATORS ------------------------

         virtual void  Write(ScriptFile& f) PURE;

		   // -------------------- REPRESENTATION ---------------------

      private:
      };

      class RtfScriptWriter
      {
      public:
         RtfScriptWriter(string&  out) : Output(out)
         {}

         void  Write(ScriptFile& f);

      private:
         string&  Output;
      };

   }
}

using namespace Logic::IO;
