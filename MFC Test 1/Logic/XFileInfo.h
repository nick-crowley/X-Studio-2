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

         // ----------------------- STATIC --------------------------

      private:
         static DWORD CalculatePrecendence(FileSource s, const Path& path);

			// --------------------- PROPERTIES ------------------------
      public:
         PROPERTY_GET(Path,DataFile,GetDataFile);

			// ---------------------- ACCESSORS ------------------------

         Path       GetDataFile() const; 

         bool       Matches(Path path, bool checkExtension) const;
         StreamPtr  Open() const;

			// ----------------------- MUTATORS ------------------------

      

			// -------------------- REPRESENTATION ---------------------

      public:
         const XFileSystem* FileSystem;
         const XCatalog*    Catalog;
         const Path         FullPath,
                            Key;
         const DWORD        Offset,
                            Length,
                            Precedence;
         const FileSource   Source;
      };

   }
}

using namespace Logic::FileSystem;
