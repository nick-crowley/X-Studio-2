#include "stdafx.h"
#include "DescriptionFileReader.h"

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

            // Return file
            return file;
         }
         catch (_com_error& ex) {
            throw ComException(HERE, ex);
         }
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
   
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
   }
}
