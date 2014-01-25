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

      /// <summary>Copy create from wstring.</summary>
      /// <param name="sz">The string</param>
      GuiString(const wstring& sz) : wstring(sz)
      {}

      /// <summary>Move create from wstring</summary>
      /// <param name="sz">The string</param>
      GuiString(const wstring&& sz) : wstring(move(sz))
      {}

      /// <summary>Trims leading characters</summary>
      /// <param name="chars">The chars.</param>
      /// <returns>Copy of string with characters removed</returns>
      GuiString  TrimLeft(const WCHAR* chars) const
      {
         GuiString s(*this);

         auto pos = s.find_first_not_of(chars);
         // Strip chars
         if (pos != 0 && pos != npos)
            s.erase(0, pos);
         return s;
      }

      /// <summary>Trims trailing characters</summary>
      /// <param name="chars">The chars.</param>
      /// <returns>Copy of string with characters removed</returns>
      GuiString  TrimRight(const WCHAR* chars) const
      {
         GuiString s(*this);

         auto pos = s.find_last_not_of(chars);
         // Strip chars
         if (pos != s.size()-1 && pos != npos)
            s.erase(s.begin()+pos, s.end());
         return s;
      }

      /// <summary>Convert string to lower case</summary>
      /// <returns>Copy of string in lower case</returns>
      GuiString  ToLower() const
      {
         GuiString s(*this);
         for (auto& ch : s)
            ch = towlower(ch);
         return s;
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
      VersionString(GameVersion v, bool acronym = false);
   };
}