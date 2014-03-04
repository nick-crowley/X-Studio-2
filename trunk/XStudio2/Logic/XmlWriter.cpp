#include "stdafx.h"
#include "XmlWriter.h"

namespace Logic
{
   namespace IO
   {
      /// <summary>UTF-8 byte ordering mark</summary>
      const BYTE  utf8_bom[3] = { 0xEF, 0xBB, 0xBF };

      /// <summary>Stylesheet used to indent xml created</summary>
      const CHAR* szStyleSheet = "<?xml version='1.0' encoding='utf-8'?> \r\n"
                                 "<xsl:stylesheet xmlns:xsl='http://www.w3.org/1999/XSL/Transform' version='1.0'> \r\n"
                                 "   <xsl:output method='xml' indent='yes'/>  \r\n"
                                 "      <xsl:template match='@* | node()'> \r\n"
                                 "         <xsl:copy> \r\n"
                                 "            <xsl:apply-templates select='@* | node()'/> \r\n"
                                 "         </xsl:copy> \r\n"
                                 "      </xsl:template> \r\n"
                                 "</xsl:stylesheet> \r\n";

      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Creates an xml writer to a stream</summary>
      /// <exception cref="Logic::ArgumentException">Stream is not writeable</exception>
      /// <exception cref="Logic::ArgumentNullException">Stream is null</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      XmlWriter::XmlWriter(StreamPtr out) : Output(out)
      {
         REQUIRED(out);

         // Ensure stream has write access
         if (!Output->CanWrite())
            throw ArgumentException(HERE, L"out", GuiString(ERR_NO_WRITE_ACCESS));

         try
         {
            // Create document /
            Document = XmlDocumentPtr(__uuidof(Xml::DOMDocument60));
            // Set properties (from MSDN)
            Document->async = VARIANT_FALSE;
            Document->validateOnParse = VARIANT_FALSE;
            Document->resolveExternals = VARIANT_FALSE;
            Document->preserveWhiteSpace = VARIANT_TRUE;
         }
         catch (_com_error& ex) {
            throw ComException(HERE, ex);
         }
      }

      /// <summary></summary>
      XmlWriter::~XmlWriter()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Writes the Xml to the output stream</summary>
      /// <exception cref="Logic::ComException">COM Error</exception>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      /// <remarks>See http://support.microsoft.com/kb/275883 for further details</remarks>
      void  XmlWriter::Close()
      {
         try
         {
            const CHAR* header = "<?xml version=\"1.0\"?>";
            UINT headerLen = lstrlenA(header);

            // Load style sheet
            XmlDocumentPtr style = XmlDocumentPtr(__uuidof(Xml::DOMDocument60));
            style->preserveWhiteSpace = VARIANT_TRUE;
            style->loadXML(szStyleSheet);

            // Perform transformation and convert to UTF8
            string utf = StringResource::Convert( wstring(Document->transformNode(style)), CP_UTF8);

            // HACK: Use of 'xml' property instead of 'save' method removes the encoding processing instruction. Replace it manually.
            if (utf.compare(0, headerLen, header) == 0)
            {
               const CHAR* instruction = "<?xml version='1.0' encoding='utf-8'?>";
               
               // Manually add UTF-8 BOM + processing instruction
               Output->Write(utf8_bom, 3);
               Output->Write((const BYTE*)instruction, lstrlenA(instruction));

               // Save document but strip the header
               Output->Write((BYTE*)utf.c_str() + headerLen, utf.length() - headerLen);
            }
            else
               // If header already correct, output verbatim
               Output->Write((BYTE*)utf.c_str(), utf.length());

            // Close stream
            Output->Close();
         }
         catch (_com_error& ex) {
            throw ComException(HERE, ex);
         }
      }

      /// <summary>Writes an attribute to a node</summary>
      /// <param name="node">The node</param>
      /// <param name="name">The attribute name.</param>
      /// <param name="value">The attribute value.</param>
      /// <exception cref="Logic::ArgumentNullException">Node is null</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      void  XmlWriter::WriteAttribute(XmlElementPtr& node, const wstring& name, const wstring& value)
      {
         REQUIRED(node);

         try
         {
            // Create/set/attach attribute
            XmlAttributePtr attr = Document->createAttribute(name.c_str());
            attr->value = value.c_str();
            node->setAttributeNode(attr);
         }
         catch (_com_error& ex) {
            throw ComException(HERE, ex);
         }
      }

