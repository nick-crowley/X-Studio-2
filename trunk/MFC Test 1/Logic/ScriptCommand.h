#pragma once

#include "Common.h"
#include "CommandSyntax.h"
#include "ScriptParameter.h"
#include "RtfWriter.h"

namespace Logic
{
   namespace Scripts
   {
      class ScriptFile;

      /// <summary></summary>
      enum BranchLogic   { None, NOP, If, While, SkipIf, Else, ElseIf, End, Break, Continue };
      const WCHAR* GetString(BranchLogic l);

      /// <summary></summary>
      class ScriptCommand
      {
         // --------------------- CONSTRUCTION ----------------------

      public:
         ScriptCommand(const CommandSyntax& syntax, ParameterArray& params);
         ScriptCommand(const CommandSyntax& syntax, UINT  ref, ParameterArray& params);
         virtual ~ScriptCommand();

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
			
		   // ---------------------- ACCESSORS ------------------------			

         BranchLogic  GetConditional() const;

		   // ----------------------- MUTATORS ------------------------
      public:
         /*void  TranslateRtf(ScriptFile& f, RtfWriter& w);
         void  Generate() {}*/

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

using namespace Logic::Scripts;
