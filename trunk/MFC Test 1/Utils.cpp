#include "stdafx.h"
#include "Utils.h"
#include "MFC Test 1.h"
// C String handling
#include <Strsafe.h>

namespace Library
{
   wstring  StringResource::Format(const WCHAR*  format, ...)
   {
      TCHAR    szBuffer[512];
      va_list  pArgs;

      // Format + Return
      StringCchVPrintf(szBuffer, 512, format, va_start(pArgs, format));
      return szBuffer;
   }

   wstring  StringResource::Load(UINT  id, ...)
   {
      va_list  pArgs;
      return LoadV(id, va_start(pArgs, id));
   }

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