#include "stdafx.h"
#include "GuiString.h" 
#include <Strsafe.h>

namespace Logic
{
   // -------------------------------- CONSTRUCTION --------------------------------

   /// <summary>Create empty string</summary>
   GuiString::GuiString()
   {}

   /// <summary>Copy move ctor</summary>
   GuiString::GuiString(GuiString&& r) : wstring(move(r))
   {}

   /// <summary>Copy move ctor</summary>
   GuiString::GuiString(wstring&& r) : wstring(move(r))
   {}

   /// <summary>Loads a resource string</summary>
   /// <param name="id">Resource ID</param>
   GuiString::GuiString(UINT id)
   {
      assign( GuiString::Load(id) );
   }

   /// <summary>Create from char array</summary>
   /// <param name="src">Source</param>
   GuiString::GuiString(const WCHAR* src)
   {
      assign(src);
   }

   /// <summary>Creates string from iterators</summary>
   /// <param name="from">start</param>
   /// <param name="to">end</param>
   GuiString::GuiString(const_iterator from, const_iterator to) : wstring(from, to)
   {}

   /// <summary>Copy create from wstring.</summary>
   /// <param name="sz">The string</param>
   GuiString::GuiString(const wstring& sz) : wstring(sz)
   {}

   /// <summary>Move create from wstring</summary>
   /// <param name="sz">The string</param>
   GuiString::GuiString(const wstring&& sz) : wstring(move(sz))
   {}

   /// <summary>Create game version string</summary>
   /// <param name="v">version.</param>
   /// <param name="acronym">acronym or full name.</param>
   VersionString::VersionString(GameVersion v, bool acronym)
   {
      switch (v)
      {
      case GameVersion::AlbionPrelude:  assign(acronym ? L"X3AP" : L"Albion Prelude");  break;
      case GameVersion::TerranConflict: assign(acronym ? L"X3TC" : L"Terran Conflict"); break;
      case GameVersion::Reunion:        assign(acronym ? L"X3R"  : L"Reunion");         break;
      case GameVersion::Threat:         assign(acronym ? L"X2"   : L"The Threat");      break;
      case GameVersion::Rebirth:        assign(acronym ? L"X4R"  : L"Rebirth");         break;
      default: 
         throw ArgumentException(HERE, L"v", L"Unknown game version");
      }
   }

   // ------------------------------- STATIC METHODS -------------------------------

   /// <summary>Converts a narrow byte string to wide char</summary>
   /// <param name="str">The string</param>
   /// <param name="codepage">The codepage used for conversion</param>
   /// <returns>Wide char equivilent</returns>
   wstring  GuiString::Convert(const string& str, UINT codepage)
   {
      UINT buflen = str.length();
      unique_ptr<WCHAR> wide(new WCHAR[buflen+1]);
      
      // Convert and null terminate buffer
      UINT len = MultiByteToWideChar(codepage, NULL, str.c_str(), str.length(), wide.get(), buflen);
      wide.get()[min(len,buflen)] = '\0';

      return wide.get();
   }

   /// <summary>Converts a wide char string to narrow char</summary>
   /// <param name="str">The string</param>
   /// <param name="codepage">The codepage used for conversion</param>
   /// <returns>Narrow char equivilent</returns>
   string  GuiString::Convert(const wstring& str, UINT codepage)
   {
      UINT buflen = 4*str.length();
      unique_ptr<CHAR> ansi(new CHAR[buflen+1]);

      // Convert and null terminate buffer
      UINT len = WideCharToMultiByte(codepage, NULL, str.c_str(), str.length(), ansi.get(), buflen, NULL, NULL);
      ansi.get()[min(len,buflen)] = '\0';

      return ansi.get();
   }

   /// <summary>Assembles a formatted string</summary>
   /// <param name="format">Formatting string</param>
   /// <param name="">Formatting arguments</param>
   /// <returns></returns>
   wstring  GuiString::Format(const WCHAR*  format, ...)
   {
      va_list  args;
      return FormatV(format, va_start(args, format));
   }

