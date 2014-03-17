#pragma once



namespace Logic
{
   namespace FileSystem
   {
      /// <summary>Supported file types</summary>
      enum class FileType : UINT { Unknown, Script, Language, Mission,  Project, Universe };

      /// <summary>Get file-type name</summary>
      LogicExport wstring GetString(FileType t);

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

