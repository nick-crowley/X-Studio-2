#pragma once

#include "Common.h"
#include "StringReader.h"
#include "TObject.h"
#include "TFile.h"

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

      public:
         TFile  ReadFile();

      protected:
         float    ReadFloat(const WCHAR* field);
         void     ReadFooter(TObject& o);
         void     ReadHeader(TObject& o);
         int      ReadInt(const WCHAR* field);
         wstring  ReadString(const WCHAR* field);

         // -------------------- REPRESENTATION ---------------------

      private:
         
      };
   
   }
}

using namespace Logic::IO;
