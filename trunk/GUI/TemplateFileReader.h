#pragma once
#include "DocumentBase.h"
#include "../Logic/XmlReader.h"

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
         list<FileTemplate> ReadFile();

         // -------------------- REPRESENTATION ---------------------

      protected:
      };

   }
}

using namespace GUI::Utils;
