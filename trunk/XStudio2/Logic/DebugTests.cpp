#include "stdafx.h"
#include "DebugTests.h"
#include "FileStream.h"
#include "CatalogStream.h"
#include "GZipStream.h"
#include "StringReader.h"
#include "LanguageFileReader.h"
#include "XFileSystem.h"
#include "LegacySyntaxReader.h"
#include "SyntaxLibrary.h"
#include "ScriptFileReader.h"
#include "StringLibrary.h"
#include "XmlWriter.h"
#include "SyntaxWriter.h"
#include "ExpressionParser.h"
#include "CommandLexer.h"
#include "TWare.h"
#include "TLaser.h"
#include "StringParser.h"
#include "../Testing/ScriptTextValidator.h"

namespace Logic
{
   // -------------------------------- CONSTRUCTION --------------------------------

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   
   //ScriptParser::ScriptTree  DebugTests::CompileScript(const vector<wstring>& lines)
   //{
   //   Console << Cons::Heading << L"Compiling current script text: " << ENDL;

   //   ScriptParser p(lines, GameVersion::TerranConflict);
   //   return p.Script;
   //}


   //ScriptFile  DebugTests::LoadScript(const WCHAR*  path)
   //{
   //   XFileSystem vfs;

   //   // Build VFS. Enumerate language files
   //   vfs.Enumerate(L"D:\\X3 Albion Prelude", GameVersion::TerranConflict, nullptr);
   //   StringLib.Enumerate(vfs, GameLanguage::English, nullptr);
   //   SyntaxLib.Enumerate(nullptr);

   //   // Parse script
   //   Console << Cons::Heading << L"Parsing MSCI script: " << path << ENDL;

   //   StreamPtr fs2( new FileStream(path, FileMode::OpenExisting, FileAccess::Read) );
   //   ScriptFile script = ScriptFileReader(fs2).ReadFile();

   //   Console << Colour::Green << L"Script loaded successfully" << ENDL;
   //   return script;
   //}


   void  DebugTests::RunAll()
   {
      //Test_LanguageFileReader();
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
      Test_BatchScriptCompile();
   }

	// ------------------------------ PROTECTED METHODS -----------------------------

	// ------------------------------- PRIVATE METHODS ------------------------------
   
   void DebugTests::Test_BatchScriptCompile()
   {
      XFileSystem vfs;

      // Browse scripts in VFS
      vfs.Enumerate(L"D:\\X3 Albion Prelude", GameVersion::TerranConflict);

      // Feedback
      Console << Cons::Heading << L"Performing MSCI script batch test: " << ENDL;

      // Browse scripts
      for (auto& f : vfs.Browse(XFolder::Scripts))
      {
         // Ensure PCK/XML
         if (!f.FullPath.HasExtension(L".pck") && !f.FullPath.HasExtension(L".xml"))
            continue;

         // Check Skip list
         if (f.FullPath.FileName == L"!lib.war.races.pck")     // Missing 'end' from conditional
            continue;

         // Validate
         ScriptTextValidator script(f.FullPath);
         if (!script.Validate())
            break;
      }
   }


   void DebugTests::Test_CommandSyntax()
   {
      const WCHAR* path = L"D:\\My Projects\\MFC Test 1\\MFC Test 1\\plugin.piracy.enslavepassengers.xml"; 

      try
      {
         //// Test LegacySyntaxReader
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
               Console << Colour::Green << "Success" << ENDL;
            }
            catch (ExpressionParserException& e)
            {
               Console << Colour::Red << "Failed: " << e.Message << L" " << (lex.Valid(e.Position) ? e.Position->Text : L"") << ENDL;
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

   void  DebugTests::Test_Iterator()
   {
      const WCHAR* path = L"D:\\My Projects\\MFC Test 1\\MFC Test 1\\testfile.xml"; 
   
      try
      {
         // Read file
         StreamPtr fs( new FileStream(path, FileMode::OpenExisting, FileAccess::Read) );
         auto langFile = LanguageFileReader(fs).ReadFile(Path(path).GetFileName());

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
         auto langFile = LanguageFileReader(fs).ReadFile(Path(path).GetFileName());
      }
      catch (ExceptionBase&  e)
      {
         CString sz;
         sz.Format(L"Unable to load '%s' : %s\n\n" L"Source: %s()", path, e.Message.c_str(), e.Source.c_str());
         AfxMessageBox(sz);
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

         Console << ENDL << "Matching '" << Colour::Yellow << txt2 << Colour::White << "' against " << Colour::Yellow << expr << ENDL;

         // Iterator
         const wregex reg2(expr, wregex::ECMAScript); 
         for (wsregex_iterator it(txt2.cbegin(), txt2.cend(), reg2), eof; it != eof; ++it)
            Console << "Matched: " << Colour::Green << it->str().c_str() << ENDL;
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
            Console << Colour::Yellow << obj.id << Colour::White << L" : ";
            Console << (obj.name.Exists()?Colour::Green:Colour::Red) << (obj.name.Exists()?obj.name.Text:L"Missing") << ENDL;
         }*/

         // Read TLaser
         path = L"D:\\My Projects\\BearScript\\Data\\Relevant Files\\TLaser.txt";
         Console << Cons::Heading << L"Reading TFile: " << path << ENDL;
         StreamPtr fs2( new FileStream(path, FileMode::OpenExisting, FileAccess::Read) );
         auto file2 = TLaserReader(fs2).ReadFile(MainType::Laser, GameVersion::TerranConflict);

         // Print IDs/Names
         /*for (auto& obj : file.Objects)
         {
            Console << Colour::Yellow << obj.id << Colour::White << L" : ";
            Console << (obj.name.Exists()?Colour::Green:Colour::Red) << (obj.name.Exists()?obj.name.Text:L"Missing") << ENDL;
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
   
   void DebugTests::Test_SyntaxWriter()
   {
      const WCHAR* path = L"c:\\temp\\new Syntax.xml"; 

      try
      {
         // Load legacy syntax file
         StreamPtr fs( new FileStream(L"D:\\My Projects\\MFC Test 1\\MFC Test 1\\Command Syntax.txt", FileMode::OpenExisting, FileAccess::Read) );
         auto file = LegacySyntaxReader(fs).ReadFile();

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
         // Test LegacySyntaxReader
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
         auto langFile = LanguageFileReader(fs2).ReadFile(Path(path).GetFileName());

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