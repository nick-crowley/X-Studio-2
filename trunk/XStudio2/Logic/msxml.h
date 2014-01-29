#pragma once

#import "MSXML6.dll" rename_namespace(_T("XML"))
//#include <msxml2.h>

/// <summary>Xml DOM document smart pointer</summary>
typedef XML::IXMLDOMDocumentPtr  XmlDocumentPtr;

/// <summary>Xml DOM node smart pointer</summary>
typedef XML::IXMLDOMNodePtr  XmlNodePtr;

/// <summary>Xml DOM element smart pointer</summary>
typedef XML::IXMLDOMElementPtr  XmlElementPtr;

/// <summary>Xml DOM element smart pointer</summary>
typedef XML::IXMLDOMAttributePtr  XmlAttributePtr;



