#pragma once
#include "stl.h"
#include "Shlwapi.h"

namespace Library
{
   class StringResource
   {
   public:
      static wstring  Load(UINT  id, ...);
      static wstring  LoadV(UINT  id, va_list  pArgs);
   };

   class Path : public wstring
   {
   public:
      Path(const WCHAR* path) : wstring(path) {}
      Path(wstring path) : wstring(path) {}

      bool  Exists() { return PathFileExists(c_str()) != FALSE; }

   };
}