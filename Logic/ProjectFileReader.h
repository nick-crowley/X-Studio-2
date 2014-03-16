#pragma once


#include "XmlReader.h"
#include "ProjectFile.h"

namespace Logic
{
   namespace IO
   {

      /// <summary></summary>
      class ProjectFileReader : public XmlReader
      {
         // ------------------------ TYPES --------------------------
      private:

         // --------------------- CONSTRUCTION ----------------------

      public:
         ProjectFileReader(StreamPtr in);
         virtual ~ProjectFileReader();

         DEFAULT_COPY(ProjectFileReader);	// Default copy semantics
         DEFAULT_MOVE(ProjectFileReader);	// Default move semantics

         // ------------------------ STATIC -------------------------
      public:
         static FileType ParseFileType(const wstring& type);

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------
      public:
         ProjectFile  ReadFile(Path path);

      protected:
         void        LoadDocument() override;
         ProjectItem ReadFolderItem(XmlNodePtr node);
         ProjectItem ReadFileItem(XmlNodePtr node);
         ProjectItem ReadVariableItem(XmlNodePtr node);

         // -------------------- REPRESENTATION ---------------------

      protected:
         XmlNodePtr  Root;
      };

   }
}

using namespace Logic::IO;
