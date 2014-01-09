#include "stdafx.h"
#include "XFileSystem.h"
#include "GZipStream.h"
#include "DataStream.h"

namespace Logic
{
   namespace FileSystem
   {

      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Creates a file descriptor for a physical file</summary>
      /// <param name="p">Full path of file</param>
      XFileInfo::XFileInfo(Path p) 
         : Source(FileSource::Physical), FullPath(p), FileSystem(nullptr), Catalog(nullptr), Offset(0), Length(0)
      {
         CalculatePrecendence();
      }

      /// <summary>Creates a file descriptor for a catalog based file</summary>
      /// <param name="vfs">The file system</param>
      /// <param name="cat">The catalog containing the file</param>
      /// <param name="subPath">The sub path of the file</param>
      /// <param name="size">The size of the file, in bytes</param>
      /// <param name="position">The position within the data-file, in bytes</param>
      XFileInfo::XFileInfo(XFileSystem& vfs, XCatalog& cat, Path subPath, DWORD size, DWORD position)
         : Source(FileSource::Catalog), FullPath(vfs.GetFolder()+subPath), FileSystem(&vfs), Catalog(&cat), Length(size), Offset(position) 
      {
         CalculatePrecendence();
      }

      XFileInfo::~XFileInfo() 
      {
      }

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Get full path of the data file</summary>
      /// <returns></returns>
      Path  XFileInfo::GetDataFile() const
      { 
         if (Source == FileSource::Physical)
            throw InvalidOperationException(HERE, L"Physical files have no data file");

         // Generate path
         return Catalog->FullPath.RenameExtension(L".dat"); 
      }

      /// <summary>Compares descriptor against the specified path.</summary>
      /// <param name="path">The path to compare against</param>
      /// <param name="checkExtension">Whether the file extension must also match</param>
      /// <returns></returns>
      bool  XFileInfo::Matches(Path path, bool checkExtension) const
      {
         return checkExtension ? FullPath == path : FullPath.RemoveExtension() == path.RemoveExtension(); 
      }

      /// <summary>Opens a stream for reading</summary>
      /// <returns></returns>
      StreamPtr  XFileInfo::Open() const
      {
         Stream*  s;

         // PHYSICAL: Open file directly
         if (Source == FileSource::Physical)
         {
            s = new FileStream(FullPath, FileMode::OpenExisting, FileAccess::Read);

            // TODO: Examine first 3 bytes, wrap in encrypted stream
         }
         else
            // CATALOG: Read from .dat file
            s = new DataStream(*this);

         // PCK: Wrap in GZip decompression stream
         if (FullPath.HasExtension(L".pck"))
            s = new GZipStream(StreamPtr(s), GZipStream::Operation::Decompression);

         // Return stream
         return StreamPtr(s);
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
