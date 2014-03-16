#pragma once


#include "Descriptions.h"
#include "CommandSyntax.h"
#include <regex>

namespace Logic
{
   namespace Language
   {

      /// <summary></summary>
      class LogicExport DescriptionParser
      {
         // ------------------------ TYPES --------------------------
      private:

         // --------------------- CONSTRUCTION ----------------------

      public:
         DescriptionParser(const wstring& txt);
         DescriptionParser(const wstring& txt, CommandSyntaxRef cmd);
         virtual ~DescriptionParser();

         NO_COPY(DescriptionParser);	// No copy semantics
         NO_MOVE(DescriptionParser);	// No move semantics

         // ------------------------ STATIC -------------------------
      private:
         static const wregex  MatchKeyword;
         static const wregex  MatchMacro;
         static const wregex  MatchParameters;
         static const wregex  MartchParameterMarker;

      private:
         static wstring  Populate(wstring source, CommandSyntaxRef cmd);
         static wstring  onParameterMarker(wsmatch& match, CommandSyntaxRef cmd);

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			
      private:         
         wstring  onMatchKeyword(const wsmatch& match, int depth) const;
         wstring  onMatchMacro(const wsmatch& match, int depth) const;
         wstring  Parse(wstring text, int depth = 0) const;

         // ----------------------- MUTATORS ------------------------

         // -------------------- REPRESENTATION ---------------------
      public:
         wstring  Text;

      private:
         const UINT  BUFFER_LENGTH = 32*1024;

         CharArrayPtr FormatBuffer;
      };

   }
}

using namespace Logic::Language;
