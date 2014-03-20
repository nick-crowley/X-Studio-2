#pragma once
#include "../Logic/XmlReader.h"
#include "TemplateFile.h"

namespace GUI
{
   namespace Utils
   {
      
      /// <summary></summary>
      class FileTemplateReader : public XmlReader
      {
         // ------------------------ TYPES --------------------------
      protected:

         // --------------------- CONSTRUCTION ----------------------

      public:
         FileTemplateReader();
         virtual ~FileTemplateReader();

         NO_COPY(FileTemplateReader);	// Default copy semantics
         NO_MOVE(FileTemplateReader);	// Default move semantics

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
