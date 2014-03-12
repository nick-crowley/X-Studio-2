#pragma once

#include "Common.h"
#include "XmlReader.h"
#include "BackupFile.h"

namespace Logic
{
   namespace IO
   {
      
      /// <summary></summary>
      class BackupFileReader : public XmlReader
      {
         // ------------------------ TYPES --------------------------
      private:
         
         // --------------------- CONSTRUCTION ----------------------
      public:
         BackupFileReader(StreamPtr in);
         virtual ~BackupFileReader();

         NO_COPY(BackupFileReader);	// No copy semantics
         NO_MOVE(BackupFileReader);	// No move semantics

         // ------------------------ STATIC -------------------------
      

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			
      

         // ----------------------- MUTATORS ------------------------
      public:
         BackupFile  ReadFile();

      private:
         ScriptRevision  ReadScriptRevision(XmlNodePtr n);
         
         void  ReadScriptRevisions(BackupFile& file);

         // -------------------- REPRESENTATION ---------------------

      private:
         XmlNodePtr   Root;
      };

   }
}

using namespace Logic::IO;
