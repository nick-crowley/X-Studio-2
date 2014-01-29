#include "stdafx.h"
#include "XCatalog.h"
#include "CatalogStream.h"

namespace Logic
{
   namespace FileSystem
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Initializes a new instance of the <see cref="XCatalog"/> class.</summary>
      /// <param name="vfs">File system</param>
      /// <param name="path">Full path of catalog file</param>
      XCatalog::XCatalog(XFileSystem& vfs, Path path)
         : FullPath(path), FileSystem(vfs), FileLock(new FileStream(FullPath, FileMode::OpenExisting, FileAccess::Read)) 
      { 
      }

      /// <summary>Releases the lock on the catalog</summary>
      XCatalog::~XCatalog()
      { 
         // File lock safely released (nothrow)
      }

		// ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Opens the catalog and provides a reader</summary>
      /// <returns></returns>
      /// <exception cref="Logic::FileNotFoundException">File not found</exception>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      CatalogReader  XCatalog::GetReader() const
      {
         return CatalogReader( StreamPtr(new CatalogStream(FullPath, FileMode::OpenExisting, FileAccess::Read)) );
      }

		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

		// -------------------------------- NESTED CLASSES ------------------------------

   }
}
