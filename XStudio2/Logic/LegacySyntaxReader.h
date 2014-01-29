#pragma once
#include "Common.h"
#include "StringReader.h"
#include "SyntaxFile.h"

namespace Logic
{
   namespace IO
   {
      /// <summary>Number of Command groups hard-coded into the legacy file</summary>
      #define  COMMAND_GROUP_COUNT     33

      /// <summary>Number of parameter types hard-coded into the legacy file</summary>
      #define  PARAMETER_SYNTAX_COUNT  72

      /// <summary>Reads the XStudio syntax file</summary>
      class LegacySyntaxReader : protected StringReader
      {
         typedef list<ParameterType>  ParamList;

         // --------------------- CONSTRUCTION ----------------------

      public:
         LegacySyntaxReader(StreamPtr src);
         LegacySyntaxReader(LegacySyntaxReader&& r);
         virtual ~LegacySyntaxReader();

         // Class cannot be copied
         NO_COPY(LegacySyntaxReader);
         // Move assign not implemented
         NO_MOVE_ASSIGN(LegacySyntaxReader);

         // ------------------------ STATIC -------------------------
      private:
         static ParamSyntaxArray  GenerateParams(UINT id, const wstring& syntax, const list<ParameterType>& params);
         static wstring           GenerateSyntax(const wstring& syntax);
         static CommandType       IdentifyType(UINT id, CommandGroup  g);
         static ParameterUsage    IdentifyUsage(UINT id, UINT index);

         // --------------------- PROPERTIES ------------------------
			
		   // ---------------------- ACCESSORS ------------------------
			
		   // ----------------------- MUTATORS ------------------------
      public:
         SyntaxFile   ReadFile();

      private:
         CommandGroup   LookupCommandGroup(const SyntaxFile& f, const wstring& name);
         ParameterType  LookupParameterType(const SyntaxFile& f, const wstring& name);

         bool  ReadSyntax(const SyntaxFile& f, CommandSyntax::Declaration& dec);
         bool  RequireLine(wstring& line, const WCHAR* help);

		   // -------------------- REPRESENTATION ---------------------

      private:
         // End block marker
         const wstring END_BLOCK = L"-------------- END DEFINITION ---------------";
         
         // Hard-coded Command group names
         static const WCHAR*  szCommandGroups[COMMAND_GROUP_COUNT];

         // Hard-coded parameter syntax
         static const WCHAR*  szParameterSyntax[PARAMETER_SYNTAX_COUNT];

      };

   }
}

using namespace Logic::IO;




