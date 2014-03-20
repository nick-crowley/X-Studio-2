#pragma once

namespace Logic
{
   namespace FileSystem
   {
      /// <summary></summary>
      class LogicExport FileIdentifier
      {
		   // ------------------------ TYPES --------------------------
      private:
         const static UINT PEEK_BUFFER = 4096;
	  
         // --------------------- CONSTRUCTION ----------------------
      private:
         FileIdentifier();
         virtual ~FileIdentifier();
		 
		   NO_COPY(FileIdentifier);	// No copy semantics
		   NO_MOVE(FileIdentifier);	// No move semantics

         // ------------------------ STATIC -------------------------
      public:
         static FileType  Identify(Path  path);

         // --------------------- PROPERTIES ------------------------
			
         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------

      private:
      };
   }
}


using namespace Logic::FileSystem;

