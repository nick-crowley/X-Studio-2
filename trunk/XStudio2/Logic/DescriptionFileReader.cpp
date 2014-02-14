#include "stdafx.h"
#include "DescriptionFileReader.h"
#include <regex>
#include <strsafe.h>
#include "StringResolver.h"

/// <summary>Turn console debugging output on/off</summary>
//#define PRINT_CONSOLE

namespace Logic
{
   namespace IO
   {
   
      /// <summary>Matches a capitalized keyword or a macro with no parameters, in both instances the name is captured</summary>
      const wregex  DescriptionFileReader::MatchKeyword(L"\\b([A-Z_0-9]+)(?![:\\}])\\b" L"|" L"\\{([A-Z_0-9]+)\\}");

      /// <summary>Matches a macro with one or more parameters. captures the name alone and parameters as a block</summary>
      const wregex  DescriptionFileReader::MatchMacro(L"\\{([A-Z_0-9]+):([^\\}]+)\\}");

      /// <summary>Matches each parameter in a comma delimited string</summary>
      const wregex  DescriptionFileReader::MatchParameters(L"([^,]+)((?=,)[^,]+)*");

      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Creates a description xml file reader from an input stream</summary>
      /// <param name="src">The input stream</param>
      /// <exception cref="Logic::ArgumentException">Stream is not readable</exception>
      /// <exception cref="Logic::ArgumentNullException">Stream is null</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      DescriptionFileReader::DescriptionFileReader(StreamPtr in) : XmlReader(in)
      {
         FormatBuffer = CharArrayPtr(new wchar[BUFFER_LENGTH]);
         FormatBuffer.get()[0] = L'\0';
      }