   /// <summary>Assembles a formatted string</summary>
   /// <param name="format">Formatting string</param>
   /// <param name="args">Formatting arguments</param>
   /// <returns></returns>
   /// <exception cref="Logic::ArgumentException">Insufficient buffer space</exception>
   wstring  GuiString::FormatV(const WCHAR*  format, va_list  args)
   {
      WCHAR  szBuffer[512];

      // Format string
      if (StringCchVPrintf(szBuffer, 512, format, args) == STRSAFE_E_INSUFFICIENT_BUFFER)
         throw ArgumentException(HERE, L"format", L"Formatting string exceeds 512 characters");

      return szBuffer;
   }

   /// <summary>Assembles a formatted string stored as a resource</summary>
   /// <param name="id">The resource identifier</param>
   /// <param name="">Formatting arguments</param>
   /// <returns></returns>
   wstring  GuiString::Load(UINT  id, ...)
   {
      va_list  pArgs;
      return LoadV(id, va_start(pArgs, id));
   }

   /// <summary>Assembles a formatted string stored as a resource</summary>
   /// <param name="id">The resource identifier</param>
   /// <param name="pArgs">Variable arguments</param>
   /// <returns></returns>
   /// <exception cref="Logic::ArgumentException">Insufficient buffer space</exception>
   wstring  GuiString::LoadV(UINT  id, va_list  pArgs)
   {
      WCHAR    szBuffer[512],
               szFormat[512];
      
      // Load string 
      if (!LoadString(AfxGetResourceHandle(), id, szFormat, 512))
         return Missing(id);
         
      // Format + Return
      if (StringCchVPrintf(szBuffer, 512, szFormat, pArgs) == STRSAFE_E_INSUFFICIENT_BUFFER)
         throw ArgumentException(HERE, L"format", L"Formatting string exceeds 512 characters");
      return szBuffer;
   }

   /// <summary>Formats a system error message</summary>
   /// <param name="id">The error code</param>
   /// <returns></returns>
   wstring  GuiString::FromSystem(UINT  id)
   {
      WCHAR* szBuffer;
      GuiString msg;

      if (FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS|FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, id, NULL, (WCHAR*)&szBuffer, 512, NULL))
      {
         msg = szBuffer;
         LocalFree(szBuffer);
      }

