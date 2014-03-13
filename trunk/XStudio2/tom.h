#pragma once

// Ignore warning(s) : 'automatically excluding 'wireHWND' while importing type library 'Msftedit.dll'' 
#pragma warning(disable:4192)

// Import TOM library
#undef FindText

#include "msftedit.tlh"    //#import "Msftedit.dll" rename_namespace(_T("TOM"))


/// <summary>TOM COM interface</summary>
typedef TOM::ITextDocumentPtr  TextDocumentPtr;
typedef TOM::ITextRangePtr     TextRangePtr;
typedef TOM::ITextFontPtr      TextFontPtr;
typedef TOM::ITextParaPtr      TextParaPtr;


#pragma warning(default:4192)