      /// <summary>Writes an attribute to a node</summary>
      /// <param name="node">The node</param>
      /// <param name="name">The attribute name.</param>
      /// <param name="value">The attribute value.</param>
      /// <exception cref="Logic::ArgumentNullException">Node is null</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      void  XmlWriter::WriteAttribute(XmlElementPtr& node, const wstring& name, int value)
      {
         REQUIRED(node);

         // Format int->string
         WCHAR buf[16];
         _itow_s(value, buf, 16, 10);

         // Write string
         WriteAttribute(node, name, wstring(buf));
      }

      /// <summary>Writes a comment at the start of the document</summary>
      /// <param name="txt">The text.</param>
      /// <exception cref="Logic::ComException">COM Error</exception>
      void  XmlWriter::WriteComment(const wstring& txt)
      {
         try
         {
            // Create/Attach comment
            Document->appendChild( Document->createComment(txt.c_str()) );
         }
         catch (_com_error& ex) {
            throw ComException(HERE, ex);
         }
      }

      /// <summary>Writes a comment.</summary>
      /// <param name="parent">The parent node.</param>
      /// <param name="txt">The text.</param>
      /// <exception cref="Logic::ArgumentNullException">Node is null</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      void  XmlWriter::WriteComment(XmlElementPtr& parent, const wstring& txt)
      {
         REQUIRED(parent);

         try
         {
            // Create/Attach comment
            parent->appendChild( Document->createComment(txt.c_str()) );
         }
         catch (_com_error& ex) {
            throw ComException(HERE, ex);
         }
      }

      /// <summary>Writes an element.</summary>
      /// <param name="parent">The element parent.</param>
      /// <param name="name">The element name.</param>
      /// <returns>New element</returns>
      /// <exception cref="Logic::ArgumentNullException">Node is null</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      XmlElementPtr XmlWriter::WriteElement(XmlElementPtr& parent, const wstring& name)
      {
         REQUIRED(parent);

         try
         {
            // Create/Attach/Return element
            XmlElementPtr e = Document->createElement(name.c_str());
            parent->appendChild(e);
            return e;
         }
         catch (_com_error& ex) {
            throw ComException(HERE, ex);
         }
      }

      /// <summary>Writes an element with text</summary>
      /// <param name="parent">The element parent.</param>
      /// <param name="name">The element name.</param>
      /// <param name="name">The text</param>
      /// <returns>New element</returns>
      /// <exception cref="Logic::ArgumentNullException">Node is null</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      XmlElementPtr XmlWriter::WriteElement(XmlElementPtr& parent, const wstring& name, const wstring& txt)
      {
         auto e = WriteElement(parent, name);
         WriteText(e, txt);
         return e;
      }

      /// <summary>Writes a processing instruction.</summary>
      /// <param name="txt">The instruction</param>
      /// <exception cref="Logic::ComException">COM Error</exception>
      void  XmlWriter::WriteInstruction(const wstring& txt)
      {
         try
         {
            auto proc = Document->createProcessingInstruction(L"xml", txt.c_str());
            Document->appendChild(proc);
         }
         catch (_com_error& ex) {
            throw ComException(HERE, ex);
         }
      }


      /// <summary>Writes text to the node</summary>
      /// <param name="node">The node.</param>
      /// <param name="txt">The text.</param>
      /// <exception cref="Logic::ArgumentNullException">Node is null</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      void XmlWriter::WriteText(XmlElementPtr& node, const wstring& txt)
      {
         REQUIRED(node);

         try
         {
            // Create/Attach text
            node->appendChild( Document->createTextNode(txt.c_str()) );
         }
         catch (_com_error& ex) {
            throw ComException(HERE, ex);
         }
      }

      /// <summary>Writes the root element</summary>
      /// <param name="name">The element name.</param>
      /// <returns>Root element</returns>
      /// <exception cref="Logic::InvalidOperationException">Root already added</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      XmlElementPtr XmlWriter::WriteRoot(const wstring& name)
      {
         // Ensure we haven't already added root
         if (Root != nullptr)
            throw InvalidOperationException(HERE, L"Root already present");

         try
         {
            // Create/Attach/Set/Return root
            Root = Document->createElement(name.c_str());
            Document->appendChild(Root);
            return Root;
         }
         catch (_com_error& ex) {
            throw ComException(HERE, ex);
         }
      }

		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

     

   }
}