      return msg.TrimRight(L"\r\n\t ");
   }

   /// <summary>Get Missing string placeholder</summary>
   /// <param name="id">The string identifier.</param>
   /// <returns></returns>
   wstring GuiString::Missing(UINT id)
   {
      return VString(L"{Missing string: %d}", id);
   }

   // ------------------------------- PUBLIC METHODS -------------------------------
   
   /// <summary>Test for presence of a substring</summary>
   /// <param name="str">The substring.</param>
   /// <param name="matchCase">Whether to perform case sensitive search</param>
   /// <returns></returns>
   bool  GuiString::Contains(const wstring& str, bool matchCase) const
   {
      return matchCase ? find(str) != npos : StrStrI(c_str(), str.c_str()) != nullptr;
   }
      
   /// <summary>Perform case insensitive comparison</summary>
   /// <param name="r">Other string</param>
   /// <returns></returns>
   bool  GuiString::Equals(const wstring& r) const
   {
      return StrCmpI(c_str(), r.c_str()) == 0;
   }

   /// <summary>Find index of a substring</summary>
   /// <param name="str">The substring.</param>
   /// <param name="matchCase">Whether to perform case sensitive search</param>
   /// <returns>Offset or npos</returns>
   int  GuiString::Find(const wstring& str, bool matchCase) const
   {
      // Sensitive: Use stl
      if (matchCase)
         return find(str);

      // Insensitive: Convert string ptr into char index
      auto txt = c_str();
      auto pos = StrStrI(txt, str.c_str());
      return pos ? pos - txt : npos;
   }

   /// <summary>Find index of a substring</summary>
   /// <param name="str">The substring.</param>
   /// <param name="offset">Start offset.</param>
   /// <param name="matchCase">Whether to perform case sensitive search</param>
   /// <returns>Offset or npos</returns>
   /// <exception cref="Logic::IndexOutOfRangeException">Invalid offset</exception>
   int  GuiString::Find(const wstring& str, UINT offset, bool matchCase) const
   {
      // Validate offset
      if (offset >= length())
         throw IndexOutOfRangeException(HERE, offset, length());

      // Sensitive: Use stl
      if (matchCase)
         return find(str, offset);

      // Insensitive: Convert string ptr into char index
      auto txt = c_str();
      auto pos = StrStrI(&txt[offset], str.c_str());
      return pos ? pos - txt : npos;
   }

   
   /// <summary>Get whether string is numeric.</summary>
   /// <returns>False if empty or non-numeric, otherwise true</returns>
   bool  GuiString::IsNumeric() const
   {
      for (wchar ch : *this)
         if (!iswdigit(ch))
            return false;

      return true;
   }

   /// <summary>Extracts n characters from start of the string</summary>
   /// <param name="chars">Number of characters</param>
   /// <returns>New string containing desired characters, or the entire string if chars is greater than length</returns>
   GuiString  GuiString::Left(UINT chars) const
   {
      return chars < length() ? GuiString(begin(),begin()+chars) : *this;
   }
      
   /// <summary>Extracts n characters from end of the string</summary>
   /// <param name="chars">Number of characters</param>
   /// <returns>New string containing desired characters, or the entire string if chars is greater than length</returns>
   GuiString  GuiString::Right(UINT chars) const
   {
      return chars < length() ? GuiString(end()-chars,end()) : *this;
   }

   /// <summary>Removes all instances of given characters</summary>
   /// <param name="chars">The chars.</param>
   /// <returns>Copy of string with characters removed</returns>
   GuiString  GuiString::Remove(const WCHAR* chars) const
   {
      GuiString s(*this);

      // Iterate thru characters
      for (const wchar* ch = chars; ch[0]; ++ch)
         // Strip all instances of character
         for (auto pos = s.find_first_of(ch[0]); pos != GuiString::npos; pos = s.find_first_of(ch[0]))
            s.erase(pos, 1);

      return s;
   }

   /// <summary>Replaces all instances of a substring with another.</summary>
   /// <param name="txt">Substring.</param>
   /// <param name="with">replacement.</param>
   /// <returns>Copy of string with replacements made</returns>
   GuiString  GuiString::ReplaceAll(const wstring& txt, const wstring& with) const
   {
      GuiString s(*this);

      // Store invariants
      auto search = txt.c_str();
      auto replace_len = with.length();

      // Replace all
      size_t pos = 0;
      while ((pos=s.find(search, pos)) != npos)
      {
         s.replace(pos, pos+replace_len, with);
         pos += replace_len;
      }

      return s;
   }

   /// <summary>Convert to int.</summary>
   /// <returns></returns>
   int  GuiString::ToInt() const
   {
      return _ttoi(c_str());
   }
      
   /// <summary>Convert string to lower case</summary>
   /// <returns>New string in lower case</returns>
   GuiString  GuiString::ToLower() const
   {
      GuiString s(*this);
      for (auto& ch : s)
         ch = towlower(ch);
      return s;
   }
      
   /// <summary>Convert string to upper case</summary>
   /// <returns>New string in upper case</returns>
   GuiString  GuiString::ToUpper() const
   {
      GuiString s(*this);
      for (auto& ch : s)
         ch = towupper(ch);
      return s;
   }

   /// <summary>Trims leading and trailing characters</summary>
   /// <param name="chars">The chars.</param>
   /// <returns>Copy of string with characters removed</returns>
   GuiString  GuiString::Trim(const WCHAR* chars) const
   {
      return TrimLeft(chars).TrimRight(chars);
   }

   /// <summary>Trims leading characters</summary>
   /// <param name="chars">The chars.</param>
   /// <returns>Copy of string with characters removed</returns>
   GuiString  GuiString::TrimLeft(const WCHAR* chars) const
   {
      GuiString s(*this);

      // Find start of chars to trim
      auto pos = s.find_first_not_of(chars);
         
      // None: String is entirely made of target chars
      if (pos == npos)
         s.clear();

      // Found: Strip chars
      else if (pos != 0)
         s.erase(0, pos);

      return s;
   }

   /// <summary>Trims trailing characters</summary>
   /// <param name="chars">The chars.</param>
   /// <returns>Copy of string with characters removed</returns>
   GuiString  GuiString::TrimRight(const WCHAR* chars) const
   {
      GuiString s(*this);

      // Find start of chars to trim
      auto pos = s.find_last_not_of(chars);

      // Found: Strip chars 
      if (pos != npos)
         s.erase(s.begin()+pos+1, s.end());
         
      else // Not Found: String is entirely made of target chars
         s.clear();

      return s;
   }

   // ------------------------------ PROTECTED METHODS -----------------------------

   // ------------------------------- PRIVATE METHODS ------------------------------

   

}