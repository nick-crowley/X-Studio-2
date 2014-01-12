#pragma once

#include "Exceptions.h"
#include "../Resource.h"

using namespace Logic;

namespace Logic
{
   
   /// <summary>Defines the available game data languages</summary>
   enum class GameLanguage : UINT { Russian = 7, French = 33, Spanish = 34, Italian = 39, Czech = 42, English = 44, Polish = 48, German = 49 };


   /// <summary>Defines the available game versions</summary>
   enum class GameVersion : UINT { Threat=1, Reunion=2, TerranConflict=4, AlbionPrelude=8, Rebirth=16 };

   /// <summary>String holding name of a game version</summary>
   class VersionString : public wstring
   {
   public:
      VersionString(GameVersion v)
      {
         switch (v)
         {
         case GameVersion::AlbionPrelude:  assign(L"Albion Prelude");  break;
         case GameVersion::TerranConflict: assign(L"Terran Conflict"); break;
         case GameVersion::Reunion:        assign(L"Reunion");         break;
         case GameVersion::Threat:         assign(L"The Threat");      break;
         case GameVersion::Rebirth:        assign(L"Rebirth");         break;
         default: throw ArgumentException(HERE, L"v", L"Unknown game version");
         }
      }
   };

   // ------------------------ USER TYPES --------------------------

   /// <summary>Occurs when a language string is missing</summary>
   class PageNotFoundException : public ExceptionBase
   {
   public:
      /// <summary>Create an PageNotFoundException</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="page">page id</param>
      PageNotFoundException(wstring  src, UINT page) 
         : ExceptionBase(src, GuiString(L"Cannot find page %d", page))
      {}
   };

   /// <summary>Occurs when a language string is missing</summary>
   class StringNotFoundException : public ExceptionBase
   {
   public:
      /// <summary>Create an StringNotFoundException</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="page">page id</param>
      /// <param name="id">string id</param>
      StringNotFoundException(wstring  src, UINT page, UINT id) 
         : ExceptionBase(src, GuiString(L"Cannot find string %d in page %d", id, page))
      {}
   };

   /// <summary>Occurs when a command syntax is missing</summary>
   class SyntaxNotFoundException : public ExceptionBase
   {
   public:
      /// <summary>Create an SyntaxNotFoundException</summary>
      /// <param name="src">Location of throw</param>
      /// <param name="id">command id</param>
      /// <param name="ver">game version</param>
      SyntaxNotFoundException(wstring  src, UINT id, GameVersion v) 
         : ExceptionBase(src, GuiString(L"Cannot find %s syntax for command with id %d", VersionString(v).c_str(), id))
      {}
   };

}

