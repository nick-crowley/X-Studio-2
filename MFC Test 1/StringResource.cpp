#include "stdafx.h"
#include "Common.h"
#include <Strsafe.h>       // C String handling
#include "MFC Test 1.h"    // the app

namespace Library
{

   /// <summary>Assembles a formatted string</summary>
   /// <param name="format">Formatting string</param>
   /// <param name="">Formatting arguments</param>
   /// <returns></returns>
   wstring  StringResource::Format(const WCHAR*  format, ...)
   {
      TCHAR    szBuffer[512];
      va_list  pArgs;

      // Format + Return
      StringCchVPrintf(szBuffer, 512, format, va_start(pArgs, format));
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
      TCHAR    szBuffer[512],
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
   wstring  StringResource::FormatMessage(UINT  id)
   {
      WCHAR* szBuffer;
      wstring msg;

      if (FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS|FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, id, NULL, (WCHAR*)&szBuffer, 512, NULL))
      {
         msg = szBuffer;
         LocalFree(szBuffer);
      }

      return msg;
   }

}