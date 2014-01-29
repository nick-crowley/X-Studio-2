#pragma once

#include "Common.h"
#include <regex>

namespace Logic
{
   namespace Language
   {
      class LanguageString;

      /// <summary></summary>
      class StringParser
      {
         // ------------------------ TYPES --------------------------
      private:
         // --------------------- CONSTRUCTION ----------------------

      public:
         StringParser(const LanguageString& str, UINT page);
         virtual ~StringParser();

         DEFAULT_COPY(StringParser);	// Default copy semantics
         DEFAULT_MOVE(StringParser);	// Default move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------
      protected:
         wstring  OnFullMarker(wsmatch& match);
         wstring  OnDefaultMarker(wsmatch& match);

      private:
         void  Parse();

         // -------------------- REPRESENTATION ---------------------
      private:
         static const wregex DefaultMarker, 
                             FullMarker, 
                             RemoveComment;

      public:
         const UINT  Page;
         wstring     Text;
         
      private:
         UINT  Position;
      };

   }
}

using namespace Logic::Language;
