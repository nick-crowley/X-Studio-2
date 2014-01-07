#include "stdafx.h"
#include "XFileSystem.h"

namespace Library
{
   namespace FileSystem
   {

      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Creates a file descriptor for a physical file</summary>
      /// <param name="p">Full path of file</param>
      XFileInfo::XFileInfo(Path p) 
         : Source(FileSource::Physical), FullPath(p), FileSystem(nullptr), Catalog(nullptr), Offset(0) 
      {
      }

      /// <summary>Creates a file descriptor for a catalog based file</summary>
      /// <param name="vfs">The file system</param>
      /// <param name="cat">The catalog containing the file</param>
      /// <param name="subPath">The sub path of the file</param>
      /// <param name="pos">The position within the data-file, in bytes</param>
      XFileInfo::XFileInfo(XFileSystem& vfs, XCatalog& cat, Path subPath, DWORD pos)
         : Source(FileSource::Catalog), FullPath(vfs.GetFolder()+subPath), FileSystem(&vfs), Catalog(&cat), Offset(pos) 
      {
      }

      XFileInfo::~XFileInfo() 
      {
      }

      // ------------------------------- PUBLIC METHODS -------------------------------

		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

		// -------------------------------- NESTED CLASSES ------------------------------


   }
}
