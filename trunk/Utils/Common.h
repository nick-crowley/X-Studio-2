#pragma once
#include "Macros.h"

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

#include <Richedit.h>

namespace Logic
{
   
   /// <summary>Paragraph alignment</summary>
   enum class Alignment { Left = PFA_LEFT, Centre = PFA_CENTER, Right = PFA_RIGHT, Justify = PFA_FULL_INTERWORD };

   /// <summary>Get paragraph alignment string</summary>
   wstring  GetString(Alignment a);

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

   /// <summary>Convert colour enumeration to RGB</summary>
   RichTextColour ForRichText(Colour c);

   /// <summary>Convert colour enumeration to RGB</summary>
   TooltipColour ForTooltip(Colour c);

   /// <summary>Compare character ranges</summary>
   bool operator==(const CHARRANGE& a, const CHARRANGE& b);
   bool operator!=(const CHARRANGE& a, const CHARRANGE& b);


   template<typename T> 
   class reverse_adapter 
   { 
   public: 
      reverse_adapter(T& c) : c(c) 
      {} 
      
      typename T::reverse_iterator begin() { return c.rbegin(); } 
      typename T::reverse_iterator end()   { return c.rend();   } 

      typename T::const_reverse_iterator begin() const { return c.rbegin(); } 
      typename T::const_reverse_iterator end() const   { return c.rend();   } 
   
   private: 
      T& c; 
   };


   /// <summary>Defines the available game data languages</summary>
   enum class GameLanguage : UINT { Russian = 7, French = 33, Spanish = 34, Italian = 39, Czech = 42, English = 44, Polish = 48, German = 49 };

   /// <summary>Get game language string</summary>
   wstring  GetString(GameLanguage l);

   /// <summary>Map Game Language into zero-based index</summary>
   class GameLanguageIndex
   {
   public:
      /// <summary>Convert Language to zero-based index</summary>
      /// <param name="lang">language.</param>
      GameLanguageIndex(GameLanguage lang) : Index(0), Language(lang)
      {
         switch (lang)
         {
         case GameLanguage::Czech:     Index = 0;  break;
         case GameLanguage::English:   Index = 1;  break;
         case GameLanguage::French:    Index = 2;  break;
         case GameLanguage::German:    Index = 3;  break;
         case GameLanguage::Italian:   Index = 4;  break;
         case GameLanguage::Polish:    Index = 5;  break;
         case GameLanguage::Russian:   Index = 6;  break;
         case GameLanguage::Spanish:   Index = 7;  break;
         }
      }

      /// <summary>Convert zero-based index to Language.</summary>
      /// <param name="index">The index.</param>
      GameLanguageIndex(UINT index) : Index(index), Language(GameLanguage::English)
      {
         switch (index)
         {
         case 0:  Language = GameLanguage::Czech;     break;
         case 1:  Language = GameLanguage::English;   break;
         case 2:  Language = GameLanguage::French;    break;
         case 3:  Language = GameLanguage::German;    break;
         case 4:  Language = GameLanguage::Italian;   break;
         case 5:  Language = GameLanguage::Polish;    break;
         case 6:  Language = GameLanguage::Russian;   break;
         case 7:  Language = GameLanguage::Spanish;   break;
         }
      }

      int           Index;
      GameLanguage  Language;
   };



   /// <summary>Defines the available game versions</summary>
   enum class GameVersion : UINT { Threat=1, Reunion=2, TerranConflict=4, AlbionPrelude=8, Rebirth=16 };

   /// <summary>Map Game version into zero-based index</summary>
   class GameVersionIndex
   {
   public:
      /// <summary>Convert version to zero-based index</summary>
      /// <param name="v">The v.</param>
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

      /// <summary>Convert zero-based index to version.</summary>
      /// <param name="index">The index.</param>
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

}

using namespace Logic;



#include "Path.h"
#include "Exceptions.h"
#include "GuiString.h"
#include "ConsoleWnd.h"