      DescriptionFileReader::~DescriptionFileReader()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Reads the entire script file</summary>
      /// <param name="path">Full file path</param>
      /// <param name="justProperties">True for properties only, False for commands</param>
      /// <param name="dropJMPs">True for remove JMP commands, False to retain</param>
      /// <returns>New script file</returns>
      /// <exception cref="Logic::ArgumentNullException">Missing node</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      /// <exception cref="Logic::FileFormatException">Corrupt XML / Missing elements / missing attributes</exception>
      /// <exception cref="Logic::InvalidValueException">Invalid script command</exception>
      /// <exception cref="Logic::InvalidOperationException">Invalid goto/gosub command</exception>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      DescriptionFile  DescriptionFileReader::ReadFile()
      {
         try
         {
            DescriptionFile file;

            // Read file
            LoadDocument();

            // Get root 
            Root = Document->documentElement;
            ReadElement(Root, L"descriptions");

            // TODO: Read file properties

            // Read Macros
            ReadMacros();

            // Read Commands/Constants
            ReadCommands(file);
            ReadConstants(file);

            // Parse constants
            for (auto& c : file.Constants)
            {
               try
               {
                  ConstantDescription& d = c.second;
                  d.Text = Parse(d.Text);
               }
               catch (ExceptionBase& e) {
                  Console.Log(HERE, e, GuiString(L"Unable to parse constant description: '%s'", c.second.Text.c_str()));
               }
            }

            // Parse commands
            for (auto& c : file.Commands)
            {
               try
               {
                  CommandDescription& d = c.second;
                  d.Text = Parse(d.Text);
               }
               catch (ExceptionBase& e) {
                  Console.Log(HERE, e, GuiString(L"Unable to parse command description: '%s'", c.second.Text.c_str()));
               }
            }

            //for (auto& c : file.Constants)
            //{
            //   try
            //   {
            //      ConstantDescription& d = c.second;
            //      
            //      //if (d.Page == 2006 && d.ID == 2020)
            //      {
            //         Console << "Parsing constant: page=" << d.Page << " id=" << d.ID << " txt='" << Colour::Cyan << d.Text << "'" << ENDL;
            //         Console << ENDL << ENDL;

            //         d.Text = Parse(d.Text);

            //         Console << ENDL << Colour::Green << "Success: " << Colour::Yellow << d.Text << ENDL;
            //         Console << ENDL << ENDL << ENDL << ENDL;
            //      }
            //   }
            //   catch (ExceptionBase& e) {
            //      Console.Log(HERE, e);
            //   }
            //}

            // Return file
            return file;
         }
         catch (_com_error& ex) {
            throw ComException(HERE, ex);
         }
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   
      /// <summary>Called for each occurrence of parameterized macros</summary>
      /// <param name="match">The match.</param>
      /// <param name="depth">Debugging output depth</param>
      /// <returns>Replacement text</returns>
      wstring  DescriptionFileReader::onMatchMacro(const wsmatch& match, int depth) const 
      {
         const Macro* macro;
         wstring name      = match[1].str(),
                 arguments = match[2].str();

         // Lookup macro 
         if (Macros.TryFind(name, macro))
         {
            // HEADING: Assume comments are part of text, not comma delimited arguments
            if (name == L"HEADING")
               StringCchPrintf(FormatBuffer.get(), BUFFER_LENGTH, macro->Text.c_str(), arguments.c_str());
            else
            {
               // MACRO: Extract arguments and format them into the replacement text
               vector<wstring> arg = { L"", L"", L"", L"", L"", L"" };
               UINT i = 0;

               // Separate parameters into array
               for (wsregex_iterator m(arguments.begin(), arguments.end(), MatchParameters), end; m != end && i <= 5; ++m)
                  arg[i++] = (*m)[0].str();

               // Verify argument count
               if (macro->ParamCount != i)
                  throw FileFormatException(HERE, GuiString(L"The macro '%s' requires %d parameters : '%s'", macro->Name.c_str(), macro->ParamCount, match[0].str().c_str()));

               // Format macro with up to six parameters...
               StringCchPrintf(FormatBuffer.get(), BUFFER_LENGTH, 
                               macro->Text.c_str(), 
                               arg[0].c_str(), arg[1].c_str(), arg[2].c_str(), arg[3].c_str(), arg[4].c_str(), arg[5].c_str());
            }

#ifdef PRINT_CONSOLE
            Console << Colour::Cyan << Indent(depth) << "Matched Macro: " << Colour::Yellow << match[0].str() 
                    << Colour::White << " with " << Colour::Yellow << FormatBuffer.get() << ENDL;
#endif
            // Recursively parse
            return Parse(FormatBuffer.get(), depth+1);
         }

#ifdef PRINT_CONSOLE
         Console << Colour::Red << Indent(depth) << "Ignored Macro: " << Colour::White << match[0].str() << ENDL;
#endif

         // Failed: Return verbatim
         return match[0].str();
      }

      /// <summary>Called for each occurrence of parameterless macros</summary>
      /// <param name="match">The match.</param>
      /// <param name="depth">Debugging output depth</param>
      /// <returns>Replacement text</returns>
      wstring  DescriptionFileReader::onMatchKeyword(const wsmatch& match, int depth) const 
      {
         const Macro* m;
         wstring name = (match[1].matched ? match[1].str() : match[2].str()); // Retrieve keyword/macro
         
         // Lookup macro + recursively parse
         if (Macros.TryFind(name, m))
         {
#ifdef PRINT_CONSOLE
            Console << Colour::Cyan << Indent(depth) << "Matched Keyword: " << Colour::Yellow << match[0].str()
                    << Colour::White << " with " << Colour::Yellow << m->Text << ENDL;
#endif

            return m->Recursive ? Parse(m->Text, depth+1) : m->Text;
         }

#ifdef PRINT_CONSOLE
         Console << Colour::Red << Indent(depth) << "Ignored Keyword: " << Colour::White << match[0].str() << ENDL;
#endif

         // Failed: Return match
         return match[0].str();
      }


      /// <summary>Parses all the keywords/macros in a string and replaces them recursively</summary>
      /// <param name="text">Source text to parse</param>
      /// <param name="depth">Debugging output depth</param>
      /// <returns>Fully parsed text</returns>
      wstring  DescriptionFileReader::Parse(wstring text, int depth) const 
      {
         UINT Position;
         wsmatch match;
         wstring r;
         
         try
         {
#ifdef PRINT_CONSOLE
            Console << Colour::Cyan << Indent(depth) << "Parsing: " << Colour::White << text << ENDL;
#endif

            // Find/Replace all macros:  {AAAA:bbb}, {AAAA:bbb,ccc}, {AAAA:bbb,ccc,ddd} ...
            for (Position = 0; regex_search(text.cbegin()+Position, text.cend(), match, MatchMacro); ) //  Manually track position for in-place replacement + avoid infinite loop
            {
               // Recursively generate replacement text
               r = onMatchMacro(match, depth);

#ifdef PRINT_CONSOLE
               Console << Indent(depth) << "Replacing text: " << Colour::Yellow << match[0].str() << Colour::White << " with " << Colour::Green << r << ENDL;
#endif

               // Advance position to beyond inserted text, and insert text
               Position = (match[0].first - text.cbegin()) + r.length();
               text.replace(match[0].first, match[0].second, r);
            }

            // Find/Replace all keywords:  {AAAAA}, AAAAA
            for (Position = 0; regex_search(text.cbegin()+Position, text.cend(), match, MatchKeyword); )  
            {
               // Recursively generate replacement text
               r = onMatchKeyword(match, depth);

#ifdef PRINT_CONSOLE
               Console << Indent(depth) << "Replacing text: " << Colour::Yellow << match[0].str() << Colour::White << " with " << Colour::Green << r << ENDL;
#endif

               // Advance position to beyond inserted text, and insert text
               Position = (match[0].first - text.cbegin()) + r.length();
               text.replace(match[0].first, match[0].second, r);
            }

            return text;
         }
         catch (regex_error& e) {
            throw RegularExpressionException(HERE, e);
         }
      }


      /// <summary>Reads a command description</summary>
      /// <param name="n">command node</param>
      /// <returns></returns>
      CommandDescription  DescriptionFileReader::ReadCommand(XmlNodePtr n)
      {
         wstring tmp;
         UINT id;
         GameVersion ver;

         // Verify tag
         ReadElement(n, L"command");

         // Read id
         id = _ttoi(ReadAttribute(n, L"id").c_str());

         // Read version
         tmp = ReadAttribute(n, L"version");
         if (tmp == L"X2")
            ver = GameVersion::Threat;
         else if (tmp == L"X3R")
            ver = GameVersion::Reunion;
         else if (tmp == L"X3TC")
            ver = GameVersion::TerranConflict;
         else if (tmp == L"X3AP")
            ver = GameVersion::AlbionPrelude;
         else
            throw InvalidValueException(HERE, GuiString(L"Unrecognised version '%s'", tmp.c_str()));

         // Create command description
         return CommandDescription(id, ver, (wchar*)n->text);  
      }


      /// <summary>Reads a script object description</summary>
      /// <param name="n">constant node</param>
      /// <returns></returns>
      ConstantDescription  DescriptionFileReader::ReadConstant(XmlNodePtr n)
      {
         KnownPage page;
         UINT      id;

         // Verify tag
         ReadElement(n, L"constant");

         // Read page/id
         id = _ttoi(ReadAttribute(n, L"id").c_str());
         page = (KnownPage)_ttoi(ReadAttribute(n, L"page").c_str());

         // Create script object description
         return ConstantDescription(page, id, (wchar*)n->text);  
      }

      
      /// <summary>Reads a macro element</summary>
      /// <param name="n">Macro node</param>
      /// <returns></returns>
      DescriptionFileReader::Macro  DescriptionFileReader::ReadMacro(XmlNodePtr n)
      {
         wstring name, txt, tmp;
         UINT params = 0;
         bool recurse = true;

         // Verify tag
         ReadElement(n, L"macro");

         // Read name/text
         name = ReadAttribute(n, L"id");
         txt  = (wchar*)n->text;

         // Read formatting parameter count
         if (TryReadAttribute(n, L"parameters", tmp))
            params = _ttoi(tmp.c_str());
         // Read recursive flag
         if (TryReadAttribute(n, L"recursive", tmp))
            recurse = (tmp != L"0");

         // Create macro
         return Macro(name, txt, params, recurse);  
      }

      
      /// <summary>Reads the command descriptions</summary>
      /// <param name="file">description file</param>
      void  DescriptionFileReader::ReadCommands(DescriptionFile& file)
      {
         // Find macros node
         auto commandsNode = Root->selectSingleNode(L"commands");
         if (commandsNode == nullptr)
            throw FileFormatException(HERE, L"Missing 'commands' node");

         // Read macros
         for (int i = 0; i < commandsNode->childNodes->length; i++)
         {
            XmlNodePtr n = commandsNode->childNodes->item[i];

            // Read all elements
            if (n->nodeType == XML::NODE_ELEMENT)
               file.Commands.Add( ReadCommand(n) );
         }
      }


      /// <summary>Reads the script object descriptions</summary>
      /// <param name="file">description file</param>
      void  DescriptionFileReader::ReadConstants(DescriptionFile& file)
      {
         // Find macros node
         auto constantsNode = Root->selectSingleNode(L"constants");
         if (constantsNode == nullptr)
            throw FileFormatException(HERE, L"Missing 'constants' node");

         // Read macros
         for (int i = 0; i < constantsNode->childNodes->length; i++)
         {
            XmlNodePtr n = constantsNode->childNodes->item[i];

            // Read all elements
            if (n->nodeType == XML::NODE_ELEMENT)
               file.Constants.Add( ReadConstant(n) );
         }
      }


      /// <summary>Reads the macros.</summary>
      void  DescriptionFileReader::ReadMacros()
      {
         // Find macros node
         auto macrosNode = Root->selectSingleNode(L"macros");
         if (macrosNode == nullptr)
            throw FileFormatException(HERE, L"Missing 'macros' node");

         // Read macros
         for (int i = 0; i < macrosNode->childNodes->length; i++)
         {
            XmlNodePtr n = macrosNode->childNodes->item[i];

            // Read all elements
            if (n->nodeType == XML::NODE_ELEMENT)
               Macros.Add( ReadMacro(n) );
         }
      }

   }
}
