#pragma once
#include "Common.h"
#include "Stream.h"


namespace Logic
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
         XFileInfo(XFileSystem& vfs, XCatalog& cat, Path subPath, DWORD size, DWORD position);
         virtual ~XFileInfo();

         // Default copy/move semantics
         DEFAULT_COPY(XFileInfo);
         DEFAULT_MOVE(XFileInfo);

			// --------------------- PROPERTIES ------------------------
			
         PROPERTY_GET(Path,DataFile,GetDataFile);

			// ---------------------- ACCESSORS ------------------------

         Path       GetDataFile() const; 

         bool       Matches(Path path, bool checkExtension) const;
         StreamPtr  Open() const;

			// ----------------------- MUTATORS ------------------------

      private:
         void  CalculatePrecendence();

			// -------------------- REPRESENTATION ---------------------

      public:
         XFileSystem* FileSystem;
         XCatalog*    Catalog;
         Path         FullPath;
         DWORD        Offset,
                      Length,
                      Precedence;
         FileSource   Source;
      };

   }
}

using namespace Logic::FileSystem;
