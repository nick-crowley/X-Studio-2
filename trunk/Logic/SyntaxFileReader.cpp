#include "stdafx.h"
#include "SyntaxFileReader.h"

namespace Logic
{
   namespace IO
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Creates an xml reader from an input stream</summary>
      /// <param name="src">The input stream</param>
      /// <exception cref="Logic::ArgumentException">Stream is not readable</exception>
      /// <exception cref="Logic::ArgumentNullException">Stream is null</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      SyntaxFileReader::SyntaxFileReader(StreamPtr in) : XmlReader(in)
      {
      }

      SyntaxFileReader::~SyntaxFileReader()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      /// <summary>Parses the command type</summary>
      /// <param name="t">command type</param>
      /// <returns>command type</returns>
      /// <exception cref="Logic::InvalidValueException">Invalid command type</exception>
      CommandType  SyntaxFileReader::ParseCommandType(const GuiString&  t)
      {
         if (t.Equals(L"standard"))
            return CommandType::Standard;

         else if (t.Equals(L"auxiliary"))
            return CommandType::Auxiliary;

         else if (t.Equals(L"macro"))
            return CommandType::Macro;

         // Error
         throw InvalidValueException(HERE, VString(L"Invalid command type '%s'", t.c_str()) );
      }
      
      /// <summary>Parses the execution type</summary>
      /// <param name="t">execution type</param>
      /// <returns>execution  type</returns>
      /// <exception cref="Logic::InvalidValueException">Invalid execution type</exception>
      ExecutionType  SyntaxFileReader::ParseExecutionType(const GuiString&  t)
      {
         if (t.Equals(L"concurrent"))
            return ExecutionType::Concurrent;

         else if (t.Equals(L"serial"))
            return ExecutionType::Serial;

         else if (t.Equals(L"either"))
            return ExecutionType::Either;

         // Error
         throw InvalidValueException(HERE, VString(L"Invalid execution type '%s'", t.c_str()) );
      }
      
      /// <summary>Parses the usage type</summary>
      /// <param name="t">usage type</param>
      /// <returns>usage type</returns>
      /// <exception cref="Logic::InvalidValueException">Invalid usage type</exception>
      ParameterUsage  SyntaxFileReader::ParseParameterUsage(const GuiString&  t)
      {
         if (t.Equals(L"none"))
            return ParameterUsage::None;

         else if (t.Equals(L"PageID"))
            return ParameterUsage::PageID;

         else if (t.Equals(L"ScriptName"))
            return ParameterUsage::ScriptName;

         else if (t.Equals(L"StringID"))
            return ParameterUsage::StringID;

         // Error
         throw InvalidValueException(HERE, VString(L"Invalid parameter usage '%s'", t.c_str()) );
      }
      
      
      /// <summary>Parses the varg method</summary>
      /// <param name="t">varg method</param>
      /// <returns>varg method</returns>
      /// <exception cref="Logic::InvalidValueException">Invalid varg method</exception>
      VArgMethod  SyntaxFileReader::ParseVArgMethod(const GuiString&  t)
      {
         if (t.Equals(L"None"))
            return VArgMethod::None;

         else if (t.Equals(L"Drop"))
            return VArgMethod::Drop;

         else if (t.Equals(L"PadNull"))
            return VArgMethod::PadNull;

         // Error
         throw InvalidValueException(HERE, VString(L"Invalid varg method '%s'", t.c_str()) );
      }
      
      
      /// <summary>Parses the varg syntax type</summary>
      /// <param name="t">varg syntax</param>
      /// <returns>varg syntax</returns>
      /// <exception cref="Logic::InvalidValueException">Invalid varg syntax</exception>
      VArgSyntax  SyntaxFileReader::ParseVArgSyntax(const GuiString&  t)
      {
         if (t.Equals(L"None"))
            return VArgSyntax::None;

         else if (t.Equals(L"CommaDelimited"))
            return VArgSyntax::CommaDelimited;

         else if (t.Equals(L"NamedPairs"))
            return VArgSyntax::NamedPairs;

         // Error
         throw InvalidValueException(HERE, VString(L"Invalid varg syntax '%s'", t.c_str()) );
      }

      /// <summary>Parses the version flags</summary>
      /// <param name="t">comma delimited versions</param>
      /// <returns>Compatibility versions</returns>
      UINT  SyntaxFileReader::ParseVersions(const GuiString&  t)
      {
         UINT vers = 0;

         // Read versions
         if (t.Contains(L"X2", false))
            vers |= (UINT)GameVersion::Threat;
         if (t.Contains(L"X3R", false))
            vers |= (UINT)GameVersion::Reunion;
         if (t.Contains(L"X3TC", false))
            vers |= (UINT)GameVersion::TerranConflict;
         if (t.Contains(L"X3AP", false))
            vers |= (UINT)GameVersion::AlbionPrelude;

         return vers;
      }
      
		// ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Reads the entire Syntax file</summary>
      /// <returns>New Syntax file</returns>
      /// <exception cref="Logic::ComException">COM Error</exception>
      /// <exception cref="Logic::FileFormatException">Corrupt XML / Missing elements / missing attributes</exception>
      /// <exception cref="Logic::InvalidValueException">Invalid command type, version, exec type, varg type, parameter usage etc.</exception>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      SyntaxFile SyntaxFileReader::ReadFile()
      {
         try
         {
            // Parse document
            LoadDocument();
            XmlNodePtr root(Document->documentElement);

            // Read title/version
            SyntaxFile file(ReadAttribute(root, L"title"), ReadAttribute(root, L"version"));

            
            // Read groups
            XmlNodePtr groups(root->selectSingleNode(L"groups"));
            for (UINT i = 0; i < GetChildCount(groups, L"command groups"); i++)
            {
               XmlNodePtr n = GetChild(groups, i, L"command group");

               // Skip comments
               if (n->nodeType == Xml::NODE_ELEMENT)
               {
                  GuiString id(ReadAttribute(n, L"id"));
                  file.Groups.Add((LPCWSTR)n->text, (CommandGroup)id.ToInt());
               }
            }
            
            // Read parameter types
            XmlNodePtr types(root->selectSingleNode(L"types"));
            for (UINT i = 0; i < GetChildCount(types, L"parameter types"); i++)
            {
               XmlNodePtr n = GetChild(types, i, L"parameter type");

               // Skip comments
               if (n->nodeType == Xml::NODE_ELEMENT)
               {
                  GuiString id(ReadAttribute(n, L"value"));
                  file.Types.Add((LPCWSTR)n->text, (ParameterType)id.ToInt());
               }
            }
            
            // Read commands
            XmlNodePtr commands(root->selectSingleNode(L"commands"));
            for (UINT i = 0; i < GetChildCount(types, L"commands"); i++)
            {
               XmlNodePtr n = GetChild(commands, i, L"commands");

               // Skip comments
               if (n->nodeType == Xml::NODE_ELEMENT)
                  file.Commands.Add( ReadCommand(file, n) );
            }

            return file;
         }
         catch (_com_error& ex) {
            throw ComException(HERE, ex);
         }
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

      /// <summary>Reads a command node</summary>
      /// <param name="f">syntax file</param>
      /// <param name="node">command element</param>
      /// <returns>CommandSyntax</returns>
      /// <exception cref="Logic::FileFormatException">Missing element or attributes</exception>
      /// <exception cref="Logic::InvalidValueException">Invalid command type, version, exec type, varg type, parameter usage etc.</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      CommandSyntax  SyntaxFileReader::ReadCommand(SyntaxFile& f, XmlNodePtr& node)
      {
         CommandSyntax::Declaration  d;

         // Verify tag
         ReadElement(node, L"command");

         // Read properties
         d.ID = GuiString(ReadAttribute(node, L"id")).ToInt();
         d.Group = f.Groups[ReadAttribute(node, L"group")];
         d.Type = ParseCommandType(ReadAttribute(node, L"type"));
         d.Versions = ParseVersions(ReadAttribute(node, L"version"));

         // Text/URL
         d.Syntax = GetChild(node, 0, L"command text")->text;
         d.URL = GetChild(node, 1, L"command MSCI URL")->text;

         // Execution
         auto exec = GetChild(node, 2, L"command execution");
         d.Execution = ParseExecutionType((LPCWSTR)exec->text);

         // Params
         auto params = GetChild(node, 3, L"command parameters");
         for (UINT i = 0; i < GetChildCount(params, L"command parameters"); ++i)
         {
            //ParameterSyntax::Declaration d;
            auto p = GetChild(params, i, L"command parameter");

            // Properties
            auto type = f.Types[(LPCWSTR)p->text];
            auto display = GuiString(ReadAttribute(p, L"displayIndex")).ToInt();

            // Optional properties
            GuiString txt;
            int ordinal = 0;
            auto usage = ParameterUsage::None;

            if (TryReadAttribute(p, L"ordinal", txt))
               ordinal = txt.ToInt();
            if (TryReadAttribute(p, L"usage", txt))
               usage = ParseParameterUsage(txt);

            // Create/append parameter
            auto param = ParameterSyntax(ParameterSyntax::Declaration(type, i, display, ordinal, usage));
            d.Params.push_back(param);
         }

         // Vargs
         auto vargs = GetChild(node, 4, L"command vargs");
         d.VArgument = ParseVArgSyntax((LPCWSTR)vargs->text);

         if (d.VArgument != VArgSyntax::None)
         {
            d.VArgCount = GuiString(ReadAttribute(vargs, L"count")).ToInt();
            d.VArgParams = ParseVArgMethod(ReadAttribute(vargs, L"encoding"));
         }

         // Return command
         return CommandSyntax(d);
      }

   }
}