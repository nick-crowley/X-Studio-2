#include "stdafx.h"
#include "XFileSystem.h"
#include "CatalogStream.h"

using namespace Library::IO;

namespace Library
{
   namespace FileSystem
   {

      /// <summary>Initializes a new instance of the <see cref="XFileSystem"/> class.</summary>
      /// <param name="folder">The folder.</param>
      /// <param name="version">The version.</param>
      XFileSystem::XFileSystem(Path folder, GameVersion version) : Folder(folder), Version(version)
      {
         if (!folder.Exists())
            throw ArgumentException(HERE, L"folder", L"path does not exist");
         if (version == GameVersion::Rebirth)
            throw NotSupportedException(HERE, L"Rebirth is not supported");

         // Ensure trailing backslash
         Folder.AppendBackslash();
      }


      XFileSystem::~XFileSystem()
      {
         Catalogs.Clear();
      }

      DWORD  XFileSystem::Enumerate()
      {
         // Clear previous
         Catalogs.Clear();
         Files.Clear();

         // Enumerate
         EnumerateCatalogs();
         return EnumerateFiles();
      }

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
   }
}
