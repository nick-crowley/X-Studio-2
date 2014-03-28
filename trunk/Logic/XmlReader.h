#pragma once

#include "Stream.h"
#include "../XML/xml.h"      // Import MS-XML library

namespace Logic
{
   namespace IO
   {
      

      /// <summary>Base class LogicExport for DOM based XML readers</summary>
      class LogicExport XmlReader
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

         XmlNodePtr  GetChild(XmlNodePtr& parent, UINT index, const WCHAR* help);
         UINT        GetChildCount(XmlNodePtr& parent, const WCHAR* help);
         wstring     ReadAttribute(XmlNodePtr&  node, const WCHAR*  name);
         void        ReadElement(XmlNodePtr&  node, const WCHAR*  name);
         bool        TryReadAttribute(XmlNodePtr&  node, const WCHAR*  name, wstring& val);

		   // -------------------- REPRESENTATION ---------------------

         XmlDocumentPtr  Document;

      private:
         CharArrayPtr    Buffer;
         StreamPtr       Input;
      };

   }
}

using namespace Logic::IO;
