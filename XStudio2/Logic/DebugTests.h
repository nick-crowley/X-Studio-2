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

      //static ScriptParser::ScriptTree CompileScript(const vector<wstring>& lines);
      //static ScriptFile  LoadScript(const WCHAR* path);

   private:
      static void  Test_BatchScriptCompile();
      static void  Test_CommandSyntax();
      static void  Test_LanguageFileReader();
      static void  Test_TFileReader();
      static void  Test_CatalogReader();
      static void  Test_ExpressionParser();
      static void  Test_FileSystem();
      static void  Test_GZip();
      static void  Test_Iterator();
      static void  Text_RegEx();
      static void  Test_StringLibrary();
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
