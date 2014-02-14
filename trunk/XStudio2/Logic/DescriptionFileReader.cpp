#include "stdafx.h"
#include "DescriptionFileReader.h"
#include <regex>
#include <strsafe.h>
#include "StringResolver.h"

namespace Logic
{
   namespace IO
   {
   
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

            // DEBUG: Parse constants
            for (auto& c : file.Constants)
            {
               try
               {
                  ConstantDescription& d = c.second;
                  
                  //if (d.Page == 2006 && d.ID == 2020)
                  {
                     Console << "Parsing constant: page=" << d.Page << " id=" << d.ID << " txt='" << Colour::Cyan << d.Text << "'" << ENDL;
                     Console << ENDL << ENDL;

                     d.Text = Parse(d.Text);

                     Console << ENDL << Colour::Green << "Success: " << Colour::Yellow << d.Text << ENDL;
                     Console << ENDL << ENDL << ENDL << ENDL;
                  }
               }
               catch (ExceptionBase& e) {
                  Console.Log(HERE, e);
               }
            }

            // Return file
            return file;
         }
         catch (_com_error& ex) {
            throw ComException(HERE, ex);
         }
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   
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


      /// <summary>Called for each occurrence of parameterized macros</summary>
      /// <param name="match">The match.</param>
      /// <returns>Replacement text</returns>
      wstring  DescriptionFileReader::onMatchMacro(wsmatch& match)
      {
         const Macro* macro;
         wstring name      = match[1].str(),
                 arguments = match[2].str();

         // DEBUG:
         //Console << Colour::Cyan << "Matched Macro: " << Colour::White << match[0].str() << ENDL;

         // Lookup macro 
         if (Macros.TryFind(name, macro))
         {
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

            // Recursively parse
            return Parse(FormatBuffer.get());
         }

         // Failed: Return verbatim
         return match[0].str();
      }

      /// <summary>Called for each occurrence of parameterless macros</summary>
      /// <param name="match">The match.</param>
      /// <returns>Replacement text</returns>
      wstring  DescriptionFileReader::onMatchKeyword(wsmatch& match)
      {
         const Macro* m;
         wstring name = (match[1].matched ? match[1].str() : match[2].str()); // Retrieve keyword/macro
         
         // DEBUG:
         //Console << Colour::Cyan << "Matched Keyword: " << Colour::White << match[0].str() << ENDL;

         // Lookup macro + recursively parse
         if (Macros.TryFind(name, m))
            return m->Recursive ? Parse(m->Text) : m->Text;

         // Failed: Return match
         return match[0].str();
      }

      /// <summary>Matches a capitalized keyword or a macro with no parameters, in both instances the name is captured</summary>
      wregex  DescriptionFileReader::MatchKeyword(L"\\b([A-Z_0-9]+)(?![:\\}])\\b" L"|" L"\\{([A-Z_0-9]+)\\}");

      /// <summary>Matches a macro with one or more parameters. captures the name alone and parameters as a block</summary>
      wregex  DescriptionFileReader::MatchMacro(L"\\{([A-Z_0-9]+):([^\\}]+)\\}");

      /// <summary>Matches each parameter in a comma delimited string</summary>
      wregex  DescriptionFileReader::MatchParameters(L"([^,]+)((?=,)[^,]+)*");

      // ------------------------------- PRIVATE METHODS ------------------------------

      /// <summary>Parses the text.</summary>
      wstring  DescriptionFileReader::Parse(wstring text)
      {
         UINT Position;
         wsmatch match;
         wstring r;
         
         try
         {
            // DEBUG:
            //Console << Colour::Cyan << "Parsing: " << Colour::White << text << ENDL;

            // Replace parameterized macros.   
            for (Position = 0; regex_search(text.cbegin()+Position, text.cend(), match, MatchMacro); ) //  Manually track position for in-place replacement + avoid infinite loop
            {
               r = onMatchMacro(match);
               Console << "  Replace Macro: " << Colour::Yellow << match[0].str() << Colour::White << " with " << Colour::Green << r << ENDL;

               // Advance position to beyond inserted text, and insert text
               Position = (match[0].first - text.cbegin()) + r.length();
               text.replace(match[0].first, match[0].second, r);
            }

            // Replace keywords
            for (Position = 0; regex_search(text.cbegin()+Position, text.cend(), match, MatchKeyword); )  
            {
               r = onMatchKeyword(match);
               Console << "  Replace Keyword: " << Colour::Yellow << match[0].str() << Colour::White << " with " << Colour::Green << r << ENDL;

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

   }
}
