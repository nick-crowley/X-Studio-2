#pragma once

#include "Common.h"
#include "ScriptFile.h"

namespace Library
{
   /// <summary></summary>
   class DebugTests
   {
      // --------------------- CONSTRUCTION ----------------------

   public:
      DebugTests() {};
      virtual ~DebugTests() {};

      // ------------------------ STATIC -------------------------

   public:
      static void  RunAll();

      static ScriptFile  LoadScript(const WCHAR* path);

   private:
      static void  Test_CommandSyntax();
      static void  Test_LanguageFileReader();
      static void  Test_CatalogReader();
      static void  Test_FileSystem();
      static void  Test_GZip();

      // --------------------- PROPERTIES ------------------------
			
		// ---------------------- ACCESSORS ------------------------			

		// ----------------------- MUTATORS ------------------------

		// -------------------- REPRESENTATION ---------------------

   private:
   };
}

using namespace Library;
