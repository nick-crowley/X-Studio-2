#include "stdafx.h"
#include "XCatalog.h"
#include "CatalogStream.h"

namespace Library
{
   namespace FileSystem
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Initializes a new instance of the <see cref="XCatalog"/> class.</summary>
      /// <param name="vfs">File system</param>
      /// <param name="path">Full path of catalog file</param>
      XCatalog::XCatalog(XFileSystem& vfs, Path path)
         : FullPath(path), FileSystem(vfs), FileLock(new FileStream(FullPath.c_str(), FileMode::OpenExisting, FileAccess::Read)) 
      { 
      }

      XCatalog::~XCatalog() 
      { 
      }

		// ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Opens the catalog and provides a reader</summary>
      /// <returns></returns>
      CatalogReader  XCatalog::GetReader()
      {
         return CatalogReader( StreamPtr(new CatalogStream(FullPath, FileMode::OpenExisting, FileAccess::Read)) );
      }

		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

		// -------------------------------- NESTED CLASSES ------------------------------

   }
}
