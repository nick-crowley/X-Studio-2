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
      XFileInfo::XFileInfo(const Path& p) 
         : Source(FileSource::Physical), 
           FullPath(p), FileSystem(nullptr), Catalog(nullptr), Offset(0), Length(0),
           Precedence(CalculatePrecendence(Source, FullPath)), 
           Key(FullPath.RemoveExtension())
      {
      }

      /// <summary>Creates a file descriptor for a catalog based file</summary>
      /// <param name="vfs">The file system</param>
      /// <param name="cat">The catalog containing the file</param>
      /// <param name="subPath">The sub path of the file</param>
      /// <param name="size">The size of the file, in bytes</param>
      /// <param name="position">The position within the data-file, in bytes</param>
      XFileInfo::XFileInfo(const XFileSystem& vfs, const XCatalog& cat, Path subPath, DWORD size, DWORD position)
         : Source(FileSource::Catalog), 
           FullPath(vfs.GetFolder()+subPath), FileSystem(&vfs), Catalog(&cat), Length(size), Offset(position),
           Precedence(CalculatePrecendence(Source, FullPath)), 
           Key(FullPath.RemoveExtension())
      {
      }

      XFileInfo::~XFileInfo() 
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      /// <summary>Calculates the file precendence based on extension</summary>
      /// <param name="s">File source</param>
      /// <param name="path">Full path</param>
      /// <returns>File precendence</returns>
      DWORD  XFileInfo::CalculatePrecendence(FileSource s, const Path& path)
      {
         DWORD  precedence = 0;

         if (path.HasExtension(L".pck"))
            precedence = 2;
         else if (path.HasExtension(L".xml") || path.HasExtension(L".txt"))
            precedence = 1;

         // Ensure Physical files always take precendence over catalog files
         if (s == FileSource::Physical)
            precedence += 3;

         return precedence;
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
         return checkExtension ? FullPath == path : Key == path.RemoveExtension(); 
      }

      /// <summary>Opens a stream for reading</summary>
      /// <returns></returns>
      StreamPtr  XFileInfo::OpenRead() const
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

      

		// -------------------------------- NESTED CLASSES ------------------------------


   }
}
