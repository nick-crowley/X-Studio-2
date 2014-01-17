#pragma once

namespace Logic
{
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

   /// <summary>String holding name of a game version</summary>
   class VersionString : public wstring
   {
   public:
      VersionString(GameVersion v);
   };
}