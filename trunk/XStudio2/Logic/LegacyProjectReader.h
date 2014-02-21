#pragma once

#include "Common.h"
#include "XmlReader.h"
#include "ProjectFile.h"

namespace Logic
{
   namespace IO
   {

      /// <summary></summary>
      class LegacyProjectReader : public XmlReader
      {
         // ------------------------ TYPES --------------------------
      private:

         // --------------------- CONSTRUCTION ----------------------

      public:
         LegacyProjectReader(StreamPtr in);
         virtual ~LegacyProjectReader();

         DEFAULT_COPY(LegacyProjectReader);	// Default copy semantics
         DEFAULT_MOVE(LegacyProjectReader);	// Default move semantics

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
