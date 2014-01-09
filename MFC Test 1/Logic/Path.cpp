#include "stdafx.h"
#include "Common.h"
#include <Strsafe.h>       // C String handling
#include "Shlwapi.h"       // PathFileExists

namespace Library
{
   // -------------------------------- CONSTRUCTION --------------------------------

   /// <summary>Create an initially empty path</summary>
   Path::Path() : Buffer(Init(L""))
   {
   }

   /// <summary>Create path from a char array</summary>
   /// <param name="path">The path.</param>
   /// <exception cref="Library::ArgumentNullException">Path is null</exception>
   Path::Path(const WCHAR* path) : Buffer(Init(path))
   {
   }

   /// <summary>Create path from a string</summary>
   /// <param name="path">The path.</param>
   Path::Path(wstring path) : Buffer(Init(path.c_str()))
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

   // ------------------------------- STATIC METHODS ------------------------------

   /// <summary>Creates a MAX_PATH char array buffer containing a path</summary>
   /// <param name="path">The initial path</param>
   /// <returns></returns>
   /// <exception cref="Library::ArgumentNullException">Path is null</exception>
   Path::CharPtr  Path::Init(const WCHAR*  path)
   {
      REQUIRED(path);

      CharPtr  ptr(new WCHAR[MAX_PATH]);
      StringCchCopy(ptr.get(), MAX_PATH, path);
      return ptr;
   }

	// ------------------------------- PUBLIC METHODS -------------------------------

   /// <summary>Appends another path, handling trailing backslashes as necessary</summary>
   /// <param name="path">The path to append</param>
   /// <returns>New path containing combination of both paths</returns>
   Path  Path::Append(wstring  path) const
   {
      CharPtr buf( Copy() );

      // Supply copy of buffer
      if (!PathAppend(buf.get(), path.c_str()))
         throw Win32Exception(HERE, L"Unable to append path");
      return buf.get();
   }

   /// <summary>Appends a trailing backslash if necessary</summary>
   /// <returns>New path with the trailing backslash</returns>
   Path  Path::AppendBackslash() const
   {
      CharPtr buf( Copy() );

      // Supply copy of buffer
      PathAddBackslash(buf.get());
      return buf.get();
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
   /// <returns>Filename if found, otherwise empty string</returns>
   wstring  Path::GetFileName() const
   {
      WCHAR* fn = PathFindFileName(Buffer.get());
      return (fn != Buffer.get() ? fn : L"");
   }

   /// <summary>Gets the folder portion of the path only</summary>
   /// <returns>Folder including a trailing backslash</returns>
   wstring  Path::GetFolder() const
   {
      CharPtr buf( Copy() );

      // Supply copy of buffer
      PathRemoveFileSpec(buf.get());
      PathAddBackslash(buf.get());

      return buf.get();
   }

   /// <summary>Get path<summary>
   /// <returns></returns>
   wstring  Path::GetText() const
   {
      return Buffer.get();
   }

   /// <summary>Determines whether path has a given extension (case insensitive)</summary>
   /// <param name="ext">The extention preceeded by a dot</param>
   /// <returns></returns>
   /// <exception cref="Library::ArgumentNullException">Path is null</exception>
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
   /// <exception cref="Library::Win32Exception">Path does not exist</exception>
   bool  Path::IsDirectory() const
   {
      int attr = GetFileAttributes(Buffer.get());
      
      if (attr == INVALID_FILE_ATTRIBUTES)
         throw Win32Exception(HERE);

      return (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
   }

   /// <summary>Move assignment</summary>
   /// <param name="path">Path to assign</param>
   /// <returns>this</returns>
   Path& Path::operator=(Path&& r)
   {
      // Ensure not self-assignment
      if (r != *this)
      {
         Buffer.release();
         Buffer = std::move(r.Buffer);
      }
      return *this;
   }

   /// <summary>Removes the trailing backslash, if any</summary>
   /// <returns>New path without the trailing backslash</returns>
   Path  Path::RemoveBackslash() const
   {
      CharPtr buf( Copy() );

      // Supply copy of buffer
      PathRemoveBackslash(buf.get());
      return buf.get();
   }

   /// <summary>Removes the file extension, if present</summary>
   /// <returns>New path without the extension</returns>
   /// <exception cref="Library.Win32Exception">Path exceeds character limit</exception>
   Path  Path::RemoveExtension() const
   {
      CharPtr buf( Copy() );

      // Supply copy of buffer
      PathRemoveExtension(buf.get());
      return buf.get();
   }

   /// <summary>Renames the file extension, if none is present then it is appended</summary>
   /// <param name="ext">The file extension, preceeded by a dot</param>
   /// <returns>New path with renamed extension</returns>
   /// <exception cref="Library.Win32Exception">Path exceeds character limit</exception>
   Path  Path::RenameExtension(wstring  ext) const
   {
      CharPtr buf( Copy() );

      // Supply copy of buffer
      if (!PathRenameExtension(buf.get(), ext.c_str()))
         throw Win32Exception(HERE, L"Unable to rename extension");
      
      return buf.get();
   }
   
   // ------------------------------ PROTECTED METHODS -----------------------------

	// ------------------------------- PRIVATE METHODS ------------------------------

   /// <summary>Assigns the specified path</summary>
   /// <param name="path">The path</param>
   /// <returns>this</returns>
   /// <exception cref="Library::ArgumentNullException">Path is null</exception>
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
   /// <exception cref="Library::ArgumentNullException">Path is null</exception>
   int   Path::Compare(const WCHAR* path) const
   {
      REQUIRED(path);

      return StrCmpI(Buffer.get(), path);
   }

   /// <summary>Duplicate the buffer into a MAX_PATH chara array<summary>
   /// <returns></returns>
   Path::CharPtr  Path::Copy() const
   {
      CharPtr  ptr(new WCHAR[MAX_PATH]);
      StringCchCopy(ptr.get(), MAX_PATH, Buffer.get());
      return ptr;
   }

}