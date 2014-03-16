#include "stdafx.h"
#include "Utils.h"

namespace Logic
{
   
   // -------------------------------- CONSTRUCTION --------------------------------

   // ------------------------------- STATIC METHODS -------------------------------

   /// <summary>Convert colour enumeration to RGB tooltip colour</summary>
   /// <param name="c">colour</param>
   /// <returns></returns>
   /// <exception cref="Logic::ArgumentException">Invalid colour</exception>
   UtilExport TooltipColour ForTooltip(Colour c)
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

      throw ArgumentException(HERE, L"c", VString(L"Unrecognised colour enumeration: %d", c));
   }

   /// <summary>Convert colour enumeration to RGB</summary>
   /// <param name="c">colour</param>
   /// <returns></returns>
   /// <exception cref="Logic::ArgumentException">Invalid colour</exception>
   UtilExport RichTextColour ForRichText(Colour c)
   {
      switch (c)
      {
      case Colour::Black:   return RichTextColour::Black;
      case Colour::White:   return RichTextColour::White;
      case Colour::Default: return RichTextColour::Default;

      case Colour::Grey:    return RichTextColour::Grey;
      case Colour::Blue:    return RichTextColour::Blue;
      case Colour::Cyan:    return RichTextColour::Cyan;
      case Colour::Green:   return RichTextColour::Green;
      case Colour::Orange:  return RichTextColour::Orange;
      case Colour::Purple:  return RichTextColour::Purple;
      case Colour::Red:     return RichTextColour::Red;
      case Colour::Silver:  return RichTextColour::Silver;
      case Colour::Yellow:  return RichTextColour::Yellow;
      }

      throw ArgumentException(HERE, L"c", VString(L"Unrecognised colour enumeration: %d", c));
   }


   /// <summary>Convert paragraph alignment string</summary>
   /// <param name="a">alignment</param>
   /// <returns></returns>
   UtilExport wstring  GetString(Alignment a)
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
   UtilExport wstring  GetString(GameLanguage l)
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
   UtilExport bool operator==(const CHARRANGE& a, const CHARRANGE& b)
   {
      return a.cpMax == b.cpMax && a.cpMin == b.cpMin;
   }

   /// <summary>Compare character ranges</summary>
   UtilExport bool operator!=(const CHARRANGE& a, const CHARRANGE& b)
   {
      return a.cpMax != b.cpMax || a.cpMin != b.cpMin;
   }

   /// <summary>Dialog data exchange for std::wstring</summary>
   UtilExport void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, wstring& value)
   {
	   HWND hWndCtrl = pDX->PrepareEditCtrl(nIDC);
      CString str;

	   if (pDX->m_bSaveAndValidate)
	   {
		   int nLen = ::GetWindowTextLength(hWndCtrl);
		   ::GetWindowText(hWndCtrl, str.GetBufferSetLength(nLen), nLen+1);
		   str.ReleaseBuffer();
         value = (LPCWSTR)str;
	   }
	   else
	   {
		   AfxSetWindowText(hWndCtrl, value.c_str());
	   }
   }

   /// <summary>Dialog data exchange for paths</summary>
   UtilExport void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, Path& path)
   {
	   HWND hWndCtrl = pDX->PrepareEditCtrl(nIDC);

	   if (pDX->m_bSaveAndValidate)
	      ::GetWindowText(hWndCtrl, (wchar*)path, MAX_PATH);
	   else
	      AfxSetWindowText(hWndCtrl, path.c_str());
   }

   // ------------------------------- PUBLIC METHODS -------------------------------


   // ------------------------------ PROTECTED METHODS -----------------------------

   // ------------------------------- PRIVATE METHODS ------------------------------
      

}