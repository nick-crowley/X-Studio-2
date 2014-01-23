
// MFC Test 1.h : main header file for the MFC Test 1 application
//
#pragma once
#include "Logic/Event.h"

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

/// <summary>Application state</summary>
enum class AppState  { NoGameData, GameDataPresent };

/// <summary>Raise when application state changes</summary>
typedef Event<AppState>                    AppStateChangedEvent;
typedef AppStateChangedEvent::DelegatePtr  AppStateChangedHandler;

/// <summary>Main thread</summary>
class Application : public CWinAppEx
{
   // ------------------------ TYPES --------------------------
private:
	  
   // --------------------- CONSTRUCTION ----------------------
public:
	Application();

   // ------------------------ STATIC -------------------------
protected:
   DECLARE_MESSAGE_MAP()

public:
   AppStateChangedEvent     StateChanged;

   // --------------------- PROPERTIES ------------------------
	
   PROPERTY_GET_SET(AppState,State,GetState,SetState);

   // ---------------------- ACCESSORS ------------------------			
   
public:
   AppState  GetState() const
   {
      return GameDataState;
   }

   HICON  LoadIcon(UINT nResID, UINT iSize) const
   {
      return (HICON)::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(nResID), IMAGE_ICON, iSize, iSize, 0);
   }

   CBitmap*  LoadBitmap(UINT nResID, int x, int y, UINT flags)
   {
      CBitmap* bmp = new CBitmap();
      HBITMAP h = (HBITMAP)::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(nResID), IMAGE_BITMAP, x, y, flags);
      bmp->Attach(h);
      return bmp;
   }

   // ----------------------- MUTATORS ------------------------
public:
	virtual BOOL InitInstance();
	virtual int  ExitInstance();

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

   void  SetState(AppState s) 
   {
      GameDataState = s;
      StateChanged.Raise(s);
   }

protected:
	afx_msg void OnAppAbout();

   // -------------------- REPRESENTATION ---------------------
protected:
   AppState    GameDataState;
};

extern Application theApp;
