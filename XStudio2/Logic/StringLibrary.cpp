#include "stdafx.h"
#include "StringLibrary.h"
#include "LanguageFileReader.h"
#include "StringResolver.h"

namespace Logic
{
   namespace Language
   {
      StringLibrary  StringLibrary::Instance;

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
      /// <param name="data">Background worker data</param>
      /// <returns>Number of files found</returns>
      UINT  StringLibrary::Enumerate(XFileSystem& vfs, GameLanguage lang, WorkerData* data)
      {
         list<XFileInfo> results;

         // Clear previous contents
         Clear();

         // Feedback
         data->SendFeedback(Cons::Heading, ProgressType::Operation, 1, GuiString(L"Enumerating %s language files", GetString(lang).c_str()));

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
            try
            {
               // Feedback
               data->SendFeedback(ProgressType::Info, 2, GuiString(L"Reading language file '%s'...", f.FullPath.c_str()));
               Console << L"Reading language file: " << f.FullPath << L"...";

               // Parse language file
               LanguageFile file = LanguageFileReader(f.OpenRead()).ReadFile(f.FullPath);

               // check language tag matches filename
               if (file.Language == lang)
               {
                  Files.insert(move(file));
                  Console << Cons::Green << L"Success" << ENDL;
               }
               else
               {  // Skip files that turn out to be foreign
                  data->SendFeedback(ProgressType::Warning, 3, GuiString(L"Skipping %s language file...", GetString(file.Language).c_str()) );
                  Console << Cons::Yellow << L"Skipped" << ENDL;
               }
            }
            catch (ExceptionBase& e) {
               data->SendFeedback(Cons::Error, ProgressType::Error, 3, GuiString(L"Failed: ") + e.Message);
               
               // SkipBroken: Abort/Continue after error
               if (!PrefsLib.SkipBrokenFiles)
                  throw;
            }
         }

         return Files.size();
      }

      /// <summary>Clears the library of all files/strings</summary>
      void  StringLibrary::Clear()
      {
         Files.clear();
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
      const LanguageString&  StringLibrary::Find(UINT page, UINT id) const
      {
         // Search files in descending file ID
         for (const LanguageFile& f : Files)
            if (f.Contains(page, id))
               return f.Find(page, id);

         // Not found
         throw StringNotFoundException(HERE, page, id);
      }

      /// <summary>Finds a string, resolves the substrings and removes the comments</summary>
      /// <param name="page">The page id</param>
      /// <param name="id">The string id</param>
      /// <returns>Resolved text</returns>
      /// <exception cref="Logic::PageNotFoundException">Page does not exist</exception>
      /// <exception cref="Logic::StringNotFoundException">String does not exist</exception>
      wstring  StringLibrary::Resolve(UINT page, UINT id) const
      {
         return Find(page,id).ResolvedText;
      }

      /// <summary>Attempts to find a string</summary>
      /// <param name="page">The page id</param>
      /// <param name="id">The string id</param>
      /// <param name="str">The string</param>
      /// <returns>true if found, false otherwise</returns>
      bool  StringLibrary::TryFind(UINT page, UINT id, const LanguageString* &str) const
      {
         str = nullptr;

         // Search files in descending file ID
         for (const LanguageFile& f : Files)
            if (f.Contains(page, id))
            {
               str = &f.Find(page, id);
               return true;
            }

         // Not found
         return false;
      }

		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

   }
}
