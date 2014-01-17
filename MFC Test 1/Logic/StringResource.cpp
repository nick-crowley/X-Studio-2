#include "stdafx.h"
#include "Common.h"       // Do not include StringResource.h directly, referenced via common.h
#include <Strsafe.h>
#include "../MFC Test 1.h"    // the app

namespace Logic
{
   // -------------------------------- CONSTRUCTION --------------------------------

   VersionString::VersionString(GameVersion v)
   {
      switch (v)
      {
      case GameVersion::AlbionPrelude:  assign(L"Albion Prelude");  break;
      case GameVersion::TerranConflict: assign(L"Terran Conflict"); break;
      case GameVersion::Reunion:        assign(L"Reunion");         break;
      case GameVersion::Threat:         assign(L"The Threat");      break;
      case GameVersion::Rebirth:        assign(L"Rebirth");         break;
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
      unique_ptr<WCHAR> wide(new WCHAR[str.length()]);
      // Convert and null terminate buffer
      wide.get()[ MultiByteToWideChar(codepage, NULL, str.c_str(), str.length(), wide.get(), str.length()) ] = NULL;
      return wide.get();
   }

   /// <summary>Converts a wide char string to narrow char</summary>
   /// <param name="str">The string</param>
   /// <param name="codepage">The codepage used for conversion</param>
   /// <returns>Narrow char equivilent</returns>
   string  StringResource::Convert(const wstring& str, UINT codepage)
   {
      unique_ptr<CHAR> ansi(new CHAR[4*str.length()]);
      // Convert and null terminate buffer
      ansi.get()[ WideCharToMultiByte(codepage, NULL, str.c_str(), str.length(), ansi.get(), 4*str.length(), NULL, NULL) ] = NULL;
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
   wstring  StringResource::FormatV(const WCHAR*  format, va_list  args)
   {
      WCHAR  szBuffer[512];

      // Format + Return
      StringCchVPrintf(szBuffer, 512, format, args);
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
   wstring  StringResource::LoadV(UINT  id, va_list  pArgs)
   {
      WCHAR    szBuffer[512],
               szFormat[512];
      
      // Load string + format
      if (LoadString(theApp.m_hInstance, id, szFormat, 512))
         StringCchVPrintf(szBuffer, 512, szFormat, pArgs);
      else
         StringCchPrintf(szBuffer, 512, L"Error: Missing string '%d'", id);

      // Return
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

   // ------------------------------- PUBLIC METHODS -------------------------------

   // ------------------------------ PROTECTED METHODS -----------------------------

   // ------------------------------- PRIVATE METHODS ------------------------------

   

}