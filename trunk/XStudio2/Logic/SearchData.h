#pragma once

#include "Common.h"

namespace Logic
{
   
      /// <summary></summary>
      enum class SearchTarget { Selection, Document, OpenDocuments, ProjectDocuments, ScriptFolder };

      /// <summary>Get search target name</summary>
      wstring  GetString(SearchTarget t);

      /// <summary></summary>
      class SearchData
      {
         // ------------------------ TYPES --------------------------
      private:

         // --------------------- CONSTRUCTION ----------------------

      public:
         SearchData();
         SearchData(const wstring& txt, SearchTarget targ);
         virtual ~SearchData();

         DEFAULT_COPY(SearchData);	// Default copy semantics
         DEFAULT_MOVE(SearchData);	// Default move semantics

         // ------------------------ STATIC -------------------------
      private:
         static const CHARRANGE  NO_MATCH;

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------
      public:
         bool  FindNext();
         void  SetMatch(int pos = -1);

      private:
         void  BuildFileList();

         // -------------------- REPRESENTATION ---------------------

      public:
         list<IO::Path>  Files;
         CHARRANGE       LastMatch;
         SearchTarget    Target;
         wstring         Term;
         bool            Complete;
      };

   
}

using namespace Logic;