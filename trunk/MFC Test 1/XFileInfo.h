#pragma once
#include "Common.h"

//using namespace Library::FileSystem;

namespace Library
{
   
   namespace FileSystem
   {
      class XFileSystem;
      class XCatalog;
      
      /// <summary>Identifies whether a file is on disk or within a catalog</summary>
      enum class FileSource  { Catalog, Physical };

      /// <summary>Represents a file descriptor for any physical or catalog based file</summary>
      class XFileInfo
      {
      public:
         // --------------------- CONSTRUCTION ----------------------

         XFileInfo(Path p);
         XFileInfo(XFileSystem& vfs, XCatalog& cat, Path subPath, DWORD pos);
         virtual ~XFileInfo();

         // Default copy/move semantics
         DEFAULT_COPY(XFileInfo);
         DEFAULT_MOVE(XFileInfo);

			// --------------------- PROPERTIES ------------------------
			
			// ---------------------- ACCESSORS ------------------------

         bool  Matches(Path path, bool checkExtension) const;

			// ----------------------- MUTATORS ------------------------

      private:
         void  CalculatePrecendence();

			// -------------------- REPRESENTATION ---------------------

      public:
         XFileSystem* FileSystem;
         XCatalog*    Catalog;
         Path         FullPath;
         DWORD        Offset,
                      Precedence;
         FileSource   Source;
      };


      
   }
}
