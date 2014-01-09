#pragma once
#include "Common.h"
#include "StringReader.h"
#include "SyntaxFile.h"

namespace Library
{
   namespace Scripts
   {
      namespace IO
      {

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

         // --------------------- PROPERTIES ------------------------
			
		   // ---------------------- ACCESSORS ------------------------
			
		   // ----------------------- MUTATORS ------------------------
      public:
         SyntaxFile   ReadFile();

      private:
         CommandGroup   LookupCommandGroup(const WCHAR* name);
         ParameterType  LookupParameterType(const WCHAR* name);

         bool  ReadSyntax(CommandSyntax::Declaration& dec);
         bool  RequireLine(wstring& line, const WCHAR* content);
         bool  TryReadLine(wstring& line);

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
}

using namespace Library::Scripts::IO;




