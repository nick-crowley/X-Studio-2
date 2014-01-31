#pragma once

#include "Common.h"
#include "CommandSyntax.h"
#include "ScriptParameter.h"
#include "ScriptToken.h"
#include "RtfWriter.h"

namespace Logic
{
   namespace Scripts
   {
      class ScriptFile;

      /// <summary>Branching logic implemented by a script command</summary>
      enum BranchLogic   { None, NOP, If, While, SkipIf, Else, ElseIf, End, Break, Continue };

      /// <summary>Get branch logic name</summary>
      const WCHAR* GetString(BranchLogic l);


      /// <summary>An MSCI script command</summary>
      class ScriptCommand
      {
         // --------------------- CONSTRUCTION ----------------------
      private:
         ScriptCommand();
      public:
         ScriptCommand(const CommandSyntax& syntax, const wstring& text, ParameterArray& params);
         ScriptCommand(const CommandSyntax& syntax, ParameterArray& params, bool comment);
         ScriptCommand(const CommandSyntax& syntax, UINT  ref, ParameterArray& params, bool comment);
         virtual ~ScriptCommand();

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
			
         PROPERTY_GET(BranchLogic,Logic,GetLogic);

		   // ---------------------- ACCESSORS ------------------------			
      public:
         BranchLogic  GetLogic() const;
         wstring      GetLabelName() const;
         UINT         GetJumpDestination() const;
         wstring      GetScriptCallName() const;

         bool  Is(UINT ID) const;
         bool  Is(CommandType t) const;

		   // ----------------------- MUTATORS ------------------------
      public:
         void  SetLabelName(const wstring& name);
         void  Translate(ScriptFile& f);

		   // -------------------- REPRESENTATION ---------------------
      public:
         static ScriptCommand  Unknown;

      public:
         const CommandSyntax  Syntax;
         bool                 Commented;
         ParameterArray       Parameters;
         wstring              Text;
         UINT                 RefIndex;
      };

      /// <summary>List of script commands</summary>
      typedef list<ScriptCommand>   CommandList;
   }
}

using namespace Logic::Scripts;
