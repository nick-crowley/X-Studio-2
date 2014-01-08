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
         CalculatePrecendence();
      }

      /// <summary>Creates a file descriptor for a catalog based file</summary>
      /// <param name="vfs">The file system</param>
      /// <param name="cat">The catalog containing the file</param>
      /// <param name="subPath">The sub path of the file</param>
      /// <param name="pos">The position within the data-file, in bytes</param>
      XFileInfo::XFileInfo(XFileSystem& vfs, XCatalog& cat, Path subPath, DWORD pos)
         : Source(FileSource::Catalog), FullPath(vfs.GetFolder()+subPath), FileSystem(&vfs), Catalog(&cat), Offset(pos) 
      {
         CalculatePrecendence();
      }

      XFileInfo::~XFileInfo() 
      {
      }

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Compares descriptor against the specified path.</summary>
      /// <param name="path">The path to compare against</param>
      /// <param name="checkExtension">Whether the file extension must also match</param>
      /// <returns></returns>
      bool  XFileInfo::Matches(Path path, bool checkExtension) const
      {
         return checkExtension ? FullPath == path : FullPath.RemoveExtension() == path.RemoveExtension(); 
      }

		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

      /// <summary>Calculates the file precendence based on extension</summary>
      void  XFileInfo::CalculatePrecendence()
      {
         if (FullPath.HasExtension(L".pck"))
            Precedence = 2;
         else if (FullPath.HasExtension(L".xml") || FullPath.HasExtension(L".txt"))
            Precedence = 1;
         else
            Precedence = 0;

         // Ensure Physical files always take precendence over catalog files
         if (Source == FileSource::Physical)
            Precedence += 3;
      }

		// -------------------------------- NESTED CLASSES ------------------------------


   }
}
