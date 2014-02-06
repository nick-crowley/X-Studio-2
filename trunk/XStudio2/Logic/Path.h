#pragma once
//#include "Common.h"

namespace Logic
{
   namespace IO
   {
      /// <summary>Represents a file or folder path</summary>
      class Path 
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         Path();
         Path(const WCHAR* path);
         Path(const wstring& path);
         Path(const Path& r);
         Path(Path&& r);

         // ------------------------ STATIC -------------------------

      private:
         static CharArrayPtr  Init(const WCHAR*  path);

         // --------------------- PROPERTIES ------------------------
		
      public:
         PROPERTY_GET(wstring,Extension,GetExtension);
         PROPERTY_GET(wstring,FileName,GetFileName);
         PROPERTY_GET(wstring,Folder,GetFolder);
         //PROPERTY_GET(wstring,Text,GetText);

		   // ---------------------- ACCESSORS ------------------------
		
         Path     AppendBackslash() const;
         Path     Append(wstring  path) const;
         bool     Exists() const;
         wstring  GetExtension() const;
         wstring  GetFileName() const;
         wstring  GetFolder() const;
         //wstring  GetText() const;
         bool     HasExtension(const WCHAR* ext) const;
         bool     HasExtension(wstring ext) const;
         bool     IsDirectory() const;
         Path     RemoveBackslash() const;
         Path     RemoveExtension() const;
         Path     RenameExtension(wstring  ext) const;

         Path& operator=(const WCHAR* path)     { return Assign(path);               }
         Path& operator=(const wstring& path)   { return Assign(path.c_str());       }
         Path& operator=(const Path& path)      { return Assign(path.Buffer.get());  }
         Path& operator=(Path&& r);

         Path operator+(const WCHAR* path) const      { return Append(path);               }
         Path operator+(const wstring& path) const    { return Append(path);               }
         Path operator+(const Path& path) const       { return Append(path.c_str());       }

         bool operator==(const WCHAR* path) const     { return Compare(path) == 0;               }
         bool operator==(const wstring& path) const   { return Compare(path.c_str()) == 0;       }
         bool operator==(const Path& path) const      { return Compare(path.Buffer.get()) == 0;  }

         bool operator!=(const WCHAR* path) const     { return Compare(path) != 0;               }
         bool operator!=(const wstring& path) const   { return Compare(path.c_str()) != 0;       }
         bool operator!=(const Path& path) const      { return Compare(path.Buffer.get()) != 0;  }

         bool operator<(const Path& path) const       { return Compare(path.Buffer.get()) < 0;   }
         bool operator>(const Path& path) const       { return Compare(path.Buffer.get()) > 0;   }

         //operator WCHAR*()                            { return Buffer.get(); }
         //operator const WCHAR*() const                { return Buffer.get(); }
         const WCHAR* c_str() const                   { return Buffer.get(); }
         //operator wstring()                           { return Buffer.get(); }

      private:
         CharArrayPtr  Copy() const;
         int  Compare(const WCHAR* path) const;
      
		   // ----------------------- MUTATORS ------------------------
   
      private:
         Path&  Assign(const WCHAR*  text);
      
		   // -------------------- REPRESENTATION ---------------------

      private:
         CharArrayPtr  Buffer;
      };

   }
}