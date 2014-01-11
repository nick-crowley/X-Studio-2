#pragma once

#include "Utils.h"
#include "Exceptions.h"
#include "../Resource.h"

using namespace Logic;

namespace Logic
{
   
   /// <summary>Defines the available game data languages</summary>
   enum class GameLanguage : UINT { Russian = 7, French = 33, Spanish = 34, Italian = 39, Czech = 42, English = 44, Polish = 48, German = 49 };


   /// <summary>Defines the available game versions</summary>
   enum class GameVersion : UINT { Threat=1, Reunion=2, TerranConflict=4, AlbionPrelude=8, Rebirth=16 };
}
