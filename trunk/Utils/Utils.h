#pragma once

/// <summary>Library export macro</summary>
#ifdef _UTIL_LIB
#define UtilExport  __declspec(dllexport)
#else
#define UtilExport  __declspec(dllimport)
#endif


#include "../Macros.h"
#include "Types.h"
#include "Path.h"
#include "Exceptions.h"
#include "GuiString.h"


/// <summary>Logic</summary>
namespace Logic
{
   /// <summary>Convert colour enumeration to RGB</summary>
   RichTextColour ForRichText(Colour c);

   /// <summary>Convert colour enumeration to RGB</summary>
   TooltipColour ForTooltip(Colour c);

   /// <summary>Get paragraph alignment string</summary>
   wstring  GetString(Alignment a);

   /// <summary>Get game language string</summary>
   wstring  GetString(GameLanguage l);

   /// <summary>Compare character ranges</summary>
   bool operator==(const CHARRANGE& a, const CHARRANGE& b);
   bool operator!=(const CHARRANGE& a, const CHARRANGE& b);

   /// <summary>Dialog data exchange for std::wstring</summary>
   void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, wstring& value);

   /// <summary>Dialog data exchange for paths</summary>
   void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, Path& path);


   /// <summary>Client rectangle</summary>
   class UtilExport ClientRect : public CRect
   {
      // --------------------- CONSTRUCTION ----------------------
   public:
      ClientRect(CWnd* wnd) : CRect(0,0,0,0)
      {
         wnd->GetClientRect(this);
      }

      DEFAULT_COPY(ClientRect);	// Default copy semantics
      DEFAULT_MOVE(ClientRect);	// Default move semantics

      // -------------------- REPRESENTATION ---------------------
   };

   /// <summary>Control rectangle in parent client co-ordinates</summary>
   class UtilExport CtrlRect : public CRect
   {
      // --------------------- CONSTRUCTION ----------------------
   public:
      /// <summary>Create control rectangle in parent client co-ordinates</summary>
      /// <param name="parent">The parent.</param>
      /// <param name="ctrl">The control.</param>
      CtrlRect(CWnd* parent, CWnd* ctrl) : CRect(0, 0, 0, 0)
      {
         ctrl->GetWindowRect(this);
         parent->ScreenToClient(this);
      }

      DEFAULT_COPY(CtrlRect);	// Default copy semantics
      DEFAULT_MOVE(CtrlRect);	// Default move semantics

   };

   /// <summary>Window rectangle</summary>
   class UtilExport WindowRect : public CRect
   {
      // --------------------- CONSTRUCTION ----------------------
   public:
      WindowRect(CWnd* wnd) : CRect(0,0,0,0)
      {
         wnd->GetWindowRect(this);
      }

      DEFAULT_COPY(WindowRect);	// Default copy semantics
      DEFAULT_MOVE(WindowRect);	// Default move semantics

      // -------------------- REPRESENTATION ---------------------
   };


   /// <summary>Position of mouse cursor</summary>
   class UtilExport CursorPoint : public CPoint
   {
   public:
      /// <summary>Create cursor position.</summary>
      /// <param name="wnd">Window for client co=ordinates, or nullptr for screen co-ords</param>
      CursorPoint(const CWnd* wnd = nullptr) : CPoint(0, 0)
      {
         // Get position in screen co-ords
         if (GetCursorPos(this) && wnd)
            // Window: Convert to client co-ords
            ScreenToClient(*wnd, this);
      }
   };

   
   /// <summary>Map Game Language into zero-based index</summary>
   class UtilExport GameLanguageIndex
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

   /// <summary>Map Game version into zero-based index</summary>
   class UtilExport GameVersionIndex
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

   
   /// <summary>Reverse iterator adapater</summary>
   /*template<typename T> 
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
   };*/



   

   
}


