#pragma once
#include "../Resource.h"
#include "STL.h"
#include "Macros.h"

namespace Logic
{
   /// <summary>A unique pointer to a byte array</summary>
   typedef unique_ptr<BYTE, default_delete<BYTE[]>>  ByteArrayPtr;

   /// <summary>A unique pointer to a wide char array</summary>
   typedef unique_ptr<WCHAR, default_delete<WCHAR[]>>  CharArrayPtr;

   /// <summary>Provides helper functions for loading/formatting strings from resources</summary>
   class StringResource
   {
      // --------------------- CONSTRUCTION ----------------------

		// --------------------- PROPERTIES ------------------------
			
		// ---------------------- ACCESSORS ------------------------
			
		// ------------------------ STATIC -------------------------

   public:
      static wstring  Convert(const string& str, UINT codepage);
      static string   Convert(const wstring& str, UINT codepage);
      static wstring  Format(const WCHAR*  format, ...);
      static wstring  FormatV(const WCHAR*  format, va_list  args);
      static wstring  Load(UINT  id, ...);
      static wstring  LoadV(UINT  id, va_list  pArgs);

      static wstring  FromSystem(UINT  id);

      // ----------------------- MUTATORS ------------------------

		// -------------------- REPRESENTATION ---------------------
   };

   /// <summary></summary>
   class Console
   {
      // --------------------- CONSTRUCTION ----------------------

   public:
      Console();
      virtual ~Console();

      // ------------------------ STATIC -------------------------

      // --------------------- PROPERTIES ------------------------
			
		// ---------------------- ACCESSORS ------------------------			

      /// <summary>Writes the formatted text to the console</summary>
      /// <param name="format">Formatting string</param>
      /// <param name="...">Arguments</param>
      static void  Write(const WCHAR* format, ...)
      {
         va_list args;
         OutputDebugString(StringResource::FormatV(format, va_start(args, format)).c_str());
      }

      /// <summary>Writes the formatted text to the console</summary>
      /// <param name="format">Formatting string</param>
      /// <param name="...">Arguments</param>
      static void  Write(const wstring& format, ...)
      {
         va_list args;
         OutputDebugString(StringResource::FormatV(format.c_str(), va_start(args, format)).c_str());
      }

      /// <summary>Writes the formatted text to the console</summary>
      /// <param name="format">Formatting string</param>
      /// <param name="...">Arguments</param>
      static void  WriteLn(const WCHAR* format, ...)
      {
         va_list args;
         OutputDebugString(StringResource::FormatV(format, va_start(args, format)).c_str());
         OutputDebugString(L"\n");
      }

      /// <summary>Writes the formatted text to the console</summary>
      /// <param name="format">Formatting string</param>
      /// <param name="...">Arguments</param>
      static void  WriteLn(const wstring& format, ...)
      {
         va_list args;
         OutputDebugString(StringResource::FormatV(format.c_str(), va_start(args, format)).c_str());
         OutputDebugString(L"\n");
      }

		// ----------------------- MUTATORS ------------------------

		// -------------------- REPRESENTATION ---------------------

   private:
   };

   /// <summary>A formattable GUI string</summary>
   class GuiString : public wstring
   {
   public:
      /// <summary>Loads and formats a resource string</summary>
      /// <param name="id">Resource ID</param>
      /// <param name="">Arguments</param>
      GuiString(UINT id, ...)
      {
         va_list args;
         assign( StringResource::LoadV(id, va_start(args, id)) );
      }

      /// <summary>Loads and formats a string</summary>
      /// <param name="format">Formatting string</param>
      /// <param name="">Arguments</param>
      GuiString(const WCHAR* format, ...)
      {
         va_list args;
         assign( StringResource::FormatV(format, va_start(args, format)) );
      }
   };


   
   /// <summary>String containing a system error message</summary>
   class SysErrorString : public wstring
   {
   public:
      /// <summary>Creates a string containing the last system error</summary>
      SysErrorString() { assign(StringResource::FromSystem(GetLastError())); }

      /// <summary>Creates a string containing the any system error</summary>
      SysErrorString(UINT id) { assign(StringResource::FromSystem(id));             }
   };

