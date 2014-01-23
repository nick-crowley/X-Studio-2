#pragma once

#include "Common.h"

namespace Logic
{
   namespace FileSystem
   {
      /// <summary>Supported file types</summary>
      enum class FileType : UINT { Unknown, Script, Language, Mission, Project, Universe };

      /// <summary></summary>
      class FileIdentifier
      {
		   // ------------------------ TYPES --------------------------
      private:
	  
         // --------------------- CONSTRUCTION ----------------------

      public:
         FileIdentifier();
         virtual ~FileIdentifier();
		 
		   DEFAULT_COPY(FileIdentifier);	// Default copy semantics
		   DEFAULT_MOVE(FileIdentifier);	// Default move semantics

         // ------------------------ STATIC -------------------------

         static FileType  Identify(IO::Path  path);

         // --------------------- PROPERTIES ------------------------
			
         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------

      private:
      };
   }
}


using namespace Logic::FileSystem;

