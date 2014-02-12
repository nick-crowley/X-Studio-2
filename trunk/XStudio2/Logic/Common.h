#pragma once

#include "../Resources.h"
#include "Macros.h"

/// <summary>A unique pointer to a byte array</summary>
typedef unique_ptr<BYTE, default_delete<BYTE[]>>  ByteArrayPtr;

/// <summary>A unique pointer to a wide char array</summary>
typedef unique_ptr<WCHAR, default_delete<WCHAR[]>>  CharArrayPtr;

/// <summary>Wide char</summary>
typedef wchar_t wchar;

namespace Logic
{
   /// <summary>Defines the available game data languages</summary>
   enum class GameLanguage : UINT { Russian = 7, French = 33, Spanish = 34, Italian = 39, Czech = 42, English = 44, Polish = 48, German = 49 };

   /// <summary>Defines the available game versions</summary>
   enum class GameVersion : UINT { Threat=1, Reunion=2, TerranConflict=4, AlbionPrelude=8, Rebirth=16 };

   
   /// <summary>Paragraph alignment</summary>
   enum class Alignment { Left = PFA_LEFT, Centre = PFA_CENTER, Right = PFA_RIGHT, Justify = PFA_FULL_INTERWORD };

}

using namespace Logic;


/// <summary>Enable/disable tweaks to produce byte-code that exactly matches for validation</summary>
#define VALIDATION

#include "StringResource.h"
#include "Path.h"
#include "Exceptions.h"
#include "ConsoleWnd.h"