   /// <summary>Represents a file or folder path</summary>
   class Path 
   {
      // --------------------- CONSTRUCTION ----------------------
   public:
      Path();
      Path(const WCHAR* path);
      Path(wstring path);
      Path(const Path& r);
      Path(Path&& r);

      // ------------------------ STATIC -------------------------

   private:
      CharArrayPtr  Init(const WCHAR*  path);

      // --------------------- PROPERTIES ------------------------
		
   public:
      PROPERTY_GET(wstring,Extension,GetExtension);
      PROPERTY_GET(wstring,FileName,GetFileName);
      PROPERTY_GET(wstring,Folder,GetFolder);
      PROPERTY_GET(wstring,Text,GetText);

		// ---------------------- ACCESSORS ------------------------
		
      Path     AppendBackslash() const;
      Path     Append(wstring  path) const;
      bool     Exists() const;
      wstring  GetExtension() const;
      wstring  GetFileName() const;
      wstring  GetFolder() const;
      wstring  GetText() const;
      bool     HasExtension(const WCHAR* ext) const;
      bool     HasExtension(wstring ext) const;
      bool     IsDirectory() const;
      Path     RemoveBackslash() const;
      Path     RemoveExtension() const;
      Path     RenameExtension(wstring  ext) const;

      Path& operator=(const WCHAR* path)     { return Assign(path);               }
      Path& operator=(const wstring path)    { return Assign(path.c_str());       }
      Path& operator=(const Path& path)      { return Assign(path.Buffer.get());  }
      Path& operator=(Path&& r);

      Path operator+(const WCHAR* path)      { return Append(path);               }
      Path operator+(const wstring path)     { return Append(path);               }
      Path operator+(const Path& path)       { return Append(path.Text);          }

      bool operator==(const WCHAR* path) const     { return Compare(path) == 0;               }
      bool operator==(const wstring path) const    { return Compare(path.c_str()) == 0;       }
      bool operator==(const Path& path) const      { return Compare(path.Buffer.get()) == 0;  }

      bool operator!=(const WCHAR* path) const     { return Compare(path) != 0;               }
      bool operator!=(const wstring path) const    { return Compare(path.c_str()) != 0;       }
      bool operator!=(const Path& path) const      { return Compare(path.Buffer.get()) != 0;  }

      bool operator<(const Path& path) const       { return Compare(path.Buffer.get()) < 0;   }
      bool operator>(const Path& path) const       { return Compare(path.Buffer.get()) > 0;   }

      operator WCHAR*()                            { return Buffer.get(); }
      operator const WCHAR*() const                { return Buffer.get(); }
      const WCHAR* c_str() const                   { return Buffer.get(); }
      //operator wstring()                           { return Buffer.get(); }

   private:
      CharArrayPtr  Copy() const;
      int      Compare(const WCHAR* path) const;
      
		// ----------------------- MUTATORS ------------------------
   
   private:
      Path&    Assign(const WCHAR*  text);
      
		// -------------------- REPRESENTATION ---------------------

   private:
      CharArrayPtr  Buffer;
   };

   /// <summary>Provides the ability to search for files</summary>
   class FileSearch
   {
      // --------------------- CONSTRUCTION ----------------------

   public:
      FileSearch(Path query);
      ~FileSearch();

      // ------------------------ STATIC -------------------------

      // --------------------- PROPERTIES ------------------------
			
      PROPERTY_GET(DWORD,Attributes,GetAttributes);
      PROPERTY_GET(wstring,FileName,GetFileName);
      PROPERTY_GET(DWORD,FileSize,GetFileSize);
      PROPERTY_GET(Path,FullPath,GetFullPath);

		// ---------------------- ACCESSORS ------------------------

      DWORD   GetAttributes()  { return Data.dwFileAttributes; }
      DWORD   GetFileSize()    { return Data.nFileSizeLow;     }
      wstring GetFileName()    { return Data.cFileName;        }
      Path    GetFullPath()    { return (Folder+Data.cFileName).Text; }
      bool    IsDirectory()    { return (GetAttributes() & FILE_ATTRIBUTE_DIRECTORY) != 0; }

      bool  HasResult();

		// ----------------------- MUTATORS ------------------------

      void  Close();
      void  Next();

      // -------------------- REPRESENTATION ---------------------

   private:
      WIN32_FIND_DATA Data;
      HANDLE          Handle;
      Path            Folder;
   };


}