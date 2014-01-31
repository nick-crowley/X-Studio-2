#include "stdafx.h"
#include "XFileSystem.h"
#include "FileSearch.h"
#include <algorithm>
#include <functional>

using namespace Logic::IO;

namespace Logic
{
   namespace FileSystem
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Initializes a new instance of the <see cref="XFileSystem"/> class.</summary>
      XFileSystem::XFileSystem() 
      {
      }

      /// <summary>Releases the locks on the catalogs</summary>
      XFileSystem::~XFileSystem()
      {
         Catalogs.clear();
         Files.clear();
      }
      
		// ------------------------------- PUBLIC METHODS -------------------------------
      
      /// <summary>Searches for all files within a known folder</summary>
      /// <param name="folder">Known folder</param>
      /// <returns>Results collection</returns>
      /// <exception cref="Logic::IOException">I/O error occurred</exception>
      XFileList  XFileSystem::Browse(XFolder folder) const
      {
         return Browse(GetFolder(folder));
      }

      /// <summary>Searches for all files within a folder</summary>
      /// <param name="folder">Full path of folder</param>
      /// <returns>Results collection</returns>
      /// <exception cref="Logic::IOException">I/O error occurred</exception>
      XFileList  XFileSystem::Browse(Path  folder) const
      {
         XFileList results;

         // Ensure trailing backslash
         folder = folder.AppendBackslash();

         // Copy all files with exactly the same folder
         for (auto pair : Files)
            if (folder == pair.first.Folder)
               results.push_back(pair.second);

         return results;
      }

      /// <summary>Queries whether file system contains a file</summary>
      /// <param name="path">Full path EXCLUDING extension</param>
      /// <returns></returns>
      bool  XFileSystem::Contains(Path  path) const
      {
         return Files.find(path) != Files.end();
      }

      /// <summary>Enumerates and locks the catalogs and their contents.  Any previous contents are cleared.</summary>
      /// <param name="folder">Folder to enumerate</param>
      /// <param name="version">Game version</param>
      /// <param name="data">Background worker data</param>
      /// <returns>Number of files found</returns>
      /// <exception cref="Logic::ArgumentNullException">Worker data is null</exception>
      /// <exception cref="Logic::DirectoryNotFoundException">Folder does not exist</exception>
      /// <exception cref="Logic::NotSupportedException">Version is X2 or X-Rebirth</exception>
      /// <exception cref="Logic::FileNotFoundException">Catalog not found</exception>
      /// <exception cref="Logic::IOException">I/O error occurred</exception>
      DWORD  XFileSystem::Enumerate(Path folder, GameVersion version, const WorkerData* data)
      {
         REQUIRED(data);

         // Clear previous
         Catalogs.clear();
         Files.clear();

         // Feedback
         data->SendFeedback(ProgressType::Info, 1, L"Searching for catalogs...");
         Console << ENDL << Colour::Cyan << L"Building " << VersionString(version) << L" VFS from " << (WCHAR*)folder << ENDL;

         // Ensure folder exists
         if (!folder.Exists())
            throw DirectoryNotFoundException(HERE, folder);

         // Ensure not X2/X4
         if (Version == GameVersion::Threat || version == GameVersion::Rebirth)
            throw NotSupportedException(HERE, L"X2 and Rebirth are not supported");

         // Ensure trailing backslash
         Folder = folder.AppendBackslash();

         // Enumerate catalogs/files
         EnumerateCatalogs();
         EnumerateFiles(data);
         Console << Colour::Green << L"FileSystem enumerated successfully" << ENDL;

         // Return count
         return Files.size();
      }

      /// <summary>Queries whether file system contains a file</summary>
      /// <param name="path">Full path EXCLUDING extension</param>
      /// <returns>File descriptor</returns>
      /// <exception cref="Logic::FileNotFoundException">File not found</exception>
      XFileInfo XFileSystem::Find(Path  path) const
      {
         auto it = Files.find(path);

         // Error: file not found
         if (it == Files.end())
            throw FileNotFoundException(HERE, path);

         // Return result 
         return it->second;
      }

      /// <summary>Gets the full path of a known subfolder</summary>
      /// <param name="f">The folder</param>
      /// <returns>Full path with trailing backslash</returns>
      Path   XFileSystem::GetFolder(XFolder f) const
      {
         Path base = Folder;

         // X3AP: Append addon folder
         if (Version == GameVersion::AlbionPrelude)
            base = base + L"addon\\";

         switch (f)
         {
         case XFolder::Director:  return base + L"director\\";
         case XFolder::Language:  return base + L"t\\";
         case XFolder::Scripts:   return base + L"scripts\\";
         case XFolder::Types:     return base + L"types\\";
         }

         throw ArgumentException(HERE, L"f", L"Unknown XFolder constant");
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
      /// <param name="data">Background worker data</param>
      /// <returns></returns>
      /// <exception cref="Logic::FileNotFoundException">Catalog not found</exception>
      /// <exception cref="Logic::IOException">I/O error occurred</exception>
      DWORD  XFileSystem::EnumerateFiles(const WorkerData* data)
      {
         // Iterate thru catalogs (Highest priority -> Lowest)
         for (const XCatalog& cat : Catalogs)
         {
            CatalogReader  reader(cat.GetReader());
            wstring        path;
            DWORD          size;

            // Feedback
            Console << Colour::White << L"Reading catalog " << (const WCHAR*)cat.FullPath << ENDL;
            data->SendFeedback(ProgressType::Info, 2, GuiString(L"Reading catalog '%s'", (const WCHAR*)cat.FullPath));

            // Iterate thru declarations + insert. Calculate running offset.  (Duplicate files are automatically discarded)
            for (DWORD offset = 0; reader.ReadDeclaration(path, size); offset += size)
               Files.Add( XFileInfo(*this, cat, path, size, offset) );
         }

         // Enumerate physical files
         EnumerateFolder(Folder);

         // Return count
         return Files.size();
      }

      /// <summary>Enumerates physical files within the folder</summary>
      /// <param name="folder">The folder path</param>
      void  XFileSystem::EnumerateFolder(Path  folder)
      {
         folder = folder.AppendBackslash();

         // Enumerate matching files on disc
         for (FileSearch fs(folder + L"*.*"); fs.HasResult(); fs.Next())
         {
            // Skip catalogs/datafiles
            if (fs.FileName == L"." || fs.FileName == L".." || fs.FullPath.HasExtension(L".cat") || fs.FullPath.HasExtension(L".dat"))
               continue;

            // Add files, recurse into folders
            if (!fs.IsDirectory())
               Files.Add(XFileInfo(fs.FullPath));
            else
               EnumerateFolder(fs.FullPath);
         }
         
      }

		// -------------------------------- NESTED CLASSES ------------------------------


   }
}
