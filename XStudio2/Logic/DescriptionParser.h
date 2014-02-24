#pragma once

#include "Common.h"
#include "Descriptions.h"
#include <regex>

namespace Logic
{
   namespace Language
   {

      /// <summary></summary>
      class DescriptionParser
      {
         // ------------------------ TYPES --------------------------
      private:

         // --------------------- CONSTRUCTION ----------------------

      public:
         DescriptionParser(const wstring& txt);
         virtual ~DescriptionParser();

         NO_COPY(DescriptionParser);	// No copy semantics
         NO_MOVE(DescriptionParser);	// No move semantics

         // ------------------------ STATIC -------------------------
      private:
         static const wregex  MatchKeyword,
                              MatchMacro,
                              MatchParameters;

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			
      private:         
         wstring  onMatchKeyword(const wsmatch& match, int depth) const;
         wstring  onMatchMacro(const wsmatch& match, int depth) const;
         wstring  Parse(wstring text, int depth = 0) const;
         wstring  PostProcess(wstring text) const;

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
