#pragma once

#include "Common.h"
#include "XmlReader.h"
#include "DescriptionFile.h"

namespace Logic
{
   namespace IO
   {
      
      /// <summary></summary>
      class DescriptionFileReader : public XmlReader
      {
         // ------------------------ TYPES --------------------------
      private:
         
         // --------------------- CONSTRUCTION ----------------------
      public:
         DescriptionFileReader(StreamPtr in);
         virtual ~DescriptionFileReader();

         NO_COPY(DescriptionFileReader);	// No copy semantics
         NO_MOVE(DescriptionFileReader);	// No move semantics

         // ------------------------ STATIC -------------------------
      

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			
      

         // ----------------------- MUTATORS ------------------------
      public:
         DescriptionFile  ReadFile();

      private:
         CommandDescription  ReadCommand(XmlNodePtr n);
         ConstantDescription ReadConstant(XmlNodePtr n);
         DescriptionMacro    ReadMacro(XmlNodePtr n);
         
         void  ReadCommands(DescriptionFile& file);
         void  ReadConstants(DescriptionFile& file);
         void  ReadMacros(DescriptionFile& file);

         // -------------------- REPRESENTATION ---------------------

      private:
         XmlNodePtr   Root;
      };

   }
}

using namespace Logic::IO;
