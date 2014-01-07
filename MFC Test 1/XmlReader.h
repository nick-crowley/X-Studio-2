#pragma once
#include "Stream.h"
#include "msxml.h"

using namespace XML;

namespace Library
{
   namespace IO
   {
      /// <summary>Base class for DOM based XML readers</summary>
      class XmlReader
      {
         // --------------------- CONSTRUCTION ----------------------

      protected:
         XmlReader(StreamPtr in);
      public:
         virtual ~XmlReader();

         // Prevent copying.  Move assign not impl
         NO_COPY(XmlReader);
         NO_MOVE_ASSIGN(XmlReader);

         // --------------------- PROPERTIES ------------------------
			
		   // ---------------------- ACCESSORS ------------------------
			
		   // ------------------------ STATIC -------------------------

		   // ----------------------- MUTATORS ------------------------

      protected:
         void     LoadDocument();
         wstring  ReadAttribute(XML::IXMLDOMNodePtr&  node, const WCHAR*  name);
         void     ReadElement(XML::IXMLDOMNodePtr&  node, const WCHAR*  name);
         wstring  TryReadAttribute(XML::IXMLDOMNodePtr&  node, const WCHAR*  name);

		   // -------------------- REPRESENTATION ---------------------

         IXMLDOMDocument2Ptr  Document;

      private:
         ByteArrayPtr         Buffer;
         StreamPtr            Input;
      };

   }
}
