#pragma once
#include "Common.h"
#include "Stream.h"
#include "msxml.h"

using namespace XML;

namespace Logic
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
         XmlReader::XmlReader(XmlReader&& r);
         virtual ~XmlReader();

         // Prevent copying.  Move assign not impl
         NO_COPY(XmlReader);
         NO_MOVE_ASSIGN(XmlReader);

         // --------------------- PROPERTIES ------------------------
			
		   // ---------------------- ACCESSORS ------------------------
			
		   // ------------------------ STATIC -------------------------

		   // ----------------------- MUTATORS ------------------------

      protected:
         virtual void  LoadDocument();

         wstring  ReadAttribute(XmlNodePtr&  node, const WCHAR*  name);
         void     ReadElement(XmlNodePtr&  node, const WCHAR*  name);
         bool     TryReadAttribute(XmlNodePtr&  node, const WCHAR*  name, wstring& val);

		   // -------------------- REPRESENTATION ---------------------

         XmlDocumentPtr  Document;

      private:
         CharArrayPtr    Buffer;
         StreamPtr       Input;
      };

   }
}

using namespace Logic::IO;
