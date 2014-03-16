#include "stdafx.h"
#include "Path.h"
#include <Strsafe.h>       // C String handling
#include "Shlwapi.h"       // PathFileExists
//#include "../Logic/XFileSystem.h"   // XFileSystem::GetPath(.)

namespace Logic
{
   // -------------------------------- CONSTRUCTION --------------------------------

   /// <summary>Create an initially empty path</summary>
   Path::Path() : Buffer(Init(L""))
   {
   }

   /// <summary>Create path from a char array</summary>
   /// <param name="path">The path.</param>
   /// <exception cref="Logic::ArgumentNullException">Path is null</exception>
   Path::Path(const WCHAR* path) : Buffer(Init(path))
   {
   }

   /// <summary>Create path from a string</summary>
   /// <param name="path">The path.</param>
   Path::Path(const wstring& path) : Buffer(Init(path.c_str()))
   {
   }

   /// <summary>Copies a path</summary>
   /// <param name="r">The source path</param>
   Path::Path(const Path& r) : Buffer(r.Copy())
   {
   }

   /// <summary>Moves a path</summary>
   /// <param name="r">The source path</param>
   Path::Path(Path&& r) : Buffer(std::move(r.Buffer))
   {
   }

   /// <summary>dtor</summary>
   Path::~Path()
   {}

   /// <summary>Create path of application folder</summary>
   /// <exception cref="Logic::Win32Exception">API function failed</exception>
   AppPath::AppPath()
   {
      // Get app path
      if (!GetModuleFileName(NULL, Buffer.get(), MAX_PATH))
         throw Win32Exception(HERE, L"Unable to get application path");
            
      // Remove filename
      Assign(RenameFileName(L"").c_str());
   }

   /// <summary>Create path of file in the application format</summary>
   /// <param name="filename">filename</param>
   /// <exception cref="Logic::Win32Exception">API function failed</exception>
   AppPath::AppPath(const wstring& filename)
   {
      // Get app path
      if (!GetModuleFileName(NULL, Buffer.get(), MAX_PATH))
         throw Win32Exception(HERE, L"Unable to get application path");
            
      // Rename 
      Assign(RenameFileName(filename).c_str());
   }

   /// <summary>Create temporary file path</summary>
   /// <param name="prefix">three letter filename prefix</param>
   /// <exception cref="Logic::Win32Exception">API function failed</exception>
   TempPath::TempPath(const wchar* prefix)
   {
      Path folder;

      // Get temp folder
      if (!GetTempPath(MAX_PATH, (wchar*)folder))
         throw Win32Exception(HERE, L"Unable to get temp folder");
            
      // Generate filename
      if (!GetTempFileName(folder.c_str(), prefix, NULL, Buffer.get()))
         throw Win32Exception(HERE, L"Unable to generate temporary filename");
   }

   // ------------------------------- STATIC METHODS ------------------------------

   /// <summary>Creates a MAX_PATH char array buffer containing a path</summary>
   /// <param name="path">The initial path</param>
   /// <returns></returns>
   /// <exception cref="Logic::ArgumentNullException">Path is null</exception>
   CharArrayPtr  Path::Init(const WCHAR*  path)
   {
      REQUIRED(path);

      CharArrayPtr  ptr(new WCHAR[MAX_PATH+1]);
      StringCchCopy(ptr.get(), MAX_PATH, path);
      return ptr;
   }

	// ------------------------------- PUBLIC METHODS -------------------------------

   /// <summary>Appends another path, handling trailing backslashes as necessary</summary>
   /// <param name="path">The path to append</param>
   /// <returns>New path containing combination of both paths</returns>
   /// <exception cref="Logic::IOException">An I/O error occurred</exception>
   Path  Path::Append(wstring  path) const
   {
      CharArrayPtr buf( Copy() );

      // Supply copy of buffer
      if (!PathAppend(buf.get(), path.c_str()))
         throw IOException(HERE, L"Unable to append path");
      return buf.get();
   }

   /// <summary>Appends a trailing backslash if necessary</summary>
   /// <returns>New path with the trailing backslash</returns>
   Path  Path::AppendBackslash() const
   {
      CharArrayPtr buf( Copy() );

      // Supply copy of buffer
      PathAddBackslash(buf.get());
      return buf.get();
   }
      
   /// <summary>Determines whether path is empty</summary>
   /// <returns>true/false</returns>
   bool  Path::Empty() const
   { 
      return lstrlen(Buffer.get()) == 0; 
   }

   /// <summary>Determines whether a file or folder exists</summary>
   /// <returns>true/false</returns>
   bool  Path::Exists() const
   { 
      return PathFileExists(Buffer.get()) != FALSE; 
   }

   /// <summary>Get the file extension</summary>
   /// <returns>File Extension if found (with leading dot), otherwise empty string</returns>
   wstring  Path::GetExtension() const
   {
      WCHAR* fn = PathFindExtension(Buffer.get());
      return fn;
   }

   /// <summary>Get the file name</summary>
   /// <returns>Filename if found, otherwise whole path</returns>
   wstring  Path::GetFileName() const
   {
      WCHAR* fn = PathFindFileName(Buffer.get());
      //return (fn != Buffer.get() ? fn : L"");
      return fn;
   }

