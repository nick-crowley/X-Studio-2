#pragma once



namespace Logic
{
   /// <summary>Base class for all files: script/language/project etc.</summary>
   class FileBase
   {
      // ------------------------ TYPES --------------------------
   private:
	  
      // --------------------- CONSTRUCTION ----------------------

   public:
      FileBase()
      {}
      FileBase(Path path) : FullPath(path)
      {}
      FileBase(FileBase&& r) : FullPath(move(r.FullPath))
      {}
      virtual ~FileBase()
      {}
		 
		DEFAULT_COPY(FileBase);	// Default copy semantics
		DEFAULT_MOVE(FileBase);	// Default move semantics

      // ------------------------ STATIC -------------------------

      // --------------------- PROPERTIES ------------------------
			
      // ---------------------- ACCESSORS ------------------------			

      // ----------------------- MUTATORS ------------------------

      // -------------------- REPRESENTATION ---------------------
   public:
      Path   FullPath;

   private:
   };
}