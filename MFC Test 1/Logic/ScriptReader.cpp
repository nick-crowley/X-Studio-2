#include "stdafx.h"
#include "ScriptReader.h"
#include <algorithm>

namespace Library
{
   namespace IO
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Creates an MSCI script reader from an input stream</summary>
      /// <param name="src">The input stream</param>
      /// <exception cref="Logic::ArgumentException">Stream is not readable</exception>
      /// <exception cref="Logic::ArgumentNullException">Stream is null</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      ScriptReader::ScriptReader(StreamPtr in) : XmlReader(in)
      {
      }

      /// <summary>Closes the input stream</summary>
      ScriptReader::~ScriptReader()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Reads the entire language file</summary>
      /// <returns>New language file</returns>
      /// <exception cref="Logic::ComException">COM Error</exception>
      /// <exception cref="Logic::FileFormatException">Corrupt XML / Missing elements / missing attributes</exception>
      /// <exception cref="Logic::InvalidDataException">Invalid language ID</exception>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      ScriptFile ScriptReader::ReadFile()
      {
         ScriptFile file;

         try
         {
            // Parse document
            LoadDocument();

            // Get codearray node
            XML::IXMLDOMNodePtr codeArray(Document->documentElement->selectSingleNode(L"codearray"));
            if (codeArray == nullptr)
               throw FileFormatException(HERE, L"Missing codearray node");

            // Get codearray data array
            else if (ReadArray(codeArray = codeArray->childNodes->item[0]) != 10)
               throw FileFormatException(HERE, L"Invalid codearray node");
            
            // Read properties
            file.Name        = ReadString(codeArray->childNodes->item[0]);
            file.Game        = ReadEngineVersion(codeArray->childNodes->item[1]);
            file.Description = ReadString(codeArray->childNodes->item[2]);
            file.Version     = (UINT)ReadInt(codeArray->childNodes->item[3]);
            // TODO: Live data flag

            // Arguments/Variables
            ReadVariables(file, codeArray->childNodes->item[5], codeArray->childNodes->item[7]);

            // Commands
            ReadCommands(file, codeArray->childNodes->item[6], codeArray->childNodes->item[8]);

            // TODO: Command name
            return file;
         }
         catch (_com_error& ex) {
            throw ComException(HERE, ex);
         }
      }

		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

      /// <summary>Gets an appropriate reader for a given command</summary>
      /// <param name="script">The script.</param>
      /// <param name="type">The type of command</param>
      /// <param name="cmdBranch">The branch containing the command</param>
      /// <returns>Varies</returns>
      ScriptReader::ReaderPtr  ScriptReader::GetCommandReader(ScriptFile& script, CommandType type, XmlNodePtr& cmdBranch)
      {
         // Verify node is an array
         int size = ReadArray(cmdBranch);

         // Examine command ID (from first/second node)
         int id = ReadInt(cmdBranch->childNodes->item[type == CommandType::Standard ? 0 : 1]);

         // Create appropriate reader
         switch (id)
         {
         // Simple Auxiliary: 
         case CMD_ELSE:
         case CMD_BREAK:
         case CMD_CONTINUE:
         case CMD_END:
         case CMD_NOP:
         case CMD_COMMENT:              return ReaderPtr(new AuxiliaryCommandReader(*this, script, cmdBranch));

         case CMD_COMMAND_COMMENT:      return ReaderPtr(new CommentedCommandReader(*this, script, cmdBranch));
         case CMD_CALL_SCRIPT_VAR_ARGS: return ReaderPtr(new ScriptCallCommandReader(*this, script, cmdBranch));
         case CMD_EXPRESSION:           return ReaderPtr(new ExpressionCommandReader(*this, script, cmdBranch));

         default:                       return ReaderPtr(new StandardCommandReader(*this, script, cmdBranch));
         }
      }

