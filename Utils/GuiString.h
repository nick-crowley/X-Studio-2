#pragma once

namespace Logic
{

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
      bool Equals(const wstring& r) const;
      int Find(const wstring& str, bool matchCase) const;
      int Find(const wstring& str, UINT offset, bool matchCase) const;
      bool IsNumeric() const;
      GuiString Left(UINT chars) const;
      GuiString Right(UINT chars) const;
      GuiString  Remove(const WCHAR* chars) const;
      int ToInt() const;
      GuiString  ToLower() const;
      GuiString  ToUpper() const;
      GuiString  Trim(const WCHAR* chars) const;
      GuiString  TrimLeft(const WCHAR* chars) const;
      GuiString  TrimRight(const WCHAR* chars) const;

      // ---------------------- MUTATORS ------------------------

      // ------------------- REPRESENTATION ---------------------
   };


   class VString : public GuiString
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
   class SysErrorString : public wstring
   {
   public:
      /// <summary>Creates a string containing the last system error</summary>
      SysErrorString() { assign(GuiString::FromSystem(GetLastError())); }

      /// <summary>Creates a string containing the any system error</summary>
      SysErrorString(UINT id) { assign(GuiString::FromSystem(id));             }
   };

   /// <summary>String holding name of a game version</summary>
   class VersionString : public wstring
   {
   public:
      VersionString(GameVersion v, bool acronym = false);
   };
}