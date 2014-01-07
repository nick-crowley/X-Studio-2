#pragma once
#include "Common.h"
#include "FileStream.h"
#include "CatalogReader.h"

//using namespace Library::FileSystem;

namespace Library
{
   namespace FileSystem
   {
      class XFileSystem;
      
      /// <summary>Represents a catalog in the XFileSystem</summary>
      /// <remarks>The catalog file is locked for writing/deleting during the lifetime of this object</remarks>
      class XCatalog
      {
         /// <summary>A shared pointer to a FileStream</summary>
         typedef shared_ptr<FileStream>  FileStreamPtr;

      public:
         // --------------------- CONSTRUCTION ----------------------

         XCatalog(XFileSystem& vfs, Path path);
         virtual ~XCatalog();

         // Move/copy assign not impl
         XCatalog& operator=(const XCatalog&) = delete;
         XCatalog& operator=(XCatalog&&) = delete;

			// --------------------- PROPERTIES ------------------------
			
			// ---------------------- ACCESSORS ------------------------

         CatalogReader  GetReader();
         Path           GetFullPath()  { return FullPath; }

			// ----------------------- MUTATORS ------------------------

			// -------------------- REPRESENTATION ---------------------

      private:
         Path           FullPath;
         FileStreamPtr  FileLock;
         XFileSystem&   FileSystem;

      };

   }
}