   /// <summary>Gets the folder portion of the path only</summary>
   /// <returns>Folder including a trailing backslash</returns>
   Path  Path::GetFolder() const
   {
      CharArrayPtr buf( Copy() );

      // Supply copy of buffer
      PathRemoveFileSpec(buf.get());
      PathAddBackslash(buf.get());

      return buf.get();
   }

   /// <summary>Determines whether path has a given extension (case insensitive)</summary>
   /// <param name="ext">The extention preceeded by a dot</param>
   /// <returns></returns>
   /// <exception cref="Logic::ArgumentNullException">Path is null</exception>
   bool  Path::HasExtension(const WCHAR* ext) const
   {
      REQUIRED(ext);

      // Optimization for usage with literals
      return StrCmpI(PathFindExtension(Buffer.get()), ext) == 0;
   }

   /// <summary>Determines whether path has a given extension (case insensitive)</summary>
   /// <param name="ext">The extention preceeded by a dot</param>
   /// <returns></returns>
   bool  Path::HasExtension(wstring ext) const
   {
      return StrCmpI(GetExtension().c_str(), ext.c_str()) == 0;
   }

   /// <summary>Determines whether path is directory</summary>
   /// <returns></returns>
   /// <exception cref="Logic::IOException">An I/O error occurred</exception>
   bool  Path::IsDirectory() const
   {
      int attr = GetFileAttributes(Buffer.get());
      
      if (attr == INVALID_FILE_ATTRIBUTES)
         throw IOException(HERE, SysErrorString());

      return (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
   }

   /// <summary>Move assignment</summary>
   /// <param name="path">Path to assign</param>
   /// <returns>this</returns>
   Path& Path::operator=(Path&& r)
   {
      // Ensure not self-assignment
      if (&r != this)
      {
         Buffer.reset(nullptr);
         Buffer.swap(r.Buffer);
      }
      return *this;
   }

   /// <summary>Removes the trailing backslash, if any</summary>
   /// <returns>New path without the trailing backslash</returns>
   Path  Path::RemoveBackslash() const
   {
      CharArrayPtr buf( Copy() );

      // Supply copy of buffer
      PathRemoveBackslash(buf.get());
      return buf.get();
   }

   /// <summary>Removes the file extension, if present</summary>
   /// <returns>New path without the extension</returns>
   Path  Path::RemoveExtension() const
   {
      CharArrayPtr buf( Copy() );

      // Supply copy of buffer
      PathRemoveExtension(buf.get());
      return buf.get();
   }

   /// <summary>Renames the file extension, if none is present then it is appended</summary>
   /// <param name="ext">The file extension, preceeded by a dot</param>
   /// <returns>New path with renamed extension</returns>
   /// <exception cref="Logic::IOException">An I/O error occurred</exception>
   Path  Path::RenameExtension(wstring  ext) const
   {
      CharArrayPtr buf( Copy() );

      // Supply copy of buffer
      if (!PathRenameExtension(buf.get(), ext.c_str()))
         throw IOException(HERE, L"Unable to rename extension");
      
      return buf.get();
   }
      
   /// <summary>Renames the filename only</summary>
   /// <param name="name">The new filename</param>
   /// <returns>New path with renamed extension</returns>
   /// <exception cref="Logic::IOException">An I/O error occurred</exception>
   Path  Path::RenameFileName(wstring  name) const
   {
      CharArrayPtr buf( Copy() );

      // Find filename
      auto fn = PathFindFileName(buf.get());

      // Replace filename
      if (StringCchCopy(fn, MAX_PATH - (fn-buf.get()), name.c_str()) == STRSAFE_E_INSUFFICIENT_BUFFER)
         throw IOException(HERE, L"New filename exceeds path character limit");
      
      return buf.get();
   }
      
   // ------------------------------ PROTECTED METHODS -----------------------------

	// ------------------------------- PRIVATE METHODS ------------------------------

   /// <summary>Assigns the specified path</summary>
   /// <param name="path">The path</param>
   /// <returns>this</returns>
   /// <exception cref="Logic::ArgumentNullException">Path is null</exception>
   Path&   Path::Assign(const WCHAR*  path)
   {
      REQUIRED(path);

      // Ensure not self-assignment
      if (path != Buffer.get())
         StringCchCopy(Buffer.get(), MAX_PATH, path);

      return *this;
   }

   /// <summary>Compares the path with another</summary>
   /// <param name="path">The path to compare against</param>
   /// <returns></returns>
   /// <exception cref="Logic::ArgumentNullException">Path is null</exception>
   int   Path::Compare(const WCHAR* path) const
   {
      REQUIRED(path);

      return StrCmpI(Buffer.get(), path);
   }

   /// <summary>Duplicate the buffer into a MAX_PATH char array</summary>
   /// <returns></returns>
   CharArrayPtr  Path::Copy() const
   {
      CharArrayPtr  ptr(new WCHAR[MAX_PATH+1]);
      StringCchCopy(ptr.get(), MAX_PATH, Buffer.get());
      return ptr;
   }
}
