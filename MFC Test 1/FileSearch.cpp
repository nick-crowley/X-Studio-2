#include "stdafx.h"
#include "Common.h"

namespace Library
{

   FileSearch::FileSearch(wstring  term)
   {
      ZeroMemory(&Data, sizeof(WIN32_FIND_DATA));
      Handle = FindFirstFileEx(term.c_str(), FindExInfoBasic, &Data, FindExSearchNameMatch, NULL, NULL);
   }

   FileSearch::~FileSearch()
   {
      Close();
   }

   bool  FileSearch::Exists()
   {
      return Handle != INVALID_HANDLE_VALUE;
   }

   void  FileSearch::Close()
   {
      if (Handle != INVALID_HANDLE_VALUE)
      {
         CloseHandle(Handle);
         Handle = INVALID_HANDLE_VALUE;
      }
   }

   bool  FileSearch::Next()
   {
      // Find next result
      if (Exists() && !FindNextFile(Handle, &Data))
         Close();

      return Exists();
   }

}