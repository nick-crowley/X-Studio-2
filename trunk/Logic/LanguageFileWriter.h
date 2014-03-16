#pragma once


#include "XmlWriter.h"
#include "LanguageFile.h"

namespace Logic
{
   namespace IO
   {

      /// <summary>Writer for language files</summary>
      class LogicExport LanguageFileWriter : protected XmlWriter
      {
         // ------------------------ TYPES --------------------------
      private:

         // --------------------- CONSTRUCTION ----------------------
      public:
         LanguageFileWriter(StreamPtr out);
         virtual ~LanguageFileWriter();

         NO_COPY(LanguageFileWriter);	// No copy semantics
         NO_MOVE(LanguageFileWriter);	// No move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------
      public:
         void  Close();
         void  Write(const LanguageFile& f);

         // -------------------- REPRESENTATION ---------------------

      private:
      };

   }
}

using namespace Logic::IO;