      /// <summary>Reads the array size from an sval node</summary>
      /// <param name="node">The node</param>
      /// <returns>Size of the array</returns>
      int  ScriptReader::ReadArray(XmlNodePtr& node)
      {
         // Ensure node is script value
         ReadElement(node, L"sval");

         // Array: read size
         if (ReadAttribute(node, L"type") == L"array")
            return _wtoi(ReadAttribute(node, L"size").c_str());

         // Int: Permit iff zero
         else if (ReadAttribute(node, L"type") == L"int" && ReadAttribute(node, L"val") == L"0")
            return 0;

         // Error
         throw FileFormatException(HERE, L"Cannot read <sval> array node");
      }

      
      /// <summary>Reads both the standard and auxiliary commands, then interlaces them in the correct order</summary>
      /// <param name="script">The script.</param>
      /// <param name="stdBranch">The standard commands branch.</param>
      /// <param name="auxBranch">The auxiliary commands branch.</param>
      void  ScriptReader::ReadCommands(ScriptFile&  script, XmlNodePtr& stdBranch, XmlNodePtr& auxBranch)
      {
         vector<ScriptCommand>  std;
         list<ScriptCommand>    aux; 

         // Read standard commands
         for (int i = 0; i < stdBranch->childNodes->length; i++)
            std.push_back( GetCommandReader(script, CommandType::Standard, stdBranch->childNodes->item[i])->ReadCommand() );

         // Read auxiliary commands
         for (int i = 0; i < auxBranch->childNodes->length; i++)
            aux.push_back( GetCommandReader(script, CommandType::Auxiliary, auxBranch->childNodes->item[i])->ReadCommand() );

         // Interlace in correct order
         for (UINT i = 0; i < std.size(); i++)
         {
            // Insert any/all auxiliary commands preceeding next standard command
            while (!aux.empty() && aux.front().RefIndex <= i)
            {
               script.Commands.push_back( aux.front() );
               aux.pop_front();
            }

            // Insert standard command
            script.Commands.push_back( std[i] );
         }

         // Translate parameters
         for (ScriptCommand& cmd : script.Commands)
            cmd.Translate(script);
      }


      /// <summary>Reads the engine version</summary>
      /// <param name="node">The sval node.</param>
      /// <returns>Equivilent game version</returns>
      GameVersion ScriptReader::ReadEngineVersion(XmlNodePtr& node)
      {
         int val = ReadInt(node);

         // TODO: Parse engine version
         return GameVersion::TerranConflict;
      }
      
      /// <summary>Reads an integer value from a sval node</summary>
      /// <param name="node">The sval node</param>
      /// <returns>Integer value</returns>
      int  ScriptReader::ReadInt(XmlNodePtr& node)
      {
         // Ensure node is script value
         ReadElement(node, L"sval");

         // Ensure type is int
         if (ReadAttribute(node, L"type") != L"int")
            throw FileFormatException(HERE, L"Cannot read <sval> integer node");

         // Read value
         return _wtoi(ReadAttribute(node, L"val").c_str());
      }
      
      /// <summary>Reads the string value from a sval node</summary>
      /// <param name="node">The node.</param>
      /// <returns>String value</returns>
      wstring  ScriptReader::ReadString(XmlNodePtr& node)
      {
         // Ensure node is script value
         ReadElement(node, L"sval");

         // Ensure type is string
         if (ReadAttribute(node, L"type") != L"string")
            throw FileFormatException(HERE, L"Cannot read <sval> string node");

         // Read value
         return ReadAttribute(node, L"val");
      }

      /// <summary>Reads string or int value from a script value node</summary>
      /// <param name="node">The node.</param>
      /// <returns>String/Int value</returns>
      ParameterValue  ScriptReader::ReadValue(XmlNodePtr& node)
      {
         // Ensure node is script value
         ReadElement(node, L"sval");
         wstring type = ReadAttribute(node, L"type");

         // String: return as string
         if (type == L"string")
            return ReadAttribute(node, L"val");

         // Int: return as int
         else if (type == L"int")
            return _wtoi(ReadAttribute(node, L"val").c_str());

         // Unknown type
         throw FileFormatException(HERE, L"Cannot read <sval> int/string union node");
      }

      /// <summary>Reads the variables codearray branch</summary>
      /// <param name="script">The script.</param>
      /// <param name="varBranch">The variable branch.</param>
      /// <param name="argBranch">The argument branch.</param>
      void  ScriptReader::ReadVariables(ScriptFile&  script, XmlNodePtr& varBranch, XmlNodePtr& argBranch)
      {
         wstring name;

         // Init
         script.Variables.clear();

         // Iterate thru variables branch
         for (int i = 0; i < varBranch->childNodes->length; i++)
         {
            // Generate new ScriptVariable from {name,id} pair
            name = ReadString(varBranch->childNodes->item[i]);
            script.Variables.push_back( ScriptVariable(name, i) );
         }
      }

   }
}