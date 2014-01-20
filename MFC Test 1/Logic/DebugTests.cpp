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

namespace Logic
{
   // -------------------------------- CONSTRUCTION --------------------------------

   // ------------------------------- STATIC METHODS -------------------------------

   // ------------------------------- PUBLIC METHODS -------------------------------

   
   ScriptParser::ScriptTree  DebugTests::CompileScript(const vector<wstring>& lines)
   {
      Console << ENDL << Colour::Cyan << L"Compiling current script text: " << ENDL;

      ScriptParser p(lines, GameVersion::TerranConflict);
      return p.ParseScript();
   }


   ScriptFile  DebugTests::LoadScript(const WCHAR*  path)
   {
      const WCHAR* syntaxPath = L"D:\\My Projects\\MFC Test 1\\MFC Test 1\\Command Syntax.txt";
      XFileSystem vfs;

      // Build VFS. Enumerate language files
      vfs.Enumerate(L"D:\\X3 Albion Prelude", GameVersion::TerranConflict);
      StringLib.Enumerate(vfs, GameLanguage::English);


      // Load legacy syntax file
      Console << ENDL << Colour::Cyan << L"Reading legacy syntax file: " << syntaxPath << ENDL;

      StreamPtr fs( new FileStream(syntaxPath, FileMode::OpenExisting, FileAccess::Read) );
      SyntaxLib.Merge( LegacySyntaxReader(fs).ReadFile() );

      Console << Colour::Green << L"Legacy syntax loaded successfully" << ENDL;


      // Parse script
      Console << ENDL << Colour::Cyan << L"Parsing MSCI script: " << path << ENDL;

      StreamPtr fs2( new FileStream(path, FileMode::OpenExisting, FileAccess::Read) );
      ScriptFile script = ScriptFileReader(fs2).ReadFile();

      Console << Colour::Green << L"Script loaded successfully" << ENDL;
      return script;
   }


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
      Test_TFileReader();
   }

	// ------------------------------ PROTECTED METHODS -----------------------------

	// ------------------------------- PRIVATE METHODS ------------------------------
   
   void DebugTests::Test_CommandSyntax()
   {
      const WCHAR* path = L"D:\\My Projects\\MFC Test 1\\MFC Test 1\\Command Syntax.txt"; 
   
      try
      {
         // Test LegacySyntaxReader
         StreamPtr fs( new FileStream(path, FileMode::OpenExisting, FileAccess::Read) );
         SyntaxLib.Merge( LegacySyntaxReader(fs).ReadFile() );

         // Test Lookup
         SyntaxLib.Find(CMD_EXPRESSION, GameVersion::TerranConflict);

         // Test script reader
         path = L"D:\\My Projects\\MFC Test 1\\MFC Test 1\\plugin.piracy.enslavepassengers.xml"; 
         StreamPtr fs2( new FileStream(path, FileMode::OpenExisting, FileAccess::Read) );
         auto script = ScriptFileReader(fs2).ReadFile();
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

   TokenArray  Tokenise(const wstring expr)
   {
      TokenArray tokens;

      for (WCHAR ch : expr)
         tokens.push_back(ScriptToken(iswdigit(ch) ? TokenType::Number : TokenType::Operator, 0, 0, wstring(1, ch)));

      return tokens;
   }

   void  DebugTests::Test_ExpressionParser()
   {
      try
      {
         TokenArray tokens;
         
         // Try Parse
         tokens = Tokenise(L"4+5*3");
         ExpressionParser(tokens.begin(), tokens.end()).Parse();

         // Try Parse
         tokens = Tokenise(L"1|2^3&4<5+6*-8");
         ExpressionParser(tokens.begin(), tokens.end()).Parse();

         // Try Parse
         /*tokens = Tokenise(L"4+5*3");
         ExpressionParser(tokens.begin(), tokens.end(), TokenArray()).Parse();*/

         // Test Lexer
         const WCHAR* cmd = L"$szTemp = sprintf: fmt='iMaximumDistance = %s   iAttackTimeout = %s', $iMaximumDistance, $iAttackTimeout, null, null, null";
         CommandLexer lex(cmd);

         // Test lexer
         cmd = L"= [PLAYERSHIP]-> add $iPassengersEnslaved units of {Slaves}";
         CommandLexer lex2(cmd);

         // Test lexer
         cmd = L"* INFO: `WARNING: The %s have revoked your police license.`";
         CommandLexer lex3(cmd);
         
         // Test lexer
         cmd = L"do if $szItemChosen == 'Option.Quit' OR $szItemChosen == -1";
         CommandLexer lex4(cmd);

         // Test lexer
         cmd = L"$oWeapon = $oRequirement[2]";
         CommandLexer lex5(cmd);
         
         // Test lexer
         cmd = L"$bMatch = ($BONUS.MARINES.COUNT >= $iActorCount) AND ($BONUS.SIDEARMS.COUNT >= $iActorCount + 1)";
         CommandLexer lex6(cmd);
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
         vfs.Enumerate(L"D:\\X3 Albion Prelude", GameVersion::TerranConflict);

         // Test browse
         for (auto f : vfs.Browse(L"D:\\X3 Albion Prelude\\t"))
            OutputDebugString(GuiString(L"Found file: '%s'\n", (const WCHAR*)f.FullPath).c_str());

         // Test search
         XFileInfo f = vfs.Find(L"D:\\X3 Albion Prelude\\types\\TLaser");
         StringReader reader(f.Open());

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

   void  DebugTests::Test_TFileReader()
   {
      const WCHAR* path = L"D:\\My Projects\\BearScript\\Data\\Relevant Files\\TWareT.txt";
   
      try
      {
         // Load string library
         XFileSystem vfs;
         vfs.Enumerate(L"D:\\X3 Albion Prelude", GameVersion::TerranConflict);
         StringLib.Enumerate(vfs, GameLanguage::English);

         // Read TWareT
         Console << ENDL << Colour::Cyan << L"Reading TFile: " << path << ENDL;
         StreamPtr fs( new FileStream(path, FileMode::OpenExisting, FileAccess::Read) );
         auto file = TWareReader(fs).ReadFile();

         // Print IDs/Names
         for (auto& obj : file.Objects)
         {
            Console << Colour::Yellow << obj.id << Colour::White << L" : ";
            Console << (obj.name.Exists()?Colour::Green:Colour::Red) << (obj.name.Exists()?obj.name.Text:L"Missing") << ENDL;
         }

         // Read TLaser
         path = L"D:\\My Projects\\BearScript\\Data\\Relevant Files\\TLaser.txt";
         Console << ENDL << Colour::Cyan << L"Reading TFile: " << path << ENDL;
         StreamPtr fs2( new FileStream(path, FileMode::OpenExisting, FileAccess::Read) );
         auto file2 = TLaserReader(fs2).ReadFile();

         // Print IDs/Names
         for (auto& obj : file.Objects)
         {
            Console << Colour::Yellow << obj.id << Colour::White << L" : ";
            Console << (obj.name.Exists()?Colour::Green:Colour::Red) << (obj.name.Exists()?obj.name.Text:L"Missing") << ENDL;
         }
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
         vfs.Enumerate(L"D:\\X3 Albion Prelude", GameVersion::TerranConflict);
         StringLib.Enumerate(vfs, GameLanguage::English);

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