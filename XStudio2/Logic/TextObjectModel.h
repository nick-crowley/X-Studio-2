#pragma once

// Import TOM library
#undef FindText
#import "Msftedit.dll" rename_namespace(_T("TOM"))


/// <summary>TOM COM interface</summary>
typedef TOM::ITextDocumentPtr  TextDocumentPtr;
typedef TOM::ITextRangePtr     TextRangePtr;
typedef TOM::ITextFontPtr      TextFontPtr;
typedef TOM::ITextParaPtr      TextParaPtr;

