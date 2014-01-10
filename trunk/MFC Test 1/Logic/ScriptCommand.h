#pragma once

#include "Common.h"
#include "CommandSyntax.h"
#include "ScriptParameter.h"

namespace Library
{
   namespace Scripts
   {
         
      /// <summary></summary>
      class ScriptCommand
      {
         // --------------------- CONSTRUCTION ----------------------

      public:
         ScriptCommand(CommandSyntax& syntax, ParameterArray& params) : Syntax(syntax), RefIndex(0), Parameters(params) {}
         ScriptCommand(CommandSyntax& syntax, UINT  ref, ParameterArray& params) : Syntax(syntax), RefIndex(ref), Parameters(params) {}
         virtual ~ScriptCommand();

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
			
		   // ---------------------- ACCESSORS ------------------------			

		   // ----------------------- MUTATORS ------------------------

		   // -------------------- REPRESENTATION ---------------------

         CommandSyntax  Syntax;
         ParameterArray Parameters;
         wstring        Text;
         UINT           RefIndex;

      private:
      };

      typedef list<ScriptCommand>     CommandList;
   }
}

using namespace Library::Scripts;
