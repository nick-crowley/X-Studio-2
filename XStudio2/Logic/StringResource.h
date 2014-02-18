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
      static wstring  Missing(UINT id);

      // ----------------------- MUTATORS ------------------------

		// -------------------- REPRESENTATION ---------------------
   };

   /// <summary>Indentation string, used for debugging output</summary>
   class Indent : public wstring
   {
   public:
      Indent(UINT depth) : wstring(depth, L' ')
      {}
   };

   /// <summary>A formattable GUI string</summary>
   class GuiString : public wstring
   {
   public:
      /// <summary>Create empty string</summary>
      GuiString()
      {}

      /// <summary>Copy move ctor</summary>
      GuiString(GuiString&& r) : wstring(move(r))
      {}

      /// <summary>Copy move ctor</summary>
      GuiString(wstring&& r) : wstring(move(r))
      {}

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

      /// <summary>Creates string from iterators</summary>
      /// <param name="from">start</param>
      /// <param name="to">end</param>
      GuiString(const_iterator from, const_iterator to) : wstring(from, to)
      {}

      /// <summary>Copy create from wstring.</summary>
      /// <param name="sz">The string</param>
      GuiString(const wstring& sz) : wstring(sz)
      {}

      /// <summary>Move create from wstring</summary>
      /// <param name="sz">The string</param>
      GuiString(const wstring&& sz) : wstring(move(sz))
      {}

      /// <summary>Test for presence of a substring</summary>
      /// <param name="str">The substring.</param>
      /// <param name="matchCase">Whether to perform case sensitive search</param>
      /// <returns></returns>
      bool Contains(const wstring& str, bool matchCase) const
      {
         return matchCase ? StrStrI(c_str(), str.c_str()) != nullptr : find(str) != npos;
      }
      
      /// <summary>Perform case insensitive comparison</summary>
      /// <param name="r">Other string</param>
      /// <returns></returns>
      bool Equals(const wstring& r) const
      {
         return StrCmpI(c_str(), r.c_str()) == 0;
      }

      /// <summary>Extracts n characters from start of the string</summary>
      /// <param name="chars">Number of characters</param>
      /// <returns>New string containing desired characters, or the entire string if chars is greater than length</returns>
      GuiString Left(UINT chars) const
      {
         return chars < length() ? GuiString(begin(),begin()+chars) : *this;
      }
      
      /// <summary>Extracts n characters from end of the string</summary>
      /// <param name="chars">Number of characters</param>
      /// <returns>New string containing desired characters, or the entire string if chars is greater than length</returns>
      GuiString Right(UINT chars) const
      {
         return chars < length() ? GuiString(end()-chars,end()) : *this;
      }

      /// <summary>Removes all instances of given characters</summary>
      /// <param name="chars">The chars.</param>
      /// <returns>Copy of string with characters removed</returns>
      GuiString  Remove(const WCHAR* chars) const
      {
         GuiString s(*this);

         // Iterate thru characters
         for (const wchar* ch = chars; ch[0]; ++ch)
            // Strip all instances of character
            for (auto pos = s.find_first_of(ch[0]); pos != GuiString::npos; pos = s.find_first_of(ch[0]))
               s.erase(pos, 1);

         return s;
      }
      
      /// <summary>Convert string to lower case</summary>
      /// <returns>New string in lower case</returns>
      GuiString  ToLower() const
      {
         GuiString s(*this);
         for (auto& ch : s)
            ch = towlower(ch);
         return s;
      }
      
      /// <summary>Convert string to upper case</summary>
      /// <returns>New string in upper case</returns>
      GuiString  ToUpper() const
      {
         GuiString s(*this);
         for (auto& ch : s)
            ch = towupper(ch);
         return s;
      }

      /// <summary>Trims leading and trailing characters</summary>
      /// <param name="chars">The chars.</param>
      /// <returns>Copy of string with characters removed</returns>
      GuiString  Trim(const WCHAR* chars) const
      {
         TrimLeft(chars);
         TrimRight(chars);
      }

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
         if (pos != npos)
            s.erase(s.begin()+pos+1, s.end());
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