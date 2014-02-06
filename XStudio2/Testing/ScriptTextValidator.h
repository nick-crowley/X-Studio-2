#pragma once
#include "../Logic/Common.h"
#include "../Logic/ScriptParser.h"

namespace Testing
{
   namespace Scripts
   {

      /// <summary></summary>
      class ScriptTextValidator
      {
         // ------------------------ TYPES --------------------------
      private:

         // --------------------- CONSTRUCTION ----------------------

      public:
         ScriptTextValidator(Path file);
         virtual ~ScriptTextValidator();

         DEFAULT_COPY(ScriptTextValidator);	// Default copy semantics
         DEFAULT_MOVE(ScriptTextValidator);	// Default move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------
      public:
         bool  Validate();

      private:
         bool       Compare(const ScriptFile& in, const ScriptFile& out);
         LineArray  GetAllLines(const CommandList& commands);

         // -------------------- REPRESENTATION ---------------------

      private:
         Path  FullPath;
      };

   }
}

using namespace Testing::Scripts;
