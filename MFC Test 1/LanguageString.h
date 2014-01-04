#pragma once
#include "stl.h"

namespace Library
{

class LanguageString
{
public:
   LanguageString() : ID(0) {};
   LanguageString(UINT  id, wstring  txt) : ID(id), Text(txt) {};
   ~LanguageString() {};

   UINT     ID;
   wstring  Text;
};

}