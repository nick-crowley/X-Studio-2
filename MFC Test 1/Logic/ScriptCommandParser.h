
#pragma once

#include "Common.h"

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         /// <summary></summary>
         class ScriptCommandParser
         {
            // ------------------------ TYPES --------------------------
         private:

            // --------------------- CONSTRUCTION ----------------------

         public:
            ScriptCommandParser();
            virtual ~ScriptCommandParser();

            DEFAULT_COPY(ScriptCommandParser);	// Default copy semantics
            DEFAULT_MOVE(ScriptCommandParser);	// Default move semantics

            // ------------------------ STATIC -------------------------

            // --------------------- PROPERTIES ------------------------

            // ---------------------- ACCESSORS ------------------------			

            // ----------------------- MUTATORS ------------------------

            // -------------------- REPRESENTATION ---------------------

         private:
         };
      }
   }
}

using namespace Logic::Scripts::Compiler;
