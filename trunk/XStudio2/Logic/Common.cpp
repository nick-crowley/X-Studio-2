#include "stdafx.h"
#include "Common.h"


namespace Logic
{
   
      // -------------------------------- CONSTRUCTION --------------------------------

      // ------------------------------- STATIC METHODS -------------------------------

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