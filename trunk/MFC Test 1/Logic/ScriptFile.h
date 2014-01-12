#pragma once
#include "Common.h"
#include "CommandSyntax.h"
#include "ParameterSyntax.h"
#include "ScriptCommand.h"


namespace Logic
{
   namespace Scripts
   {
      /// <summary>An argument or variable used within a script</summary>
      class ScriptVariable
      {
      public:
         ScriptVariable(const wstring& name, UINT index) : Name(name), ID(index), Type(ParameterType::UNDETERMINED)
         {}

         wstring        Name,
                        Description;
         ParameterType  Type;
         UINT           ID;
      };

      /// <summary>Vector of ScriptVariables</summary>
      typedef vector<ScriptVariable>  VariableArray;

      /// <summary></summary>
      class ScriptFile
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         ScriptFile();
         virtual ~ScriptFile();

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
			
		   // ---------------------- ACCESSORS ------------------------
			
         //string   TranslateRtf(UINT indent);
         //ScriptWriter  GetWriter();

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

using namespace Logic::Scripts;
