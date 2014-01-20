#pragma once

#include "Common.h"
#include "StringReader.h"

namespace Logic
{
   namespace IO
   {
   
      /// <summary></summary>
      class TFileReader : protected StringReader
      {
		 // ------------------------ TYPES --------------------------
      private:
	  
         // --------------------- CONSTRUCTION ----------------------

      public:
         TFileReader(StreamPtr in);
         virtual ~TFileReader();
		 
		   NO_COPY(TFileReader);	// No copy semantics
		   NO_MOVE(TFileReader);	// No move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
			
         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------

         int      ReadInt(const WCHAR* field);
         float    ReadFloat(const WCHAR* field);
         wstring  ReadString(const WCHAR* field);

         // -------------------- REPRESENTATION ---------------------

      private:
         
      };
   
   }
}

using namespace Logic::IO;
