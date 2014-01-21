
// MFC Test 1.h : main header file for the MFC Test 1 application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif


// Application:
class Application : public CWinAppEx
{
public:
	Application();


   HICON  LoadIcon(UINT nResID, UINT iSize)
   {
      return (HICON)::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(nResID), IMAGE_ICON, iSize, iSize, 0);
   }

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	UINT  m_nAppLook;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern Application theApp;
