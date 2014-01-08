#pragma once
#include "Resource.h"
#include "STL.h"
#include "Macros.h"

namespace Library
{
   /// <summary>A unique pointer to a byte array</summary>
   typedef unique_ptr<BYTE, default_delete<BYTE[]>>  ByteArrayPtr;

   /// <summary>Provides helper functions for loading/formatting strings from resources</summary>
   class StringResource
   {
      // --------------------- CONSTRUCTION ----------------------

		// --------------------- PROPERTIES ------------------------
			
		// ---------------------- ACCESSORS ------------------------
			
		// ------------------------ STATIC -------------------------

   public:
      static wstring  Format(const WCHAR*  format, ...);
      static wstring  Load(UINT  id, ...);
      static wstring  LoadV(UINT  id, va_list  pArgs);

      static wstring  FromSystem(UINT  id);

      // ----------------------- MUTATORS ------------------------

		// -------------------- REPRESENTATION ---------------------
   };

   /// <summary>Represents a file or folder path</summary>
   class Path 
   {
      typedef unique_ptr<WCHAR, default_delete<WCHAR[]>>  CharPtr;

      // --------------------- CONSTRUCTION ----------------------

   public:
      Path();
      Path(const WCHAR* path);
      Path(wstring path);
      Path(const Path& r);
      Path(Path&& r);

      // ------------------------ STATIC -------------------------

   private:
      CharPtr  Init(const WCHAR*  path);

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
      //operator wstring()                           { return Buffer.get(); }

   private:
      CharPtr  Copy() const;
      int      Compare(const WCHAR* path) const;
      
		// ----------------------- MUTATORS ------------------------
   
   private:
      Path&    Assign(const WCHAR*  text);
      
		// -------------------- REPRESENTATION ---------------------

   private:
      CharPtr  Buffer;
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
      PROPERTY_GET(DWORD,FileSize,GetFileSize);
      PROPERTY_GET(wstring,FullPath,GetFullPath);

		// ---------------------- ACCESSORS ------------------------

      DWORD   GetAttributes()  { return Data.dwFileAttributes; }
      DWORD   GetFileSize()    { return Data.nFileSizeLow;     }
      wstring GetFileName()    { return Data.cFileName;        }
      wstring GetFullPath()    { return (Folder+Data.cFileName).Text; }

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