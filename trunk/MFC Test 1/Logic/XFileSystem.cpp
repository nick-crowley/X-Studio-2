#include "stdafx.h"
#include "XFileSystem.h"
#include <algorithm>
#include <functional>

using namespace Logic::IO;

namespace Logic
{
   namespace FileSystem
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Initializes a new instance of the <see cref="XFileSystem"/> class.</summary>
      /// <param name="folder">The folder.</param>
      /// <param name="version">The version.</param>
      /// <exception cref="Logic::DirectoryNotFoundException">Folder does not exist</exception>
      /// <exception cref="Logic::NotSupportedException">Version is X2 or X-Rebirth</exception>
      XFileSystem::XFileSystem(Path folder, GameVersion version) : Folder(folder), Version(version)
      {
         // Ensure folder exists
         if (!folder.Exists())
            throw DirectoryNotFoundException(HERE, folder);

         // Ensure not X2/X4
         if (Version == GameVersion::Threat || version == GameVersion::Rebirth)
            throw NotSupportedException(HERE, L"X2 and Rebirth are not supported");

         // Ensure trailing backslash
         Folder = Folder.AppendBackslash();
      }

      /// <summary>Releases the locks on the catalogs</summary>
      XFileSystem::~XFileSystem()
      {
         Catalogs.Clear();
         Files.Clear();
      }
      
		// ------------------------------- PUBLIC METHODS -------------------------------
      
      /// <summary>Queries whether file system contains a file</summary>
      /// <param name="path">Full path including extension</param>
      /// <param name="matchExt">True to match exact extension, false to use precedence rules</param>
      /// <returns></returns>
      bool  XFileSystem::Contains(Path  path, bool  matchExt) const
      {
         // Check for results
         return Query(path, matchExt).Count > 0;
      }

      /// <summary>Enumerates and locks the catalogs and their contents.  Any previous contents are cleared.</summary>
      /// <returns></returns>
      /// <exception cref="Logic::FileNotFoundException">Catalog not found</exception>
      /// <exception cref="Logic::IOException">I/O error occurred</exception>
      DWORD  XFileSystem::Enumerate()
      {
         // Clear previous
         Catalogs.Clear();
         Files.Clear();

         // Enumerate
         EnumerateCatalogs();
         return EnumerateFiles();
      }

      /// <summary>Queries whether file system contains a file</summary>
      /// <param name="path">Full path including extension</param>
      /// <param name="matchExt">True to match exact extension, false to use precedence rules</param>
      /// <returns>File descriptor</returns>
      /// <exception cref="Logic::FileNotFoundException">File not found</exception>
      /// <exception cref="Logic::IOException">I/O error occurred</exception>
      XFileInfo XFileSystem::Find(Path  path, bool  matchExt) const
      {
         ResultCollection results( Query(path, matchExt) );

         // Check for results
         if (results.Count == 0)
            throw FileNotFoundException(HERE, path);

         // Return result with highest precedence
         return *results.begin();
      }

		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------
      
      /// <summary>Enumerates and locks the catalogs</summary>
      /// <returns></returns>
      /// <exception cref="Logic::IOException">I/O error occurred</exception>
      DWORD  XFileSystem::EnumerateCatalogs()
      {
         const WCHAR *formats[2] = { L"%s%02i.cat", L"%saddon\\%02i.cat" },
                     *format;

         // Perform two passes for AlbionPrelude
         for (int pass = 0; pass < 2; pass++)
         {
            format = formats[pass];

            // Search for catalogs
            for (int i = 1; i < 99; i++)
            {
               Path path( StringResource::Format(format, (WCHAR*)Folder, i) );

               // Ensure exists
               if (!path.Exists() || !path.RenameExtension(L".dat").Exists())
                  break;
               
               // Add to FRONT of list  [Ensure highest priority at the front]
               Catalogs.Add(XCatalog(*this, path.RenameExtension(L".cat")));
            }

            if (Version != GameVersion::AlbionPrelude)
               break;
         }
         
         return Catalogs.Count;
      }

      /// <summary>Enumerates the files within the catalogs</summary>
      /// <returns></returns>
      /// <exception cref="Logic::FileNotFoundException">Catalog not found</exception>
      /// <exception cref="Logic::IOException">I/O error occurred</exception>
      DWORD  XFileSystem::EnumerateFiles()
      {
         // Iterate thru catalogs (Highest priority -> Lowest)
         for (auto it = Catalogs.Begin(); it != Catalogs.End(); ++it)
         {
            CatalogReader  reader(it->GetReader());
            wstring        path;
            DWORD          size;

            // Iterate thru declarations + insert. Calculate running offset.  (Duplicate files are automatically discarded)
            for (DWORD offset = 0; reader.ReadDeclaration(path, size); offset += size)
               Files.Add( XFileInfo(*this, *it, path, size, offset) );
         }

         // Return count
         return Files.Count;
      }

      /// <summary>Queries whether file system contains a file</summary>
      /// <param name="path">Full path including extension</param>
      /// <param name="matchExt">True to match exact extension, false to use precedence rules</param>
      /// <returns>Results collection</returns>
      /// <exception cref="Logic::IOException">I/O error occurred</exception>
      XFileSystem::ResultCollection  XFileSystem::Query(Path  path, bool  matchExt) const
      {
         ResultCollection  results;
         
         // Enumerate matching catalog files
         for_each(Files.begin(), Files.end(), [&](const XFileInfo& f)->void { 
            if (f.Matches(path, matchExt))
               results.insert(f); 
         } );
         
         // Enumerate matching files on disc
         for (FileSearch fs(matchExt ? path : path.RenameExtension(L".*")); fs.HasResult(); fs.Next())
            results.insert(XFileInfo(fs.FullPath));

         // Return results
         return results;
      }

		// -------------------------------- NESTED CLASSES ------------------------------


   }
}
