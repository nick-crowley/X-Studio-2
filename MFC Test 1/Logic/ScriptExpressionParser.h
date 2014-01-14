#pragma once
#include "Common.h"
#include "ScriptCommandParser.h"

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         /// <summary></summary>
         class ScriptExpressionParser
         {
            // ------------------------ TYPES --------------------------
         private:

            // --------------------- CONSTRUCTION ----------------------

         public:
            ScriptExpressionParser(TokenIterator& begin, TokenIterator& end, TokenArray& params);
            virtual ~ScriptExpressionParser();

            DEFAULT_COPY(ScriptExpressionParser);	// Default copy semantics
            DEFAULT_MOVE(ScriptExpressionParser);	// Default move semantics

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
