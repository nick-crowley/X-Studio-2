#pragma once

namespace Testing
{
   /// <summary></summary>
   class LogicExport LogicTests
   {
      // --------------------- CONSTRUCTION ----------------------

   public:
      LogicTests() {};
      virtual ~LogicTests() {};

      // ------------------------ STATIC -------------------------
   public:
      static void  RunAll();

   public:
      static void  BatchTest_ScriptCompiler();
      static void  Test_CommandSyntax();
      static void  Test_LanguageFileReader();
      static void  Test_LanguageEditRegEx();
      static void  Test_TFileReader();
      static void  Test_CatalogReader();
      static void  Test_ExpressionParser();
      static void  Test_DescriptionReader();
      static void  Test_DescriptionRegEx();
      static void  Test_DiffDocument();
      static void  Test_FileSystem();
      static void  Test_GZip_Decompress();
      static void  Test_GZip_Compress();
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
