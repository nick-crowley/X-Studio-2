#pragma once

#include "Common.h"
#include "XmlReader.h"
#include "ProjectFile.h"

namespace Logic
{
   namespace IO
   {

      /// <summary></summary>
      class LegacyProjectFileReader : public XmlReader
      {
         // ------------------------ TYPES --------------------------
      private:

         // --------------------- CONSTRUCTION ----------------------

      public:
         LegacyProjectFileReader(StreamPtr in);
         virtual ~LegacyProjectFileReader();

         DEFAULT_COPY(LegacyProjectFileReader);	// Default copy semantics
         DEFAULT_MOVE(LegacyProjectFileReader);	// Default move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------
      public:
         ProjectFile  ReadFile(Path path);

      protected:
         void                 LoadDocument() override;
         ProjectFileItem*     ReadDocument(XmlNodePtr node);
         ProjectVariableItem* ReadVariable(XmlNodePtr node);

         // -------------------- REPRESENTATION ---------------------

      protected:
         XmlNodePtr  Root;
      };

   }
}

using namespace Logic::IO;
