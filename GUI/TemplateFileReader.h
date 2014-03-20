#pragma once
#include "../Logic/XmlReader.h"
#include "TemplateFile.h"

namespace GUI
{
   namespace Utils
   {
      
      /// <summary></summary>
      class TemplateFileReader : public XmlReader
      {
         // ------------------------ TYPES --------------------------
      protected:

         // --------------------- CONSTRUCTION ----------------------

      public:
         TemplateFileReader(StreamPtr in);
         virtual ~TemplateFileReader();

         NO_COPY(TemplateFileReader);	// Default copy semantics
         NO_MOVE(TemplateFileReader);	// Default move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------
      public:
         TemplateList  ReadFile();

      protected:
         TemplateFile  ReadTemplate(XmlNodePtr node);

         // -------------------- REPRESENTATION ---------------------

      protected:
      };

   }
}

using namespace GUI::Utils;
