#pragma once

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
      class LogicExport XFileInfo
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         XFileInfo(const Path& p);
         XFileInfo(const XFileSystem& vfs, const XCatalog& cat, Path subPath, DWORD size, DWORD position);
         virtual ~XFileInfo();

         // Cannot assign/move 
         NO_COPY_ASSIGN(XFileInfo);
         //NO_MOVE(XFileInfo);

         // ----------------------- STATIC --------------------------
      private:
         static DWORD CalculatePrecendence(FileSource s, const Path& path);

			// --------------------- PROPERTIES ------------------------
      public:
         PROPERTY_GET(Path,DataFile,GetDataFile);

			// ---------------------- ACCESSORS ------------------------
      public:
         Path       GetDataFile() const; 
         bool       Matches(Path path, bool checkExtension) const;
         StreamPtr  OpenRead() const;
         StreamPtr  OpenWrite(const wstring& filename = L"") const;

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

      /// <summary>List of file descriptors</summary>
      typedef list<XFileInfo>  XFileList;
   }
}

using namespace Logic::FileSystem;
