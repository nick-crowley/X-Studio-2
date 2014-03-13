#include "stdafx.h"
#include "DebugTests.h"
#include "FileStream.h"
#include "CatalogStream.h"
#include "GZipStream.h"
#include "StringReader.h"
#include "LanguageFileReader.h"
#include "XFileSystem.h"
#include "LegacySyntaxFileReader.h"
#include "SyntaxLibrary.h"
#include "ScriptFileReader.h"
#include "StringLibrary.h"
#include "XmlWriter.h"
#include "SyntaxWriter.h"
#include "ExpressionParser.h"
#include "CommandLexer.h"
#include "TWare.h"
#include "TLaser.h"
#include "StringResolver.h"
#include "RichStringParser.h"
#include "DescriptionFileReader.h"
#include "../Testing/ScriptValidator.h"
#include "dtl/dtl.hpp"
#include "../ScriptDocument.h"
#include "../DiffDocument.h"

namespace Logic
{
   // -------------------------------- CONSTRUCTION --------------------------------

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------


   void  DebugTests::RunAll()
   {
      //AfxGetMainWnd()->UpdateDialogControls(AfxGetMainWnd(), TRUE);

      /*int* violation= nullptr;
      *violation = 42;*/
      //throw exception("hahaha");

      //Test_DiffDocument();

      Test_GZip_Compress();
      

      //Test_LanguageFileReader();
      //Test_LanguageEditRegEx();
      //Test_CatalogReader();
      //Test_GZip();
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
   
   void DebugTests::Test_ScriptCompiler()
   {
      // Located problem: egosoft inserts JMPs to end-of-conditional even after RETURNs, except when it's the final RETURN of the script.

      ScriptValidator sv1(L"D:\\X3 Albion Prelude\\scripts\\!job.special.spacefly.hunter.pck");
      sv1.Print();

      ScriptValidator sv2(L"D:\\X3 Albion Prelude\\scripts\\!lib.war.races.pck");
      sv2.Validate();
   }

   void DebugTests::BatchTest_ScriptCompiler()
   {
      XFileSystem     vfs;
      vector<wstring> SkipList = 
      {
         //L"plugin.com.logistics.main.mk1.pck",      // X3TC: Corrupt <codearray> line number
         L"!move.resupply.group.pck",                 // X3TC/X3AP: Unsupported '= + ..' syntax
         //L"aab.query.menu.xml",                     // X3TC: Contains command comment encoded as comment
         //L"plugin.com.logistics.config.mk2.pck",    // X3TC: Contains command comment encoded as comment
         //L"plugin.piracy.detectbase.xml",           // X3TC: Contains command comment encoded as comment
         L"!lib.get.randomshiptype.pck",              // X3AP: Not all control paths return a value
      };

      // Feedback
      Console << Cons::Heading << L"Performing MSCI script batch test: " << ENDL;

      // Browse scripts in VFS
      vfs.Enumerate(L"D:\\X3 Albion Prelude", GameVersion::AlbionPrelude);
      UINT count = 0, total = vfs.Browse(XFolder::Scripts).size() - SkipList.size();

      // Browse scripts
      for (auto& f : vfs.Browse(XFolder::Scripts))
      {
         // Ensure PCK/XML
         if (f.FullPath.HasExtension(L".pck") || f.FullPath.HasExtension(L".xml"))
         {
            // Check Skip list
            if (any_of(SkipList.begin(), SkipList.end(), [f](wstring& path) {return f.FullPath.FileName == path;}) )
            {
               Console << Cons::Heading << L"Skipping script with known incompatiblity: " << f.FullPath << ENDL;
               continue;
            }
            // Skip my scripts, they contain commands encoded as COMMENT
            if (GuiString(f.FullPath.FileName).Left(lstrlen(L"plugin.piracy")) == L"plugin.piracy")
               continue;

            // Validate
            ScriptValidator sv(f.FullPath);
            if (!sv.Validate())
               break;
         }

         // Maintain feedback counter
         count++;
      }

      // Feedback
      Console << Cons::Yellow << "Validated " << count << " of " << total << " scripts..." << ENDL;
   }


   void DebugTests::Test_CommandSyntax()
   {
      const WCHAR* path = L"D:\\My Projects\\MFC Test 1\\MFC Test 1\\plugin.piracy.enslavepassengers.xml"; 

      try
      {
         //// Test LegacySyntaxFileReader
         //SyntaxLib.Enumerate(nullptr);

         //// Test Lookup
         //SyntaxLib.Find(CMD_EXPRESSION, GameVersion::TerranConflict);

         //// Test script reader
         //StreamPtr fs2( new FileStream(path, FileMode::OpenExisting, FileAccess::Read) );
         //auto script = ScriptFileReader(fs2).ReadFile();
      }
      catch (ExceptionBase&  e)
      {
         CString sz;
         sz.Format(L"Unable to load '%s' : %s\n\n" L"Source: %s()", path, e.Message.c_str(), e.Source.c_str());
         AfxMessageBox(sz);
      }
   }
   
   void  DebugTests::Test_CatalogReader()
   {
      const WCHAR* path = L"D:\\X3 Albion Prelude\\11.cat";
   
      try
      {
         // Test catalogue reader
         StreamPtr cs( new CatalogStream(path, FileMode::OpenExisting, FileAccess::Read) );
         StringReader rd(cs);
      
         wstring line;
         while (rd.ReadLine(line))
            OutputDebugString((line+L'\n').c_str());
      }
      catch (ExceptionBase&  e)
      {
         CString sz;
         sz.Format(L"Unable to load '%s' : %s\n\n" L"Source: %s()", path, e.Message.c_str(), e.Source.c_str());
         AfxMessageBox(sz);
      }
   }


   void  DebugTests::Test_DescriptionReader()
   {
      const AppPath path = L"Descriptions.xml";
   
      try
      {
         // Load descriptions
         StreamPtr cs( new FileStream(path, FileMode::OpenExisting, FileAccess::Read) );

         // Test description reader
         DescriptionFileReader rd(cs);
         DescriptionFile f(rd.ReadFile());
      
         /*wstring line;
         while (rd.ReadLine(line))
            OutputDebugString((line+L'\n').c_str());*/
      }
      catch (ExceptionBase&  e)
      {
         Console.Log(HERE, e, L"Unable to load descriptions");
      }
   }

   void DebugTests::Test_DescriptionRegEx()
   {
      try
      {
         wstring txt = L"The QUICK brown FOx juMPED OVER the LAZY dog \n\
                        {NONE} \n\
                        {SINGLE:Interrupts and Yielding}\n \
                        {DOUBLE:first,second}\n\
                        {TRIPLE:first,second,third}\n \
                        {QUADRUPLE:first first,second,third third,fourth} \n\
                        {QUINTUPLE:First first FIRST first,second SECOND,third THIRD,fourth fourth FOURTH,fifth fifth}";

         Console << Cons::Heading << "Using source text: " << Cons::White << txt << ENDL;

         const wchar* keywords = L"\\b([A-Z_0-9]+)(?![:\\}])\\b" L"|" L"\\{([A-Z_0-9]+)\\}";
         const wregex MatchKeyword(keywords);

         // KEYWORDS TEST
         Console << Cons::Heading << "Matching keywords: " << Cons::White << keywords << ENDL;

         // All matches:
         for (wsregex_iterator it(txt.begin(), txt.end(), MatchKeyword), end; it != end; ++it)
         {
            Console << "Instance found:" << ENDL;
            for (auto m : *it)
               Console << (m.matched ? Cons::Green : Cons::Red) << "  Matched: " << m.str().c_str() << ENDL;
         }




         // PARAMETERIZED MACROS TEST
         Console << Cons::Heading << "Using source text: " << Cons::White << txt << ENDL;

         const wchar* macros = L"\\{([A-Z_0-9]+):(.+)\\}";
         const wregex MatchMacro(macros);

         // Feedback
         Console << Cons::Heading << "Matching macros: " << Cons::White << macros << ENDL;

         // All matches:
         for (wsregex_iterator it(txt.begin(), txt.end(), MatchMacro), end; it != end; ++it)
         {
            Console << "Instance found:" << ENDL;
            for (auto m : *it)
               Console << (m.matched ? Cons::Green : Cons::Red) << "  Matched: " << m.str().c_str() << ENDL;
         }



         
         // PARAMETERS TEST
         txt = L"First first FIRST first,second SECOND,third THIRD,fourth fourth FOURTH,fifth fifth";
         Console << Cons::Heading << "Using source text: " << Cons::White << txt << ENDL;
         
         const wchar* params = L"([^,]+)((?=,)[^,]+)*";
         const wregex MatchParams(params);

         // Feedback
         Console << Cons::Heading << "Matching parameters: " << Cons::White << params << ENDL;

         // Match:
         /*wsmatch matches;
         if (!regex_match(txt.cbegin(), txt.cend(), matches, MatchParams))
            Console << Cons::Red << "Failed" << ENDL;
         else
         {
            Console << "Instance found:" << ENDL;
            for (auto m : matches)
               Console << (m.matched ? Cons::Green : Cons::Red) << "  Matched: " << m.str().c_str() << ENDL;
         }*/
         for (wsregex_iterator it(txt.begin(), txt.end(), MatchParams), end; it != end; ++it)
         {
            Console << "Instance found:" << ENDL;
            for (auto m : *it)
               Console << (m.matched ? Cons::Green : Cons::Red) << "  Matched: " << m.str().c_str() << ENDL;
         }
      }
      catch (regex_error& e)
      {
         Console.Log(HERE, RegularExpressionException(HERE, e));
      }
   }

   void  DebugTests::Test_DiffDocument()
   {
      if (!ScriptDocument::GetActive())
         return;

      // Get document/template
      auto doc = ScriptDocument::GetActive();
      auto templ = theApp.GetDocumentTemplate<DiffDocTemplate>();

      // Create different version
      FileStream fs(L"D:\\My Projects\\XStudio2\\Files\\add.dynamic.logic.diff", FileMode::OpenExisting, FileAccess::Read);
      auto bytes = fs.ReadAllBytes();
      auto alt = StringResource::Convert((char*)bytes.get(), CP_ACP);

      /*auto alt = doc->GetAllText();
      alt.erase(50, 15);
      alt.insert(140, L"NEW NEW NEW NEW NEW");
      alt.insert(70, L"NEW NEW NEW NEW NEW");
      alt.insert(10, L"NEW NEW NEW NEW NEW");*/

      // Open document
      templ->OpenDocumentFile(*doc, alt);
   }

   void  DebugTests::Test_ExpressionParser()
   {
      try
      {
         const wchar* expressions[] = 
         { 
            L"4-5",              // identify subtraction/Minus 
            L"4 - 5",            // simple Subtraction
            L"-$a",              // Minus variable
            L"4 + -$a",          // Minus variable in expression
            L"4+-$a",            // Minus variable in expression without spacing
            L"-(4+5)",           // Minus sub-expression
            L"4--5",             // Subtraction vs Minus
            L"4 + 5 - -(1+2)",   // Double-minus and brackets
            L"((-$target.shield) * 100) / $damage"   // problem from !fight.torus.turret.pck
         }; 
         
         Console << Cons::Heading << "Performing expression parser test..." << ENDL;

         // Parse
         for (int i = 0; i < sizeof(expressions)/sizeof(wchar*); ++i)
         {
            Console << ENDL << "Parsing '" << expressions[i] << "'..." << ENDL;
            CommandLexer lex(expressions[i]);

            try
            {
               ExpressionParser(lex.Tokens.begin(), lex.Tokens.end());
               Console << Cons::Green << "Success" << ENDL;
            }
            catch (ExpressionParserException& e)
            {
               Console << Cons::Red << "Failed: " << e.Message << L" " << (lex.Valid(e.Position) ? e.Position->Text : L"") << ENDL;
            }
         }

         // Test Lexer
         //const WCHAR* cmd = L"$szTemp = sprintf: fmt='iMaximumDistance = %s   iAttackTimeout = %s', $iMaximumDistance, $iAttackTimeout, null, null, null";
         //CommandLexer lex(cmd);

         //// Test lexer
         //cmd = L"= [PLAYERSHIP]-> add $iPassengersEnslaved units of {Slaves}";
         //CommandLexer lex2(cmd);

         //// Test lexer
         //cmd = L"* INFO: `WARNING: The %s have revoked your police license.`";
         //CommandLexer lex3(cmd);
         //
         //// Test lexer
         //cmd = L"do if $szItemChosen == 'Option.Quit' OR $szItemChosen == -1";
         //CommandLexer lex4(cmd);

         //// Test lexer
         //cmd = L"$oWeapon = $oRequirement[2]";
         //CommandLexer lex5(cmd);
         //
         //// Test lexer
         //cmd = L"$bMatch = ($BONUS.MARINES.COUNT >= $iActorCount) AND ($BONUS.SIDEARMS.COUNT >= $iActorCount + 1)";
         //CommandLexer lex6(cmd);
      }
      catch (...)
      {
         AfxMessageBox(L"Unable to parse expression");
      }

   }

   void  DebugTests::Test_FileSystem()
   {
      XFileSystem vfs;
      CString err;
   
      try 
      {
         // Test XFileSystem
         vfs.Enumerate(L"D:\\X3 Albion Prelude", GameVersion::TerranConflict, nullptr);

         // Test browse
         for (auto f : vfs.Browse(L"D:\\X3 Albion Prelude\\t"))
            Console << L"Found file: " << f.FullPath << ENDL;

         // Test search
         XFileInfo f = vfs.Find(L"D:\\X3 Albion Prelude\\types\\TLaser");
         StringReader reader(f.OpenRead());

         // Print file
         wstring line;
         while (reader.ReadLine(line))
            Console.WriteLnf(line);
      } 
      catch (ExceptionBase&  e) {
         err.Format(L"Unable to enumerate VFS : %s\n\n" L"Source: %s()", e.Message.c_str(), e.Source.c_str());
         AfxMessageBox(err);
         return;
      }
   }

   void  DebugTests::Test_Lexer()
   {
      try
      {
         const wchar* expressions[] = 
         { 
            L"{Game Object}",          // Game object
            L"{Game {Invalid}}"        // Illegal brackets within game object
         }; 
         
         Console << Cons::Heading << "Performing lexer test..." << ENDL;

         // Parse
         for (int i = 0; i < sizeof(expressions)/sizeof(wchar*); ++i)
         {
            Console << ENDL << "Lexing " << Cons::Yellow << expressions[i] << ENDL;
            CommandLexer lex(expressions[i]);

            for (auto tok : lex.Tokens)
               Console << tok << ENDL;
         }
      }
      catch (ExceptionBase& e)
      {
         Console.Log(HERE, e);
      }
   }
   
   void  DebugTests::Test_GZip()
   {
      const WCHAR* path = L"D:\\X3 Albion Prelude\\scripts\\!config.earth.torus.pck";
   
      try
      {
         // Test GZipStream
         StreamPtr file = StreamPtr( new FileStream(path, FileMode::OpenExisting, FileAccess::Read) );
         StreamPtr zip = StreamPtr( new GZipStream(file, GZipStream::Operation::Decompression) );

         StringReader reader(zip);
         wstring line;

         while (reader.ReadLine(line))
            Console.WriteLnf(line);
      }
      catch (ExceptionBase&  e)
      {
         CString sz;
         sz.Format(L"Unable to load '%s' : %s\n\n" L"Source: %s()", path, e.Message.c_str(), e.Source.c_str());
         AfxMessageBox(sz);
      }
   }
   
   void  DebugTests::Test_GZip_Compress()
   {
      const WCHAR* path = L"D:\\My Projects\\XStudio2\\Files\\plugin.piracy.astronaut.cmd.repair.xml";
   
      try
      {
         // input
         StreamPtr input( new FileStream(path, FileMode::OpenExisting, FileAccess::Read) );

         // output
         StreamPtr output( new FileStream(L"d:\\Temp\\test.zip", FileMode::CreateAlways, FileAccess::Write) );
         StreamPtr zip( new GZipStream(output, GZipStream::Operation::Compression) );

         // Read input
         auto length = input->GetLength();
         auto bytes = input->ReadAllBytes();
         
         // Write .ZIP
         zip->Write(bytes.get(), length);
         zip->Close();
      }
      catch (ExceptionBase&  e) {
         theApp.ShowError(HERE, e, L"Unable to compress");
      }
   }

   void  DebugTests::Test_Iterator()
   {
      const WCHAR* path = L"D:\\My Projects\\MFC Test 1\\MFC Test 1\\testfile.xml"; 
   
      try
      {
         // Read file
         StreamPtr fs( new FileStream(path, FileMode::OpenExisting, FileAccess::Read) );
         auto langFile = LanguageFileReader(fs).ReadFile(path);

         // Test iterator
         Console << Cons::Heading << "Testing Language file iterator..." << ENDL;
         for (const LanguagePage& page : langFile)
            for (const LanguageString& str : page)
               Console << GuiString(L"{%d:%d} %s", str.Page, str.ID, str.Text.c_str()) << ENDL;
      }
      catch (ExceptionBase&  e)
      {
         CString sz;
         sz.Format(L"Unable to load '%s' : %s\n\n" L"Source: %s()", path, e.Message.c_str(), e.Source.c_str());
         AfxMessageBox(sz);
      }
   }
   
   void  DebugTests::Test_LanguageFileReader()
   {
      //const WCHAR* path = L"D:\\My Projects\\MFC Test 1\\MFC Test 1\\testfile.xml"; 
      const WCHAR* path = L"D:\\My Projects\\BearScript\\Data\\Relevant Files\\AP.0001-L044.xml";
   
      try
      {
         // Test LanguageFileReader
         StreamPtr fs( new FileStream(path, FileMode::OpenExisting, FileAccess::Read) );
         auto langFile = LanguageFileReader(fs).ReadFile(path);
      }
      catch (ExceptionBase&  e)
      {
         CString sz;
         sz.Format(L"Unable to load '%s' : %s\n\n" L"Source: %s()", path, e.Message.c_str(), e.Source.c_str());
         AfxMessageBox(sz);
      }
   }

   void  DebugTests::Test_LanguageEditRegEx()
   {
      try
      {
         // Init
         //wstring text = L"[author][green]Factory Complex Constructor[/green][/author]\\n[title]System report[/title]step: [right]%sm.[/right]Rotation [select value=\"dereg\"]Reset Hotkeys[/select] [select value=\"uninstall\"]Uninstall FCC[/select]";
         //
         //// Highlight simple tags    // select/text/ranking
         //wregex MatchTag(L"\\[/?(b|i|u|left|right|center|" L"author|title|text|select|" L"black|blue|cyan|green|orange|magenta|white|yellow)\\]");
         //Console << "Searching text: " << Cons::Yellow << text << ENDL;

         //for (wsregex_iterator it(text.cbegin(), text.cend(), MatchTag), end; it != end; ++it)
         //{
         //   Console << "Matched: " << it->str().c_str() << ENDL;
         //   for (auto m : *it)
         //      Console << (m.matched ? Cons::Green : Cons::Red) << "  SubMatch: " << m.str().c_str() << ENDL;
         //}


         // source
         wstring text2 = L"[text]blurb[/text]\n"                                                // no properties
                         L"[text cols=\"4\"]blurb[/text]\n"                                     // one property
                         L"[text cols=\"4\" colwidth=\"232\"]blurb[/text]\n"                    // two properties
                         L"[text cols=\"4\" colwidth=\"232\" colspacing=\"100\"]blurb[/text]\n" // three properties
                         L"[select value=\"uninstall\"]Uninstall FCC[/select]";

         // Highlight tags with properties
         wregex MatchComplexTag(L"\\[" L"(select|text|rank)(\\s+(value|cols|colwidth|colspacing)=\"[^\"]*\")*"  L"\\]");
         Console << "Searching text: " << Cons::Yellow << text2 << ENDL;

         // Using iterator: Only returns the last property matched
         for (wsregex_iterator it(text2.cbegin(), text2.cend(), MatchComplexTag), end; it != end; ++it)
         {
            Console << "Matched: " << it->str().c_str() << ENDL;
            for (auto m : *it)
               Console << (m.matched ? Cons::Green : Cons::Red) << "  SubMatch: " << m.str() << ENDL;
         }



         // Highlight tags with properties
         wregex MatchComplexTag2(L"\\[" L"(select|text|rank)(\\s+(value|cols|colwidth|colspacing)=\"[^\"]*\")?(\\s+(value|cols|colwidth|colspacing)=\"[^\"]*\")?(\\s+(value|cols|colwidth|colspacing)=\"[^\"]*\")?"  L"\\]");
         Console << "Searching text: " << Cons::Yellow << text2 << ENDL;

         // Using iterator: Only returns the last property matched
         for (wsregex_iterator it(text2.cbegin(), text2.cend(), MatchComplexTag2), end; it != end; ++it)
         {
            Console << "Matched: " << it->str().c_str() << ENDL;
            for (auto m : *it)
               Console << (m.matched ? Cons::Green : Cons::Red) << "  SubMatch: " << m.str() << ENDL;
         }



         // Highlight tags with properties using values between inverted commas or apostrophes
         wstring text3 = L"[text]blurb[/text]\n"                        // no properties
                         L"[text cols=\'4\']blurb[/text]\n"             // apostrophes
                         L"[text cols=\"4\"]blurb[/text]\n"             // inverted commas
                         L"[text cols=\"4\']blurb[/text]\n"             // mismatched
                         L"[text cols=\'4\"]blurb[/text]\n"             // mismatched
                         L"[text cols=\"4\" colwidth=\"232\"]blurb[/text]\n"     // two properties
                         L"[text cols=\"4\" colwidth=\"232\" colspacing=\"100\"]blurb[/text]\n";      // three properties

         wregex MatchComplexTag3(L"\\[" L"(select|text|rank)" L"(\\s+(value|cols|colwidth|colspacing)=([\"\'][^\"\']*[\"\']))?" L"(\\s+(value|cols|colwidth|colspacing)=([\"\'][^\"\']*[\"\']))?" L"(\\s+(value|cols|colwidth|colspacing)=([\"\'][^\"\']*[\"\']))?"  L"\\]");
         Console << "Searching text: " << Cons::Yellow << text2 << ENDL;

         // Using iterator: Only returns the last property matched
         for (wsregex_iterator it(text3.cbegin(), text3.cend(), MatchComplexTag3), end; it != end; ++it)
         {
            Console << "Matched: " << it->str().c_str() << ENDL;
            for (auto m : *it)
               Console << (m.matched ? Cons::Green : Cons::Red) << "  SubMatch: " << m.str() << ENDL;
         }
      }
      catch (regex_error& e) {
         Console.Log(HERE, RegularExpressionException(HERE, e));
      }
   }
   
   void DebugTests::Text_RegEx()
   {
      wstring txt = L"the quick brown {17,245} jumped over the lazy {24,111}";
      Console << "Matching " << txt << " against \\{(\\d+),(\\d+)\\}" << ENDL;

      try
      {
         
         const wregex reg(L"\\{(\\d+),(\\d+)\\}");
         wsmatch matches;
 
         //vector<int> v;
         //v.push_back(1);
         //v.push_back(2);
 
         //// Token iterator
         //Console << "Using wsregex_token_iterator: " << ENDL;
         //for (wsregex_token_iterator i(txt.begin(), txt.end(), reg, v), end; i != end;)
         //{
         //   int x = _wtoi(i->str().c_str()); ++i;
         //   Console << "Matched: " << x << ENDL;

         //   int y = _wtoi(i->str().c_str()); ++i;
         //   Console << "Matched: " << y << ENDL;
         //}

         // Search
         Console << ENDL << "Using regex_search: " << ENDL;
         if (regex_search(txt, matches, reg))
            for (auto m : matches)
               Console << "Matched: " << m.str().c_str() << ENDL;
            
         // Iterator
         Console << ENDL << "Using wsregex_iterator: " << ENDL;
         for (wsregex_iterator it(txt.begin(), txt.end(), reg), eof; it != eof; ++it)
         {
            Console << "Matched: " << it->str().c_str() << ENDL;
            for (auto m : *it)
               Console << "  SubMatch: " << m.str().c_str() << ENDL;
         }

         ///////////////////////////////////

#define open_bracket  L"\\("
#define close_bracket  L"\\)"
#define backslash      L"\\\\"

         wstring txt2 = L"(comment boo)Hello captain, I have \\(twenty\\) seven bears \\(in my head\\). (throw down) Oops i've ( put a bracket here\\)";
         auto expr = L"(?!" backslash L")" open_bracket L"[^" open_bracket close_bracket backslash L"]+" L"(?!" backslash close_bracket L")" close_bracket;

         Console << ENDL << "Matching '" << Cons::Yellow << txt2 << Cons::White << "' against " << Cons::Yellow << expr << ENDL;

         // Iterator
         const wregex reg2(expr, wregex::ECMAScript); 
         for (wsregex_iterator it(txt2.cbegin(), txt2.cend(), reg2), eof; it != eof; ++it)
            Console << "Matched: " << Cons::Green << it->str().c_str() << ENDL;


         ///////////////////////////////////

         // Ware placeholder test
         const wstring txt3 = L"{SSTYPE_W_TECH@241}";
         const wregex reg3(L"\\{([\\w_]+)@(\\d+)\\}");

         Console << ENDL << "Matching " << txt3 << ENDL;
         
         if (!regex_match(txt3, matches, reg3))
            Console << Cons::Red << "Failed" << ENDL;
         
         else for (auto m : matches)
            Console << "Matched: " << m.str().c_str() << ENDL;

      }
      catch (regex_error& e)
      {
         Console.Log(HERE, RegularExpressionException(HERE, e));
      }
   }

   void  DebugTests::Test_TFileReader()
   {
      const WCHAR* path = L"D:\\My Projects\\BearScript\\Data\\Relevant Files\\TWareT.txt";
   
      try
      {
         // Load string library
         XFileSystem vfs;
         vfs.Enumerate(L"D:\\X3 Albion Prelude", GameVersion::TerranConflict, nullptr);
         StringLib.Enumerate(vfs, GameLanguage::English, nullptr);

         // Read TWareT
         Console << Cons::Heading << L"Reading TFile: " << path << ENDL;
         StreamPtr fs( new FileStream(path, FileMode::OpenExisting, FileAccess::Read) );
         auto file = TWareReader(fs).ReadFile(MainType::TechWare, GameVersion::TerranConflict);

         // Print IDs/Names
         /*for (auto& obj : file.Objects)
         {
            Console << Cons::Yellow << obj.id << Cons::White << L" : ";
            Console << (obj.name.Exists()?Cons::Green:Cons::Red) << (obj.name.Exists()?obj.name.Text:L"Missing") << ENDL;
         }*/

         // Read TLaser
         path = L"D:\\My Projects\\BearScript\\Data\\Relevant Files\\TLaser.txt";
         Console << Cons::Heading << L"Reading TFile: " << path << ENDL;
         StreamPtr fs2( new FileStream(path, FileMode::OpenExisting, FileAccess::Read) );
         auto file2 = TLaserReader(fs2).ReadFile(MainType::Laser, GameVersion::TerranConflict);

         // Print IDs/Names
         /*for (auto& obj : file.Objects)
         {
            Console << Cons::Yellow << obj.id << Cons::White << L" : ";
            Console << (obj.name.Exists()?Cons::Green:Cons::Red) << (obj.name.Exists()?obj.name.Text:L"Missing") << ENDL;
         }*/
      }
      catch (ExceptionBase&  e)
      {
         CString sz;
         sz.Format(L"Unable to load '%s' : %s\n\n" L"Source: %s()", path, e.Message.c_str(), e.Source.c_str());
         AfxMessageBox(sz);
      }
   }

   void  DebugTests::Test_StringLibrary()
   {
      XFileSystem vfs;
      CString err;
   
      try 
      {
         // Test enumerate
         vfs.Enumerate(L"D:\\X3 Albion Prelude", GameVersion::TerranConflict, nullptr);
         StringLib.Enumerate(vfs, GameLanguage::English, nullptr);

         // Test search
         list<LanguageString>  res;

         res.push_back( StringLib.Find(7, 1020112) );
         res.push_back( StringLib.Find(7, 1020114) );
         res.push_back( StringLib.Find(7, 1020117) );
         res.push_back( StringLib.Find(6022, 8) );

         // Print results
         for (LanguageString& s : res)
            Console.WriteLnf(s.Text);
      } 
      catch (ExceptionBase&  e) {
         err.Format(L"Unable to enumerate VFS : %s\n\n" L"Source: %s()", e.Message.c_str(), e.Source.c_str());
         AfxMessageBox(err);
         return;
      }
   }

   void DebugTests::Test_StringParser()
   {
      // Expressions
      vector<wstring> strings = 
      {
         L"",
         L"quick brown fox jumped over the lazy dog",
         L"quick brown fox [b]jumped over[/b] the lazy dog",
         L"quick brown fox [i]jumped over[/i] the lazy dog",
         L"quick brown fox [u]jumped over[/u] the lazy dog",

         L"[author][/author]",
         L"[author][/author][title][/title]",
         L"[author]fred[/author] quick brown fox [b]jumped over[/b] the lazy dog",
         L"[author]fred[/author][title]george[/title] quick brown fox [b]jumped over[/b] the lazy dog",

         L"[left][/left]",
         L"[left]quick brown fox [b]jumped over[/b] the lazy dog[/left]",
         L"[right]quick brown fox [b]jumped over[/b] the lazy dog[/right]",
         L"[center]quick brown fox [b]jumped over[/b] the lazy dog[/center]",
         L"[justify]quick brown fox [b]jumped over[/b] the lazy dog[/justify]",
         
         L"[select][/select]",
         L"[select]jump again[/select]",
         L"[select value='one']jump once[/select] [select value='two']jump twice[/select]",

         L"[text][/text]",
         L"[text]quick brown fox [b]jumped over[/b] the lazy dog[/text]",
         L"[text cols='2']quick brown fox [b]jumped over[/b] the lazy dog[/text]",
         L"[text colwidth='40']quick brown fox [b]jumped over[/b] the lazy dog[/text]",
         L"[text cols='3' colwidth='40']quick brown fox [b]jumped over[/b] the lazy dog[/text]",
         L"[text cols='3' colwidth='40' colspacing='50']quick brown fox [b]jumped over[/b] the lazy dog[/text]",

         L"something something[right]aligned aligned[/right][b]bold bold[/b]"
      };

      // Parse strings
      for (auto str : strings)
      {
         try
         {
            Console << Cons::Heading << "Parsing language string: " << Cons::Yellow << Cons::Bold << str << ENDL;
            RichStringParser p(str);
            Console << p.Output << ENDL;
         }
         catch (ExceptionBase& e) {
            Console.Log(HERE, e);
         }
      }
   }

   void DebugTests::Test_StringParserRegEx()
   {
      wsmatch matches;

      // Expressions
      vector<wstring> expressions = 
      {
         L"[author]",
         L"[author space]",
         L"[author_underscore]",
         L"[author304number]",
         L"[/author]",
         L"[author\\]",
         L"[/author\\]"
      };

      try
      {
         // Basic Tag
         wregex IsBasicTag(L"\\[/?([a-z]+)\\]");
         for (auto expr : expressions)
         {
            Console << Cons::Heading << "Matching basic Tag: " << Cons::White << expr << ENDL;
         
            if (!regex_match(expr, matches, IsBasicTag))
               Console << Cons::Red << "Failed" << ENDL;
         
            else for (auto m : matches)
               Console << Cons::Green << "Matched: " << m.str() << ENDL;
         }

         // Open Tag
         wregex IsOpenTag(L"\\[([a-z]+)\\]");
         for (auto expr : expressions)
         {
            Console << Cons::Heading << "Matching open Tag: " << Cons::White << expr << ENDL;
         
            if (!regex_match(expr, matches, IsOpenTag))
               Console << Cons::Red << "Failed" << ENDL;
         
            else for (auto m : matches)
               Console << Cons::Green << "Matched: " << m.str() << ENDL;
         }

         // Expressions
         expressions = 
         {
            L"[author]hello dolly[/author]something else",
            L"[/author]hello dolly[author]something else",
            L"[author]hello dolly[author]something else",
            L"[/author]hello dolly[/author]something else",
            L"[author]hello dolly[/title]something else",
            L"[author][/author]something else",
            L"[author]hellow dolly[text]something else",
         };

         // Author/Title Tag
         wregex IsAuthorTag(L"\\[author\\](.*)\\[/author\\]");
         for (auto expr : expressions)
         {
            Console << Cons::Heading << "Matching author/title Tag: " << Cons::White << expr << ENDL;
         
            if (!regex_search(expr, matches, IsAuthorTag))
               Console << Cons::Red << "Failed" << ENDL;
         
            else for (auto m : matches)
               Console << Cons::Green << "Matched: " << m.str() << ENDL;
         }

         // Expressions
         expressions = 
         {
            L"bongo [select]button text[/select] bongo",
            L"bongo [select value='id']button text[/select]",
            L"bongo [select value=\"id\"]button text[/select]",
            L"[select value = 'id']button text[/select]",
            L"bongo [select value ='id]button text[select] bongo",
            L"bongo [text cols='54' width='22']paragraph text[/text] bongo",
            L"bongo [text cols='54' width='22' extra='hello']paragraph text[/text] bongo",
         };

         // Complex Tag
         //wregex IsComplexTag(L"\\[([a-z]+)(\\s+([a-z]+)='(\\w+)')+\\]");
         wregex IsComplexTag(L"\\[([a-z]+)(?:\\s+[a-z]+\\s*=\\s*'\\w+')*\\]");
         for (auto expr : expressions)
         {
            Console << Cons::Heading << "Matching complex Tag: " << Cons::White << expr << ENDL;
         
            if (!regex_search(expr, matches, IsComplexTag))
               Console << Cons::Red << "Failed" << ENDL;
         
            else 
            {
               // Initial match
               for (auto m : matches)
                  Console << (m.matched ? Cons::Green : Cons::Red) << "Matched: " << m.str() << ENDL;

               // Properties
               auto properties = matches[0].str();
               Console << Cons::Yellow << "  Iterating over properties: '" << properties << "'..." << ENDL;
               
               wregex IsProperty(L"\\s+([a-z]+)\\s*=\\s*'(\\w+)'");
               for (wsregex_iterator it(properties.begin()+matches[1].length(), properties.end(), IsProperty), eof; it != eof; ++it)
               {
                  Console << Cons::Green << "  Matched: " << it->str() << ENDL;
                  for (auto m : *it)
                     Console << (m.matched ? Cons::Green : Cons::Red) << "    SubMatch: " << m.str() << ENDL;
               }
            }
         }

         
      }
      catch (exception& e)
      {
         Console.Log(HERE, e);
      }
   }
   
   void DebugTests::Test_SyntaxWriter()
   {
      const WCHAR* path = L"c:\\temp\\new Syntax.xml"; 

      try
      {
         // Load legacy syntax file
         StreamPtr fs( new FileStream(L"D:\\My Projects\\MFC Test 1\\MFC Test 1\\Command Syntax.txt", FileMode::OpenExisting, FileAccess::Read) );
         auto file = LegacySyntaxFileReader(fs).ReadFile();

         // Convert to new format
         SyntaxWriter w( StreamPtr(new FileStream(path, FileMode::CreateAlways, FileAccess::Write)) );
         w.Write(file);
         w.Close();
      }
      catch (ExceptionBase&  e)
      {
         CString sz;
         sz.Format(L"Unable to write '%s' : %s\n\n" L"Source: %s()", path, e.Message.c_str(), e.Source.c_str());
         AfxMessageBox(sz);
      }
   }

   void  DebugTests::Test_XmlWriter()
   {
      const WCHAR* path = L"c:\\temp\\XML_TEST.xml"; 
   
      try
      {
         // Test LegacySyntaxFileReader
         StreamPtr fs( new FileStream(path, FileMode::CreateAlways, FileAccess::Write) );
         XmlWriter w(fs);

         w.WriteInstruction(L"version='1.0' encoding='utf-8'");
         w.WriteComment(L"Written by XStudio 2");

         auto root = w.WriteRoot(L"language");
         w.WriteAttribute(root, L"id", 44);

         auto page = w.WriteElement(root, L"page");
         w.WriteAttribute(page, L"id", 1000);
         w.WriteAttribute(page, L"title", L"la la title");

         auto str = w.WriteElement(page, L"t");
         w.WriteAttribute(str, L"id", 1);
         w.WriteText(str, L"I am the text of the string");

         str = w.WriteElement(page, L"t");
         w.WriteAttribute(str, L"id", 2);
         w.WriteText(str, L"АБВГДЖКМПС");

         w.Close();
         fs->Close();

         // Test by reading
         StreamPtr fs2( new FileStream(path, FileMode::OpenExisting, FileAccess::Read) );
         auto langFile = LanguageFileReader(fs2).ReadFile(path);

         // Test fails because debug window doesn't conv to ANSI Correctly, but the strings are read+stored correctly
         for (const auto& pair : langFile.Pages)
            for (const auto& pair2 : pair.second.Strings)
               Console.WriteLnf(L"Read language string %d : '%s'", pair2.second.ID, pair2.second.Text.c_str());
      }
      catch (ExceptionBase&  e)
      {
         CString sz;
         sz.Format(L"Unable to write XML test file '%s' : %s\n\n" L"Source: %s()", path, e.Message.c_str(), e.Source.c_str());
         AfxMessageBox(sz);
      }
   }

}