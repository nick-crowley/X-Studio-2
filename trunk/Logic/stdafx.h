
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

// Exclude rarely-used stuff from Windows headers
#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            
#endif

#include "../targetver.h"


// Sanity check
#ifndef _LOGIC_DLL
#error Attempting to include LogicLib precompiled header
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
#include <memory>       // shared/unique ptr
#include <functional>
#include <algorithm>
using namespace std;


/// <summary>BugFix for Release version optimizing away [w]string::npos</summary>
/// <remarks>See https://connect.microsoft.com/VisualStudio/feedback/details/586959/std  (Bug 586959) for details</remarks>
#if _MSC_VER >= 1600
const wstring::size_type wstring::npos = (wstring::size_type) -1;
#endif

/// <summary>BugFix for code that compiled previously as part of main module, but cause compiler errors in the STL
/// when compiled as part of a library. Code added is never executed, just a compiler tweak.</summary>
#define LOGIC_COMPILER_FIX


// COM
#include <comdef.h>

// Utils
#undef _UTIL_LIB
#include "../Utils/Utils.h"
#include "ConsoleWnd.h"

// Application (base)
#include "AppBase.h"

// Import Resource IDs 
#include "../Resources/Resources.h"



// Common controls v6
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")



