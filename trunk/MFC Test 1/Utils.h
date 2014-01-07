#pragma once
#include "Resource.h"
#include "STL.h"
#include "Macros.h"

#ifdef FormatMessage
#undef FormatMessage
#endif


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

      static wstring  FormatMessage(UINT  id);

      // ----------------------- MUTATORS ------------------------

		// -------------------- REPRESENTATION ---------------------
   };

   /// <summary>Represents a file or folder path</summary>
   class Path : public wstring
   {
      // --------------------- CONSTRUCTION ----------------------

   public:
      Path() {}
      Path(const WCHAR* path) : wstring(path) {}
      Path(wstring path) : wstring(path) {}

      // --------------------- PROPERTIES ------------------------
			
		// ---------------------- ACCESSORS ------------------------
		
      bool  Exists();
      bool  IsDirectory();

		// ------------------------ STATIC -------------------------

		// ----------------------- MUTATORS ------------------------

      Path&  AppendBackslash();
      Path&  AppendPath(wstring  path);
      Path&  RenameExtension(wstring  ext);
      Path&  RemoveBackslash();

		// -------------------- REPRESENTATION ---------------------

   };

   /// <summary>Provides the ability to search for files</summary>
   class FileSearch
   {
      // --------------------- CONSTRUCTION ----------------------

   public:
      FileSearch(wstring  term);
      ~FileSearch();

      // --------------------- PROPERTIES ------------------------
			
		// ---------------------- ACCESSORS ------------------------
			
		// ------------------------ STATIC -------------------------

		// ----------------------- MUTATORS ------------------------

      void  Close();
      bool  Exists();
      bool  Next();

      // -------------------- REPRESENTATION ---------------------

   private:
      WIN32_FIND_DATA Data;
      HANDLE          Handle;
   };


}