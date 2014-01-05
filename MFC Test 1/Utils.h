#pragma once
#include "Shlwapi.h"
#include "Resource.h"

#include <string>
#include <vector>
#include <deque>
#include <set>
#include <memory>
using namespace std;


#ifdef FormatMessage
#undef FormatMessage
#endif

/// Macro: WIDEN(x)  
// Description: Creates a wide char equivilent of an input string (from MSDN)
#define      WIDEN2(x)                L ## x
#define      WIDEN(x)                 WIDEN2(x)

/// Macro: __WFILE__
// Description: Wide character equivilent of __FILE__
#define      __WFILE__                WIDEN(__FILE__)

/// Macro: __WFUNCTION__
// Description: Wide character equivilent of __FUNCTION__
#define      __WFUNCTION__            WIDEN(__FUNCTION__)

namespace Library
{
   class StringResource
   {
   public:
      static wstring  Format(const WCHAR*  format, ...);
      static wstring  Load(UINT  id, ...);
      static wstring  LoadV(UINT  id, va_list  pArgs);

      static wstring  FormatMessage(UINT  id);
   };

   class Path : public wstring
   {
   public:
      Path(const WCHAR* path) : wstring(path) {}
      Path(wstring path) : wstring(path) {}

      bool  Exists() { return PathFileExists(c_str()) != FALSE; }

   };

   typedef unique_ptr<BYTE, default_delete<BYTE[]>>  ByteArray;


}