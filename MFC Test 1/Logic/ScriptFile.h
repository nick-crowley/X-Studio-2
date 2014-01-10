#pragma once
#include "Common.h"
#include "CommandSyntax.h"
#include "ParameterSyntax.h"
#include "ScriptCommand.h"

namespace Library
{
   namespace Scripts
   {

      class ScriptVariable
      {
      public:
         ScriptVariable(const wstring& name, UINT index) : Name(name), ID(index)
         {}

         wstring        Name,
                        Description;
         ParameterType  Type;
         UINT           ID;
      };

      typedef vector<ScriptVariable>  VariableArray;

      /// <summary></summary>
      class ScriptFile
      {
      public:
         

         // --------------------- CONSTRUCTION ----------------------

      public:
         ScriptFile();
         virtual ~ScriptFile();

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
			
		   // ---------------------- ACCESSORS ------------------------
			
		   // ----------------------- MUTATORS ------------------------

		   // -------------------- REPRESENTATION ---------------------

         wstring     Name,
                     Description;
         UINT        Version;
         GameVersion Game;
         bool        LiveData;

         CommandList   Commands;
         VariableArray Variables;
      private:
      };

   }
}

using namespace Library::Scripts;
