#pragma once

#include "Common.h"
#include "XmlWriter.h"
#include "ProjectFile.h"

namespace Logic
{
   namespace IO
   {

      /// <summary>Writer for X-Studio II Project files</summary>
      class ProjectFileWriter : protected XmlWriter
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
         void          WriteItem(const ProjectItemPtr& item, XmlElementPtr& parent);
         void          WriteFile(const ProjectFileItem* file, XmlElementPtr& parent);
         XmlElementPtr WriteFolder(const ProjectFolderItem* folder, XmlElementPtr& parent);
         void          WriteVariable(const ProjectVariableItem* var, XmlElementPtr& parent);

         // -------------------- REPRESENTATION ---------------------

      private:
      };

   }
}

using namespace Logic::IO;
