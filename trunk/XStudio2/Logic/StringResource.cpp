#include "stdafx.h"
#include "Common.h"       // Do not include StringResource.h directly, referenced via common.h
#include <Strsafe.h>
#include "../Application.h"    // the app

namespace Logic
{
   // -------------------------------- CONSTRUCTION --------------------------------

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
   wstring  StringResource::Convert(const string& str, UINT codepage)
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
   string  StringResource::Convert(const wstring& str, UINT codepage)
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
   wstring  StringResource::Format(const WCHAR*  format, ...)
   {
      va_list  args;
      return FormatV(format, va_start(args, format));
   }

   /// <summary>Assembles a formatted string</summary>
   /// <param name="format">Formatting string</param>
   /// <param name="args">Formatting arguments</param>
   /// <returns></returns>
   /// <exception cref="Logic::ArgumentException">Insufficient buffer space</exception>
   wstring  StringResource::FormatV(const WCHAR*  format, va_list  args)
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
   wstring  StringResource::Load(UINT  id, ...)
   {
      va_list  pArgs;
      return LoadV(id, va_start(pArgs, id));
   }

   /// <summary>Assembles a formatted string stored as a resource</summary>
   /// <param name="id">The resource identifier</param>
   /// <param name="pArgs">Variable arguments</param>
   /// <returns></returns>
   /// <exception cref="Logic::ArgumentException">Insufficient buffer space</exception>
   wstring  StringResource::LoadV(UINT  id, va_list  pArgs)
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
   wstring  StringResource::FromSystem(UINT  id)
   {
      WCHAR* szBuffer;
      wstring msg;

      if (FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS|FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, id, NULL, (WCHAR*)&szBuffer, 512, NULL))
      {
         msg = szBuffer;
         LocalFree(szBuffer);
      }

      return msg;
   }

   /// <summary>Get Missing string placeholder</summary>
   /// <param name="id">The string identifier.</param>
   /// <returns></returns>
   wstring StringResource::Missing(UINT id)
   {
      return VString(L"{Missing string: %d}", id);
   }

   // ------------------------------- PUBLIC METHODS -------------------------------
   
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

   // ------------------------------ PROTECTED METHODS -----------------------------

   // ------------------------------- PRIVATE METHODS ------------------------------

   

}