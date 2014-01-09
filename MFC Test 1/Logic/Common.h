#pragma once

#include "Utils.h"
#include "Exceptions.h"
#include "Resource.h"

using namespace Library;

namespace Library
{
   
   /// <summary>Defines the available game data languages</summary>
   enum class GameLanguage : int { English, French, German, Russian };


   /// <summary>Defines the available game versions</summary>
   enum class GameVersion : int { Threat=1, Reunion=2, TerranConflict=4, AlbionPrelude=8, Rebirth=16 };
}
