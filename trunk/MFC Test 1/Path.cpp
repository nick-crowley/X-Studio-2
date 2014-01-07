#include "stdafx.h"
#include "Common.h"
#include <Strsafe.h>       // C String handling
#include "Shlwapi.h"       // PathFileExists

namespace Library
{

   /// <summary>Appends another path, handling trailing backslashes as necessary</summary>
   /// <param name="path">The path to append</param>
   /// <returns></returns>
   Path&  Path::AppendPath(wstring  path)
   {
      AppendBackslash();
      append(path);
      
      return *this;
   }

   /// <summary>Appends a trailing backslash if necessary</summary>
   /// <returns></returns>
   Path&  Path::AppendBackslash()
   {
      if (length() > 0 && back() != '\\')
         append(L"\\");

      return *this;
   }

   /// <summary>Removes the trailing backslash, if any</summary>
   /// <returns></returns>
   Path&  Path::RemoveBackslash()
   {
      if (length() > 0 && back() == '\\')
         pop_back();

      return *this;
   }

   /// <summary>Renames the file extension, if none is present then it is appended</summary>
   /// <param name="ext">The file extension, preceeded by a dot</param>
   /// <returns></returns>
   /// <exception cref="Library.Win32Exception">Path exceeds character limit</exception>
   Path&  Path::RenameExtension(wstring  ext)
   {
      auto_ptr<WCHAR>  path(new WCHAR[MAX_PATH]);
      StringCchCopy(path.get(), MAX_PATH, c_str());

      if (!PathRenameExtension(path.get(), ext.c_str()))
         throw Win32Exception(HERE, L"path exceeds character limit");

      assign(path.get());
      return *this;
   }
   
   /// <summary>Determines whether a file or folder exists</summary>
   /// <returns>true/false</returns>
   bool  Path::Exists()
   { 
      return PathFileExists(c_str()) != FALSE; 
   }

   /// <summary>Determines whether path is directory</summary>
   /// <returns></returns>
   /// <exception cref="Library::Win32Exception">Path does not exist</exception>
   bool  Path::IsDirectory()
   {
      int attr = GetFileAttributes(c_str());
      
      if (attr == INVALID_FILE_ATTRIBUTES)
         throw Win32Exception(HERE);

      return (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
   }

}