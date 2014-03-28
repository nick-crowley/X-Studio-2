#pragma once

#include "../XML/xml.h"    // Import MS-XML library
#include "Stream.h"

namespace Logic
{
   namespace IO
   {
         
      /// <summary>Writes XML to a stream</summary>
      class LogicExport XmlWriter
      {
         // --------------------- CONSTRUCTION ----------------------

      public:
         XmlWriter(StreamPtr out);
         virtual ~XmlWriter();

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
			
		   // ---------------------- ACCESSORS ------------------------			

		   // ----------------------- MUTATORS ------------------------
      public:
         void          Close();
         void          WriteAttribute(XmlElementPtr& node, const wstring& name, const wstring& value);
         void          WriteAttribute(XmlElementPtr& node, const wstring& name, int value);
         void          WriteComment(const wstring& txt);
         void          WriteComment(XmlElementPtr& parent, const wstring& txt);
         XmlElementPtr WriteElement(XmlElementPtr& parent, const wstring& name);
         XmlElementPtr WriteElement(XmlElementPtr& parent, const wstring& name, const wstring& txt);
         XmlElementPtr WriteElement(XmlElementPtr& parent, const wstring& name, int value);
         void          WriteInstruction(const wstring& txt);
         XmlElementPtr WriteRoot(const wstring& name);
         void          WriteText(XmlElementPtr& node, const wstring& txt);
      
		   // -------------------- REPRESENTATION ---------------------

      private:
         XmlDocumentPtr  Document;
         XmlElementPtr   Root;
         StreamPtr       Output;
      };

   }
}

using namespace Logic::IO;

