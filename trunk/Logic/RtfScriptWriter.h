#pragma once


#include "RtfWriter.h"
#include "SyntaxHighlight.h"

namespace Logic
{
   namespace IO
   {
         
      /// <summary>Writes entire scripts in RTF</summary>
      class RtfScriptWriter : protected RtfWriter
      {
      private:
         // --------------------- CONSTRUCTION ----------------------
      public:
         RtfScriptWriter(StreamPtr out, wstring font, UINT size);

         // ------------------------ STATIC -------------------------
      
         // --------------------- PROPERTIES ------------------------
			
		   // ---------------------- ACCESSORS ------------------------			

		   // ----------------------- MUTATORS ------------------------
      public:
         void  Close();
         void  Write(ScriptFile& f);

      private:
         void  WriteCommand(const ScriptFile& f, const ScriptCommand& cmd, const Indent& indent);
         void  WriteToken(const ScriptFile& f, const ScriptToken& tok);

		   // -------------------- REPRESENTATION ---------------------
      protected:
         SyntaxHighlight  Highlights;
      };

   }
}

using namespace Logic::IO;
