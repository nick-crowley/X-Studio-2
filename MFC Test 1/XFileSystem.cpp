#include "stdafx.h"
#include "XFileSystem.h"
#include <algorithm>

using namespace Library::IO;

namespace Library
{
   namespace FileSystem
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Initializes a new instance of the <see cref="XFileSystem"/> class.</summary>
      /// <param name="folder">The folder.</param>
      /// <param name="version">The version.</param>
      /// <exception cref="Library.DirectoryNotFoundException">Folder does not exist</exception>
      /// <exception cref="Library.NotSupportedException">Version is X2 or X-Rebirth</exception>
      XFileSystem::XFileSystem(Path folder, GameVersion version) : Folder(folder), Version(version)
      {
         // Ensure folder exists
         if (!folder.Exists())
            throw DirectoryNotFoundException(HERE, folder);

         // Ensure not X2/X4
         if (Version == GameVersion::Threat || version == GameVersion::Rebirth)
            throw NotSupportedException(HERE, L"X2 and Rebirth are not supported");

         // Ensure trailing backslash
         Folder.AppendBackslash();
      }

      /// <summary>Releases the locks on the catalogs</summary>
      XFileSystem::~XFileSystem()
      {
         Catalogs.Clear();
         Files.Clear();
      }
      
		// ------------------------------- PUBLIC METHODS -------------------------------
      
      /// <summary>Enumerates and locks the catalogs and their contents.  Any previous contents are cleared.</summary>
      /// <returns></returns>
      /// <exception cref="Library.FileNotFoundException">Catalog not found</exception>
      /// <exception cref="Library.Win32Exception">I/O error occurred</exception>
      DWORD  XFileSystem::Enumerate()
      {
         // Clear previous
         Catalogs.Clear();
         Files.Clear();

         // Enumerate
         EnumerateCatalogs();
         return EnumerateFiles();
      }

      bool  XFileSystem::Contains(Path  p)
      {
         return find_if(Files.begin(), Files.end(), [&](XFileInfo f)->bool { return f.FullPath == p; } ) != Files.end();
         //return true;
      }

		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------
      
      /// <summary>Enumerates and locks the catalogs</summary>
      /// <returns></returns>
      /// <exception cref="Library.Win32Exception">I/O error occurred</exception>
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
               Path path( StringResource::Format(format, Folder.c_str(), i) );

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
      /// <exception cref="Library.FileNotFoundException">Catalog not found</exception>
      /// <exception cref="Library.Win32Exception">I/O error occurred</exception>
      DWORD  XFileSystem::EnumerateFiles()
      {
         // Iterate thru catalogs (Highest priority -> Lowest)
         for (auto it = Catalogs.Begin(); it != Catalogs.End(); ++it)
         {
            CatalogReader  reader(it->GetReader());
            wstring        path;
            DWORD          size;

            // Add files, ignore duplicates from lower priority catalogs
            while (reader.ReadDeclaration(path, size))
               Files.Add( XFileInfo(*this, *it, path, size) );
         }

         // Return count
         return Files.Count;
      }
		// -------------------------------- NESTED CLASSES ------------------------------


   }
}
