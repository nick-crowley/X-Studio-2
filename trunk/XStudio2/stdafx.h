
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

// Exclude rarely-used stuff from Windows headers
#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            
#endif

#include "targetver.h"

// some CString constructors will be explicit
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

 // MFC core and standard components
#include <afxwin.h>        

// Failed Attempt at redefining CHARFORMAT2 struct with all fields, but not present in richEdit.h :(
#undef _RICHEDIT_VER
#define _RICHEDIT_VER 0x0500

// MFC extensions
#include <afxext.h>         

// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           
#endif
// MFC support for Windows Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             
#endif // _AFX_NO_AFXCMN_SUPPORT

// MFC support for ribbons and control bars
#include <afxcontrolbars.h>     

// MFC socket extensions
#include <afxsock.h>    

// MFC CtrlView
#include <afxcview.h>


// Import MS-XML library
#include "Logic/msxml.h"

// Import tom
#include "Logic/TextObjectModel.h"

// Win32 
#include <Windowsx.h>

// Visual Leak Detector
#include <vld.h>


// STL
#include <string>
#include <vector>
#include <deque>
#include <list>
#include <set>
#include <map>
#include <memory>

using namespace std;

// Common Logic + Utils
#include "Logic/Common.h"
#include "Utils.h"

// Application 
#include "Application.h"

// Preferences
#include "Logic/PreferencesLibrary.h"

// Import Resource IDs from resource project
#include "D:/My Projects/XStudio2/Resources/Resources.h"





// Common controls
#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


