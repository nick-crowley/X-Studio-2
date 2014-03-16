#pragma once


namespace Logic
{
   namespace IO
   {
      /// <summary>Provides the ability to search for files</summary>
      class LogicExport FileSearch
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
         Path    GetFullPath()    { return Folder+Data.cFileName; }
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
}

using namespace Logic::IO;
