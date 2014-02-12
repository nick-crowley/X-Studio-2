#pragma once

#include "Common.h"
#include "RtfWriter.h"

namespace Logic
{
   namespace IO
   {
         
      /// <summary>Writes entire scripts in RTF</summary>
      class RtfScriptTextWriter : protected RtfWriter
      {
      private:
         // --------------------- CONSTRUCTION ----------------------
      public:
         RtfScriptTextWriter(string&  out);

         // ------------------------ STATIC -------------------------
      private:
         const static COLORREF  Black = RGB(0,0,0), Green = RGB(0,255,0), Blue = RGB(0,0,255), 
                                White = RGB(255,255,255), Cyan = RGB(150,220,220), Red = RGB(255,0,0), 
                                Yellow = RGB(255,255,0), Grey = RGB(128,128,128), Purple = RGB(255,0,255); 

         // --------------------- PROPERTIES ------------------------
			
		   // ---------------------- ACCESSORS ------------------------			

		   // ----------------------- MUTATORS ------------------------
      public:
         void  Close();
         void  Write(ScriptFile& f);

      private:
         void  WriteCommand(const ScriptCommand& cmd, UINT  indent);
         void  WriteToken(const ScriptToken& tok);

		   // -------------------- REPRESENTATION ---------------------
      private:
      };

   }
}

using namespace Logic::IO;
