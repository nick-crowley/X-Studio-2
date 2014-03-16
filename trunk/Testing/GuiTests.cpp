#include "stdafx.h"
#include "GuiTests.h"
#include "../XStudio2/ScriptDocument.h"
#include "../XStudio2/DiffDocument.h"

namespace Testing
{
   // -------------------------------- CONSTRUCTION --------------------------------

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------


   void  GuiTests::RunAll()
   {
      //AfxGetMainWnd()->UpdateDialogControls(AfxGetMainWnd(), TRUE);

      /*int* violation= nullptr;
      *violation = 42;*/
      //throw exception("hahaha");

      //Test_DiffDocument();

      //Test_GZip_Compress();
      

      //Test_LanguageFileReader();
      //Test_LanguageEditRegEx();
      //Test_CatalogReader();
      //Test_GZip_Decompress();
      //Test_FileSystem();
      //Test_CommandSyntax();
      //Test_StringLibrary();
      //Test_XmlWriter();
      //Test_SyntaxWriter();
      //Test_ExpressionParser();
      //Test_TFileReader();
      //Text_RegEx();
      //Test_Iterator();
      //BatchTest_ScriptCompiler();
      //Test_Lexer();

      //theApp.WriteString(L"example", L"writeString");
      //theApp.WriteProfileStringW(L"Settings", L"example 1", L"WriteProfileStringW");
      //theApp.WriteSectionString(L"Settings", L"example 2", L"WriteSectionString");
      
      //PrefsLib.LargeToolbars = true;
      //AfxGetMainWnd()->SendMessageToDescendants(WM_SETTINGCHANGE, NULL, NULL);

      //Test_ScriptCompiler();
      
      //Test_StringParser();
      //Test_StringParserRegEx();

      //Test_DescriptionReader();
      //Text_DescriptionRegEx();

      //Console << theApp.GetRegSectionPath(L"Settings") << ENDL;

   }

	// ------------------------------ PROTECTED METHODS -----------------------------

	// ------------------------------- PRIVATE METHODS ------------------------------
   
   void  GuiTests::Test_DiffDocument()
   {
      if (!ScriptDocument::GetActive())
         return;

      // Get document/template
      auto doc = ScriptDocument::GetActive();
      auto templ = theApp.GetDocumentTemplate<DiffDocTemplate>();

      // Create different version
      FileStream fs(L"D:\\My Projects\\XStudio2\\Files\\add.dynamic.logic.diff", FileMode::OpenExisting, FileAccess::Read);
      auto bytes = fs.ReadAllBytes();
      auto alt = GuiString::Convert((char*)bytes.get(), CP_ACP);

      /*auto alt = doc->GetAllText();
      alt.erase(50, 15);
      alt.insert(140, L"NEW NEW NEW NEW NEW");
      alt.insert(70, L"NEW NEW NEW NEW NEW");
      alt.insert(10, L"NEW NEW NEW NEW NEW");*/

      // Open document
      templ->OpenDocumentFile(*doc, alt);
   }

}