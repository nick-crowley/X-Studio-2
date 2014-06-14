#pragma once

namespace Logic
{

   /// <summary>Indentation string, used for debugging output</summary>
   class UtilExport Indent : public wstring
   {
   public:
      Indent(UINT depth) : wstring(depth, L' ')
      {}
   };

   /// <summary>A formattable GUI string</summary>
   class UtilExport GuiString : public wstring
   {
      // --------------------- CONSTRUCTION ----------------------
   public:
      GuiString();
      GuiString(GuiString&& r);
      GuiString(wstring&& r);
      GuiString(UINT id);
      GuiString(const WCHAR* src);
      GuiString(const_iterator from, const_iterator to);
      GuiString(const wstring& sz);
      GuiString(const wstring&& sz);
      
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

      // --------------------- PROPERTIES ------------------------
			
		// ---------------------- ACCESSORS ------------------------
   public:
      /// <summary>Test for presence of a substring</summary>
      /// <param name="str">The substring.</param>
      /// <param name="matchCase">Whether to perform case sensitive search</param>
      /// <returns></returns>
      bool Contains(const wstring& str, bool matchCase) const;

      /// <summary>Perform case insensitive comparison</summary>
      /// <param name="r">Other string</param>
      /// <returns></returns>
      bool Equals(const wstring& r) const;

      /// <summary>Find index of a substring</summary>
      /// <param name="str">The substring.</param>
      /// <param name="matchCase">Whether to perform case sensitive search</param>
      /// <returns>Offset or npos</returns>
      int Find(const wstring& str, bool matchCase) const;

      /// <summary>Find index of a substring</summary>
      /// <param name="str">The substring.</param>
      /// <param name="offset">Start offset.</param>
      /// <param name="matchCase">Whether to perform case sensitive search</param>
      /// <returns>Offset or npos</returns>
      /// <exception cref="Logic::IndexOutOfRangeException">Invalid offset</exception>
      int Find(const wstring& str, UINT offset, bool matchCase) const;

      /// <summary>Get whether string is numeric.</summary>
      /// <returns>False if empty or non-numeric, otherwise true</returns>
      bool IsNumeric() const;

      /// <summary>Extracts n characters from start of the string</summary>
      /// <param name="chars">Number of characters</param>
      /// <returns>New string containing desired characters, or the entire string if chars is greater than length</returns>
      GuiString  Left(UINT chars) const;
      
      /// <summary>Extracts n characters from end of the string</summary>
      /// <param name="chars">Number of characters</param>
      /// <returns>New string containing desired characters, or the entire string if chars is greater than length</returns>
      GuiString  Right(UINT chars) const;

      /// <summary>Removes all instances of given characters</summary>
      /// <param name="chars">The chars.</param>
      /// <returns>Copy of string with characters removed</returns>
      GuiString  Remove(const WCHAR* chars) const;

      /// <summary>Replaces all instances of a substring with another.</summary>
      /// <param name="substring">Substring.</param>
      /// <param name="replacement">replacement.</param>
      /// <returns>Copy of string with replacements made</returns>
      GuiString  ReplaceAll(const wstring& substring, const wstring& replacement) const;

      /// <summary>Convert to int.</summary>
      /// <returns></returns>
      int  ToInt() const;

      /// <summary>Convert string to lower case</summary>
      /// <returns>New string in lower case</returns>
      GuiString  ToLower() const;

      /// <summary>Convert string to upper case</summary>
      /// <returns>New string in upper case</returns>
      GuiString  ToUpper() const;

      /// <summary>Trims leading and trailing characters</summary>
      /// <param name="chars">The chars.</param>
      /// <returns>Copy of string with characters removed</returns>
      GuiString  Trim(const WCHAR* chars) const;

      /// <summary>Trims leading characters</summary>
      /// <param name="chars">The chars.</param>
      /// <returns>Copy of string with characters removed</returns>
      GuiString  TrimLeft(const WCHAR* chars) const;

      /// <summary>Trims trailing characters</summary>
      /// <param name="chars">The chars.</param>
      /// <returns>Copy of string with characters removed</returns>
      GuiString  TrimRight(const WCHAR* chars) const;

      // ---------------------- MUTATORS ------------------------

      // ------------------- REPRESENTATION ---------------------
   };


   class UtilExport VString : public GuiString
   {
   public:
      /// <summary>Loads and formats a string</summary>
      /// <param name="format">Formatting string</param>
      /// <param name="">Arguments</param>
      /// <exception cref="Logic::ArgumentException">Insufficient buffer space</exception>
      VString(const WCHAR* format, ...)
      {
         va_list args;
         assign( GuiString::FormatV(format, va_start(args, format)) );
      }

      
      /// <summary>Loads and formats a resource string</summary>
      /// <param name="id">Resource ID</param>
      /// <param name="">Arguments</param>
      /// <exception cref="Logic::ArgumentException">Insufficient buffer space</exception>
      VString(UINT id, ...)
      {
         va_list args;
         assign( GuiString::LoadV(id, va_start(args, id)) );
      }


   };


   /// <summary>String containing a system error message</summary>
   class UtilExport SysErrorString : public wstring
   {
   public:
      /// <summary>Creates a string containing the last system error</summary>
      SysErrorString() : wstring(GuiString::FromSystem(GetLastError()))
      {}

      /// <summary>Creates a string containing the any system error</summary>
      SysErrorString(UINT id) : wstring(GuiString::FromSystem(id))
      {}
   };

   /// <summary>String holding name of a game version</summary>
   class UtilExport VersionString : public wstring
   {
   public:
      VersionString(GameVersion v, bool acronym = false);
   };
}