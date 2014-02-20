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

   /// <summary>Get game language string</summary>
   wstring  GetString(GameLanguage l);



   /// <summary>Defines the available game versions</summary>
   enum class GameVersion : UINT { Threat=1, Reunion=2, TerranConflict=4, AlbionPrelude=8, Rebirth=16 };

   /// <summary>Map Game version into zero-based index</summary>
   class GameVersionIndex
   {
   public:
      GameVersionIndex(GameVersion v) : Index(0), Version(v)
      {
         switch (v)
         {
         case GameVersion::Threat:         Index = 0;  break;
         case GameVersion::Reunion:        Index = 1;  break;
         case GameVersion::TerranConflict: Index = 2;  break;
         case GameVersion::AlbionPrelude:  Index = 3;  break;
         case GameVersion::Rebirth:        Index = 4;  break;
         }
      }

      GameVersionIndex(UINT index) : Index(index), Version(GameVersion::Threat)
      {
         switch (index)
         {
         case 0:  Version = GameVersion::Threat;          break;
         case 1:  Version = GameVersion::Reunion;         break;
         case 2:  Version = GameVersion::TerranConflict;  break;
         case 3:  Version = GameVersion::AlbionPrelude;   break;
         case 4:  Version = GameVersion::Rebirth;         break;
         }
      }

      int          Index;
      GameVersion  Version;
   };


   
   /// <summary>Paragraph alignment</summary>
   enum class Alignment { Left = PFA_LEFT, Centre = PFA_CENTER, Right = PFA_RIGHT, Justify = PFA_FULL_INTERWORD };

   /// <summary>Get paragraph alignment string</summary>
   wstring  GetString(Alignment a);



   /// <summary>Compare character ranges</summary>
   bool operator==(const CHARRANGE& a, const CHARRANGE& b);
   bool operator!=(const CHARRANGE& a, const CHARRANGE& b);
}

using namespace Logic;


/// <summary>Enable/disable tweaks to produce byte-code that exactly matches for validation</summary>
#define VALIDATION

#include "StringResource.h"
#include "Path.h"
#include "Exceptions.h"
#include "ConsoleWnd.h"

