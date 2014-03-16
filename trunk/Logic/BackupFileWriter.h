#pragma once


#include "XmlWriter.h"
#include "BackupFile.h"

namespace Logic
{
   namespace IO
   {

      /// <summary>Writer for X-Studio II Backup files</summary>
      class LogicExport BackupFileWriter : protected XmlWriter
      {
         // ------------------------ TYPES --------------------------
      private:

         // --------------------- CONSTRUCTION ----------------------
      public:
         BackupFileWriter(StreamPtr out);
         virtual ~BackupFileWriter();

         NO_COPY(BackupFileWriter);	// No copy semantics
         NO_MOVE(BackupFileWriter);	// No move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------
      public:
         void  Close();
         void  WriteFile(const BackupFile& f);

      protected:
         void  WriteRevision(const ScriptRevision& r, XmlElementPtr& parent);

         // -------------------- REPRESENTATION ---------------------

      private:
      };

   }
}

using namespace Logic::IO;
