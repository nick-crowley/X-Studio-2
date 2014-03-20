#pragma once
#include <Richedit.h>

/// <summary>A unique pointer to a byte array</summary>
typedef unique_ptr<BYTE, std::default_delete<BYTE[]>>  ByteArrayPtr;

/// <summary>A unique pointer to a wide char array</summary>
typedef unique_ptr<WCHAR, std::default_delete<WCHAR[]>>  CharArrayPtr;

/// <summary>Wide char</summary>
typedef wchar_t wchar;

/// <summary>Vector of strings</summary>
typedef vector<wstring>  LineArray;

/// <summary>Constant line iterator</summary>
typedef LineArray::const_iterator  LineIterator;



namespace Logic
{
   
   /// <summary>Paragraph alignment</summary>
   enum class Alignment { Left = PFA_LEFT, Centre = PFA_CENTER, Right = PFA_RIGHT, Justify = PFA_FULL_INTERWORD };

   /// <summary>Device independent colour enumeration</summary>
   enum class Colour  
   { 
      Default,
      Black, White,  Blue,   Cyan,
      Green, Grey,   Orange, Purple,
      Red,   Silver, Yellow
   };

   /// <summary>RGB values for RichText colours</summary>
   enum class RichTextColour : COLORREF 
   { 
      Default = RGB(200,200,200),
      Black = RGB(0,0,0),     White  = RGB(255,255,255), Blue   = RGB(0,0,255),     Cyan   = RGB(42,168,200), 
      Green = RGB(13,191,25), Grey   = RGB(128,128,128), Orange = RGB(255,150,20),  Purple = RGB(210,20,255), 
      Red   = RGB(204,0,0),   Silver = RGB(233,233,233), Yellow = RGB(215,215,0) 
   };

   /// <summary>RGB values for Tooltip colours</summary>
   enum class TooltipColour : COLORREF
   {
      Default = RGB(200,200,200),
      Black = RGB(0,0,0),     White  = RGB(255,255,255), Blue   = RGB(25,55,242),  Cyan   = RGB(42,168,200), 
      Green = RGB(13,191,25), Grey   = RGB(128,128,128), Orange = RGB(213,145,28), Purple = RGB(200,36,167), 
      Red   = RGB(180,37,37), Silver = RGB(233,233,233), Yellow = RGB(180,180,0)
   };


   /// <summary>Defines the available game data languages</summary>
   enum class GameLanguage : UINT { Russian = 7, French = 33, Spanish = 34, Italian = 39, Czech = 42, English = 44, Polish = 48, German = 49 };

   /// <summary>Defines the available game versions</summary>
   enum class GameVersion : UINT { Threat=1, Reunion=2, TerranConflict=4, AlbionPrelude=8, Rebirth=16 };


   /// <summary>Supported file types</summary>
   enum class FileType : UINT { Unknown, Script, Language, Mission,  Project, Universe };
   
}

using namespace Logic;
