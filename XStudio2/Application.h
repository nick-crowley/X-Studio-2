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
   HICON     LoadIcon(UINT nResID, UINT iSize) const;
   CBitmap*  LoadBitmap(UINT nResID, int x, int y, UINT flags) const;
   BOOL      ShowError(const GuiString& src, const ExceptionBase& e, const GuiString& msg) const;
   
   // ----------------------- MUTATORS ------------------------
public:
   virtual int  ExitInstance();
	virtual BOOL InitInstance();
   virtual void LoadCustomState();
   BOOL         OpenStringLibrary();
	virtual void PreLoadState();
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
