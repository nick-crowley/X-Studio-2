#pragma once

#include "Common.h"
#include "CommandSyntax.h"
#include "ScriptParameter.h"

namespace Library
{
   namespace Scripts
   {
      class ScriptFile;

      /// <summary></summary>
      class ScriptCommand
      {
         // --------------------- CONSTRUCTION ----------------------

      public:
         ScriptCommand(CommandSyntax& syntax, ParameterArray& params);
         ScriptCommand(CommandSyntax& syntax, UINT  ref, ParameterArray& params);
         virtual ~ScriptCommand();

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
			
		   // ---------------------- ACCESSORS ------------------------			

		   // ----------------------- MUTATORS ------------------------
      public:
         void  Translate(ScriptFile& f);
         void  Generate() {}

		   // -------------------- REPRESENTATION ---------------------
      public:
         CommandSyntax  Syntax;
         ParameterArray Parameters;
         wstring        Text;
         UINT           RefIndex;
      };

      typedef list<ScriptCommand>     CommandList;
   }
}

using namespace Library::Scripts;
