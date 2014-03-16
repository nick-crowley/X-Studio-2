#include "stdafx.h"
#include "FileSearch.h"
#include <Shlwapi.h>

namespace Logic
{
   namespace IO
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Create a file search from a search term</summary>
      /// <param name="query">Pattern to search for</param>
      FileSearch::FileSearch(Path query) : Handle(NULL), Folder(query.Folder)
      {
         ZeroMemory(&Data, sizeof(WIN32_FIND_DATA));

         // Find first file
         Handle = FindFirstFileEx(query.c_str(), FindExInfoBasic, &Data, FindExSearchNameMatch, NULL, NULL);
      }

      /// <summary>Closes the file search</summary>
      FileSearch::~FileSearch()
      {
         Close();
      }

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Determines whether current result is valid</summary>
      /// <returns></returns>
      bool  FileSearch::HasResult()
      {
         return Handle != INVALID_HANDLE_VALUE;
      }

      /// <summary>Closes the query</summary>
      void  FileSearch::Close()
      {
         if (Handle != INVALID_HANDLE_VALUE)
         {
            FindClose(Handle);
            Handle = INVALID_HANDLE_VALUE;
         }
      }

      /// <summary>Get next result</summary>
      void  FileSearch::Next()
      {
         // Find next result
         if (HasResult() && !FindNextFile(Handle, &Data))
            Close();
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

	   // ------------------------------- PRIVATE METHODS ------------------------------
   }
}