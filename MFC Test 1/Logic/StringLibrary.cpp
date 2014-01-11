#include "stdafx.h"
#include "StringLibrary.h"
#include "LanguageFileReader.h"

namespace Logic
{
   namespace Language
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      StringLibrary::StringLibrary()
      {
      }


      StringLibrary::~StringLibrary()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Populates the library with all the language files in the 't' subfolder</summary>
      /// <param name="vfs">The VFS to search</param>
      /// <param name="lang">The language of strings to search for</param>
      /// <returns>Number of files found</returns>
      UINT  StringLibrary::Enumerate(XFileSystem& vfs, GameLanguage lang)
      {
         list<XFileInfo> results;

         // Clear previous contents
         Files.clear();

         // Enumerate non-foreign language files
         for (XFileInfo& f : vfs.Browse(XFolder::Language))
         {
            LanguageFilenameReader fn(f.FullPath.FileName);

            // Add if language matches or is unspecified
            if (fn.Valid && (fn.Language == lang || !fn.HasLanguage))
               results.push_back(f);
         }

         // Read/Store each file
         for (XFileInfo& f : results)
         {
            LanguageFile file = LanguageFileReader(f.Open()).ReadFile(f.FullPath.FileName);

            // Skip files that turn out to be foreign
            if (file.Language == lang)
               Files.insert(file);
         }

         return Files.size();
      }

      /// <summary>Queries whether a string is present</summary>
      /// <param name="page">The page id</param>
      /// <param name="id">The string id.</param>
      /// <returns></returns>
      bool  StringLibrary::Contains(UINT page, UINT id) const
      {
         // Search files in descending file ID
         for (const LanguageFile& f : Files)
            if (f.Contains(page, id))
               return true;

         // Not found
         return false;
      }

      /// <summary>Finds the specified string</summary>
      /// <param name="page">The page id</param>
      /// <param name="id">The string id</param>
      /// <returns></returns>
      /// <exception cref="Logic::PageNotFoundException">Page does not exist</exception>
      /// <exception cref="Logic::StringNotFoundException">String does not exist</exception>
      LanguageString  StringLibrary::Find(UINT page, UINT id) const
      {
         // Search files in descending file ID
         for (const LanguageFile& f : Files)
            if (f.Contains(page, id))
               return f.Find(page, id);

         // Not found
         throw StringNotFoundException(HERE, page, id);
      }

		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

   }
}
