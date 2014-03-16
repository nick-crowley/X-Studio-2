#pragma once


#include "XmlWriter.h"
#include "ProjectFile.h"

namespace Logic
{
   namespace IO
   {

      /// <summary>Writer for X-Studio II Project files</summary>
      class LogicExport ProjectFileWriter : protected XmlWriter
      {
         // ------------------------ TYPES --------------------------
      private:

         // --------------------- CONSTRUCTION ----------------------
      public:
         ProjectFileWriter(StreamPtr out);
         virtual ~ProjectFileWriter();

         NO_COPY(ProjectFileWriter);	// No copy semantics
         NO_MOVE(ProjectFileWriter);	// No move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------
      public:
         void  Close();
         void  Write(const ProjectFile& f);

      protected:
         void          WriteItem(const ProjectItem& item, XmlElementPtr& parent);
         void          WriteFile(const ProjectItem& file, XmlElementPtr& parent);
         XmlElementPtr WriteFolder(const ProjectItem& folder, XmlElementPtr& parent);
         void          WriteVariable(const ProjectItem& var, XmlElementPtr& parent);

         // -------------------- REPRESENTATION ---------------------

      private:
      };

   }
}

using namespace Logic::IO;
