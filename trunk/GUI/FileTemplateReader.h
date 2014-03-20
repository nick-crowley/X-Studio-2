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
         TemplateFileReader();
         virtual ~TemplateFileReader();

         NO_COPY(TemplateFileReader);	// Default copy semantics
         NO_MOVE(TemplateFileReader);	// Default move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------
      public:
         list<TemplateFile> ReadFile();

         // -------------------- REPRESENTATION ---------------------

      protected:
      };

   }
}

using namespace GUI::Utils;
