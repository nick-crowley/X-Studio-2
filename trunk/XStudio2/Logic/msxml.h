#pragma once

#import "MSXML6.dll" rename_namespace(_T("Xml"))
//#include <msxml2.h>

/// <summary>Xml DOM document smart pointer</summary>
typedef Xml::IXMLDOMDocumentPtr  XmlDocumentPtr;

/// <summary>Xml DOM node smart pointer</summary>
typedef Xml::IXMLDOMNodePtr  XmlNodePtr;

/// <summary>Xml DOM element smart pointer</summary>
typedef Xml::IXMLDOMElementPtr  XmlElementPtr;

/// <summary>Xml DOM element smart pointer</summary>
typedef Xml::IXMLDOMAttributePtr  XmlAttributePtr;



