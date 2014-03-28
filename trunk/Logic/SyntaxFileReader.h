#pragma once

#include "SyntaxFile.h"
#include "XmlReader.h"


namespace Logic
{
   namespace IO
   {
      
      /// <summary>Reads strings and pages of an X3 Syntax xml file</summary>
      class LogicExport SyntaxFileReader : XmlReader
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         SyntaxFileReader(StreamPtr in);
         ~SyntaxFileReader();

         // ------------------------ STATIC -------------------------
      private:
         CommandType    ParseCommandType(const GuiString&  t);
         ExecutionType  ParseExecutionType(const GuiString&  t);
         ParameterUsage ParseParameterUsage(const GuiString&  t);
         VArgMethod     ParseVArgMethod(const GuiString&  t);
         VArgSyntax     ParseVArgSyntax(const GuiString&  t);
         UINT           ParseVersions(const GuiString&  t);

         // --------------------- PROPERTIES ------------------------
			
			// ---------------------- ACCESSORS ------------------------

			// ----------------------- MUTATORS ------------------------
      public:
         SyntaxFile    ReadFile(Path path);

      private:
         CommandSyntax ReadCommand(SyntaxFile& f, XmlNodePtr& node);

         // -------------------- REPRESENTATION ---------------------
      };

   }
}

using namespace Logic::IO;
