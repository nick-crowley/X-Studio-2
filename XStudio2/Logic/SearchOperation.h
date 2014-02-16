#pragma once

#include "Logic/Common.h"

namespace Logic
{
   
      /// <summary></summary>
      enum class SearchTarget { Selection, Document, OpenDocuments, ProjectDocuments, ScriptFolder };

      /// <summary></summary>
      class SearchOperation
      {
         // ------------------------ TYPES --------------------------
      private:

         // --------------------- CONSTRUCTION ----------------------

      public:
         SearchOperation();
         SearchOperation(const wstring& txt, SearchTarget targ);
         virtual ~SearchOperation();

         DEFAULT_COPY(SearchOperation);	// Default copy semantics
         DEFAULT_MOVE(SearchOperation);	// Default move semantics

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------

         // ---------------------- ACCESSORS ------------------------			

         // ----------------------- MUTATORS ------------------------
      public:
         bool  FindNext();

      private:
         void  BuildFileList();

         // -------------------- REPRESENTATION ---------------------

      public:
         list<IO::Path>    Files;
         CHARRANGE     LastMatch;
         SearchTarget  Target;
         wstring       Term;
         bool          Complete;
      };

   
}

using namespace Logic;