#include "stdafx.h"
#include "XFileSystem.h"
#include "GZipStream.h"
#include "DataStream.h"
#include "EncryptedStream.h"

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
         StreamPtr s;

         // PHYSICAL: Open file directly
         if (Source == FileSource::Physical)
         {
            // basic file stream
            s = StreamPtr(new FileStream(FullPath, FileMode::OpenExisting, FileAccess::Read));

            // Wrap in encrypted stream if necessary
            if (s->GetLength() > 3)
            {
               WORD header;

               // Read 2-byte header and reset position
               s->Seek(1, SeekOrigin::Begin);
               s->Read(reinterpret_cast<BYTE*>(&header), 2);
               s->Seek(0, SeekOrigin::Begin);

               // Check for encrypted GZip header 
               if ((header ^ 0x7e7e) == 0x8b1f)
                  s = StreamPtr(new EncryptedStream(s));
            }
         }
         else
            // CATALOG: Read from .dat file
            s = StreamPtr(new DataStream(*this));

         // PCK: Wrap in GZip decompression stream
         if (FullPath.HasExtension(L".pck") || FullPath.HasExtension(L".zip"))
            s = StreamPtr(new GZipStream(StreamPtr(s), GZipStream::Operation::Decompression));

         // Return stream
         return s;
      }

      /// <summary>Opens a stream for writing</summary>
      /// <param name="filename">Optional: Name of file within GZip archive, if desired</param>
      /// <returns></returns>
      StreamPtr  XFileInfo::OpenWrite(const wstring& filename) const
      {
         // Ensure physical
         if (Source == FileSource::Catalog)
            throw InvalidOperationException(HERE, L"Catalog based files are read-only");

         // basic file stream
         StreamPtr s(new FileStream(FullPath, FileMode::OpenAlways, FileAccess::Write));

         // PCK: Wrap in GZip compression stream
         if (FullPath.HasExtension(L".pck") || FullPath.HasExtension(L".zip"))
         {
            shared_ptr<GZipStream> gzip(new GZipStream(s, GZipStream::Operation::Compression));

            // Set filename within archive
            gzip->SetFileName(!filename.empty() ? filename : FullPath.RemoveExtension().FileName);
            return gzip;
         }

         // Return stream
         return s;
      }

		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

      

		// -------------------------------- NESTED CLASSES ------------------------------


   }
}
