#include "stdafx.h"
#include "XmlReader.h"
#include "StringReader.h"
#include "FileStream.h"

namespace Logic
{
   namespace IO
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Creates an xml reader from an input stream</summary>
      /// <param name="src">The input stream</param>
      /// <exception cref="Logic::ArgumentException">Stream is not readable</exception>
      /// <exception cref="Logic::ArgumentNullException">Stream is null</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      XmlReader::XmlReader(StreamPtr src) : Input(src)
      {
         REQUIRED(src);

         // Ensure stream has read access
         if (!Input->CanRead())
            throw ArgumentException(HERE, L"src", GuiString(ERR_NO_READ_ACCESS));

         try
         {
            // Create document / Disable async access
            Document = XmlDocumentPtr(__uuidof(Xml::DOMDocument60));
            Document->async = VARIANT_FALSE;
         }
         catch (_com_error& ex) {
            throw ComException(HERE, ex);
         }
      }

      /// <summary>Move from an existing string reader</summary>
      /// <param name="r">The existing reader</param>
      XmlReader::XmlReader(XmlReader&& r) : Document(r.Document), Buffer(std::move(r.Buffer)), Input(std::move(r.Input))
      {
      }

      /// <summary>Closes the input stream</summary>
      XmlReader::~XmlReader()
      {
         if (Input != nullptr)      // Ensure we haven't been moved
            Input->SafeClose();
      }

      // ------------------------------- PUBLIC METHODS -------------------------------

		// ------------------------------ PROTECTED METHODS -----------------------------

      /// <summary>Retrieves the child of a node</summary>
      /// <param name="parent">The node</param>
      /// <param name="index">Zero-based index</param>
      /// <param name="help">Meaning of node</param>
      /// <returns>Child node</returns>
      /// <exception cref="Logic::ArgumentNullException">Parent is null</exception>
      /// <exception cref="Logic::FileFormatException">Invalid index</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      XmlNodePtr  XmlReader::GetChild(XmlNodePtr& parent, UINT index, const WCHAR* help)
      {
         REQUIRED(parent);

         // Verify index
         if (index >= (UINT)parent->childNodes->length)
            throw FileFormatException(HERE, GuiString(L"Cannot read %s from node %d of %d", help, index+1, parent->childNodes->length));

         // Retrieve child
         return parent->childNodes->item[index];
      }


      /// <summary>Parses the entire contents of the input stream</summary>
      /// <exception cref="Logic::ComException">COM Error</exception>
      /// <exception cref="Logic::FileFormatException">Error in the XML</exception>
      /// <exception cref="Logic::InvalidOperationException">Document already loaded</exception>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      void  XmlReader::LoadDocument()
      {
         try
         {
            // Ensure we haven't already loaded
            if (Buffer != nullptr)
               throw InvalidOperationException(HERE, L"Document has already been loaded");

            // Extract stream
            DWORD length = Input->GetLength();
            Buffer = FileStream::ConvertFileBuffer(Input, length);

            // Load/Parse file : "%s (line %d, char %d)"
            if (Document->loadXML(Buffer.get()) == VARIANT_FALSE)
            {
               Console << "Dumping XML File: " << ENDL << Buffer.get() << ENDL;

               throw FileFormatException(HERE, GuiString(ERR_XML_PARSE_FAILED, (WCHAR*)Document->parseError->reason, Document->parseError->line, Document->parseError->linepos));
            }
         }
         catch (_com_error& ex) {
            throw ComException(HERE, ex);
         }
      }

      /// <summary>Reads the value of an attribute</summary>
      /// <param name="node">The element containing the attribute</param>
      /// <param name="name">The attribute name</param>
      /// <returns>Attribute value</returns>
      /// <exception cref="Logic::ArgumentNullException">Node is null</exception>
      /// <exception cref="Logic::FileFormatException">Attribute is missing</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      wstring  XmlReader::ReadAttribute(XmlNodePtr&  node, const WCHAR*  name)
      {
         REQUIRED(node);

         try
         {
            // Lookup attribute
            XmlNodePtr attr = node->attributes->getNamedItem(name);

            // Ensure present : "Missing '%s' attribute on '<%s>' element"
            if (attr == nullptr)
               throw FileFormatException(HERE, GuiString(ERR_XML_MISSING_ATTRIBUTE, name, (WCHAR*)node->nodeName));

            // Return text
            return (WCHAR*)attr->text;
         }
         catch (_com_error& ex) {
            throw ComException(HERE, ex);
         }
      }

      /// <summary>Verifies the name of an element</summary>
      /// <param name="node">The element</param>
      /// <param name="name">Name of the element</param>
      /// <exception cref="Logic::ArgumentNullException">Node is null</exception>
      /// <exception cref="Logic::FileFormatException">Missing element</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      void  XmlReader::ReadElement(XmlNodePtr&  node, const WCHAR*  name)
      {
         REQUIRED(node);

         try
         {
            // Ensure name correct : "Unexpected '<%s>' element while searching for '<%s>' element"
            if (node->nodeName != _bstr_t(name))
               throw FileFormatException(HERE, GuiString(ERR_XML_UNEXPECTED_ELEMENT, (WCHAR*)node->nodeName, name));
         }
         catch (_com_error& ex) {
            throw ComException(HERE, ex);
         }
      }


      /// <summary>Reads an optional attribute</summary>
      /// <param name="node">The element containing the attribute</param>
      /// <param name="name">Name of attribute</param>
      /// <param name="val">On return, contains attribute value, or empty string</param>
      /// <returns>True/False</returns>
      /// <exception cref="Logic::ArgumentNullException">Node is null</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      bool XmlReader::TryReadAttribute(XmlNodePtr&  node, const WCHAR*  name, wstring& val)
      {
         REQUIRED(node);

         try
         {
            // Read attribute text or set empty string
            XmlNodePtr attr = node->attributes->getNamedItem(name);
            val = (attr == NULL ? L"" : (WCHAR*)attr->text);

            // Return true if successful
            return attr != NULL;
         }
         catch (_com_error& ex) {
            throw ComException(HERE, ex);
         }
      }

		// ------------------------------- PRIVATE METHODS ------------------------------

   }
}
