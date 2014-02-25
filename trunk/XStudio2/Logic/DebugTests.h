#pragma once

#include "Common.h"
#include "ScriptFile.h"
#include "ScriptParser.h"

namespace Logic
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

   private:
      static void  BatchTest_ScriptCompiler();
      static void  Test_CommandSyntax();
      static void  Test_LanguageFileReader();
      static void  Test_LanguageEditRegEx();
      static void  Test_TFileReader();
      static void  Test_CatalogReader();
      static void  Test_ExpressionParser();
      static void  Test_DescriptionReader();
      static void  Text_DescriptionRegEx();
      static void  Test_FileSystem();
      static void  Test_GZip();
      static void  Test_Lexer();
      static void  Test_Iterator();
      static void  Text_RegEx();
      static void  Test_StringLibrary();
      static void  Test_ScriptCompiler();
      static void  Test_StringParser();
      static void  Test_StringParserRegEx();
      static void  Test_SyntaxWriter();
      static void  Test_XmlWriter();

      // --------------------- PROPERTIES ------------------------
			
		// ---------------------- ACCESSORS ------------------------			

		// ----------------------- MUTATORS ------------------------

		// -------------------- REPRESENTATION ---------------------

   private:
   };
}

using namespace Logic;
