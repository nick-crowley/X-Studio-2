
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

// Exclude rarely-used stuff from Windows headers
#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            
#endif

// Target 
#include "../targetver.h"

// Sanity check
#ifndef _UTIL_LIB
#error Attempting to include UtilLib precompiled header
#endif

// Tweaks
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS        // some CString constructors will be explicit
#define _AFX_ALL_WARNINGS     // turns off MFC's hiding of some common and often safely ignored warning messages

 
// MFC
#include <afxwin.h>              // MFC core and standard components
#include <afxext.h>              // MFC extensions
#include <afxcmn.h>             // MFC support for Windows Common Controls
#include <afxcontrolbars.h>     // MFC support for ribbons and control bars


// Visual Leak Detector
#include <vld.h>


// STL
#include <string>
#include <vector>
#include <deque>
#include <list>
#include <set>
#include <map>
#include <memory>    // shared/unique ptr
using namespace std;

// COM
#include <comdef.h>

// Utils
#include "Utils.h"

// Import Resource IDs 
#include "../Resources/Resources.h"



// Common controls v6
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")



