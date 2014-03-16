#pragma once

#include "FileStream.h"
#include "CatalogReader.h"

//using namespace Logic::FileSystem;

namespace Logic
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
         NO_COPY_ASSIGN(XCatalog);
         NO_MOVE_ASSIGN(XCatalog);

			// --------------------- PROPERTIES ------------------------
			
			// ---------------------- ACCESSORS ------------------------

         CatalogReader  GetReader() const;
         Path           GetFullPath()  { return FullPath; }

			// ----------------------- MUTATORS ------------------------

			// -------------------- REPRESENTATION ---------------------

         Path           FullPath;

      private:
         FileStreamPtr  FileLock;
         XFileSystem&   FileSystem;

      };

   }
}
