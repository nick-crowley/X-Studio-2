
// MFC Test 1.h : main header file for the MFC Test 1 application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif


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

   // --------------------- PROPERTIES ------------------------
	  
   // ---------------------- ACCESSORS ------------------------			
   
public:
   HICON  LoadIcon(UINT nResID, UINT iSize) const
   {
      return (HICON)::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(nResID), IMAGE_ICON, iSize, iSize, 0);
   }

   // ----------------------- MUTATORS ------------------------
public:
	virtual BOOL InitInstance();
	virtual int  ExitInstance();

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();

   // -------------------- REPRESENTATION ---------------------
   
};

extern Application theApp;
