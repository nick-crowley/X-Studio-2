#include "stdafx.h"
#include "Common.h"


namespace Logic
{
   
      // -------------------------------- CONSTRUCTION --------------------------------

      // ------------------------------- STATIC METHODS -------------------------------

      /// <summary>Convert colour enumeration to RGB tooltip colour</summary>
      /// <param name="c">colour</param>
      /// <returns></returns>
      /// <exception cref="Logic::ArgumentException">Invalid colour</exception>
      TooltipColour ForTooltip(Colour c)
      {
         switch (c)
         {
         case Colour::Black:   return TooltipColour::Black;
         case Colour::Default: return TooltipColour::Default;
         case Colour::White:   return TooltipColour::White;
         case Colour::Silver:  return TooltipColour::Silver;

         case Colour::Grey:    return TooltipColour::Grey;
         case Colour::Blue:    return TooltipColour::Blue;
         case Colour::Cyan:    return TooltipColour::Cyan;
         case Colour::Green:   return TooltipColour::Green;
         case Colour::Orange:  return TooltipColour::Orange;
         case Colour::Purple:  return TooltipColour::Purple;
         case Colour::Red:     return TooltipColour::Red;
         case Colour::Yellow:  return TooltipColour::Yellow;
         }

         throw ArgumentException(HERE, L"c", GuiString(L"Unrecognised colour enumeration: %d", c));
      }

      /// <summary>Convert colour enumeration to RGB</summary>
      /// <param name="c">colour</param>
      /// <returns></returns>
      /// <exception cref="Logic::ArgumentException">Invalid colour</exception>
      LanguageColour ForLanguage(Colour c)
      {
         switch (c)
         {
         case Colour::Black:   return LanguageColour::Black;
         case Colour::White:   return LanguageColour::White;
         case Colour::Default: return LanguageColour::Default;

         case Colour::Grey:    return LanguageColour::Grey;
         case Colour::Blue:    return LanguageColour::Blue;
         case Colour::Cyan:    return LanguageColour::Cyan;
         case Colour::Green:   return LanguageColour::Green;
         case Colour::Orange:  return LanguageColour::Orange;
         case Colour::Purple:  return LanguageColour::Purple;
         case Colour::Red:     return LanguageColour::Red;
         case Colour::Silver:  return LanguageColour::Silver;
         case Colour::Yellow:  return LanguageColour::Yellow;
         }

         throw ArgumentException(HERE, L"c", GuiString(L"Unrecognised colour enumeration: %d", c));
      }


      /// <summary>Convert paragraph alignment string</summary>
      /// <param name="a">alignment</param>
      /// <returns></returns>
      wstring  GetString(Alignment a)
      {
         switch (a)
         {
         case Alignment::Left:    return L"Left";
         case Alignment::Centre:  return L"Centre";
         case Alignment::Right:   return L"Right";
         case Alignment::Justify: return L"Justify";
         }
         return L"Invalid";
      }
      
      /// <summary>Gets game language string</summary>
      /// <param name="l">language</param>
      /// <returns></returns>
      wstring  GetString(GameLanguage l)
      {
         switch (l)
         {
         case GameLanguage::Russian:  return L"Russian";
         case GameLanguage::French:   return L"French";
         case GameLanguage::Spanish:  return L"Spanish";
         case GameLanguage::Italian:  return L"Italian";
         case GameLanguage::Czech:    return L"Czech";
         case GameLanguage::English:  return L"English";
         case GameLanguage::Polish:   return L"Polish"; 
         case GameLanguage::German:   return L"German";
         }
         return L"Invalid";
      }

      /// <summary>Compare character ranges</summary>
      bool operator==(const CHARRANGE& a, const CHARRANGE& b)
      {
         return a.cpMax == b.cpMax && a.cpMin == b.cpMin;
      }

      /// <summary>Compare character ranges</summary>
      bool operator!=(const CHARRANGE& a, const CHARRANGE& b)
      {
         return a.cpMax != b.cpMax || a.cpMin != b.cpMin;
      }


      // ------------------------------- PUBLIC METHODS -------------------------------


      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
      

}