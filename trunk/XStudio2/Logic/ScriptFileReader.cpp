#include "stdafx.h"
#include "ScriptFileReader.h"
#include <algorithm>

namespace Logic
{
   namespace IO
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Creates an MSCI script reader from an input stream</summary>
      /// <param name="src">The input stream</param>
      /// <exception cref="Logic::ArgumentException">Stream is not readable</exception>
      /// <exception cref="Logic::ArgumentNullException">Stream is null</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      ScriptFileReader::ScriptFileReader(StreamPtr in) : XmlReader(in)
      {
      }

      /// <summary>Closes the input stream</summary>
      ScriptFileReader::~ScriptFileReader()
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
      ScriptFile ScriptFileReader::ReadFile()
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
            else if (ReadArray(codeArray = codeArray->childNodes->item[0], L"codearray branch") != 10)
               throw FileFormatException(HERE, L"Invalid codearray node");
            
            // Read properties
            file.Name        = ReadString(codeArray->childNodes->item[0], L"script name");
            file.Game        = ReadEngineVersion(codeArray->childNodes->item[1]);
            file.Description = ReadString(codeArray->childNodes->item[2], L"script description");
            file.Version     = (UINT)ReadInt(codeArray->childNodes->item[3], L"script version");
            file.LiveData    = (UINT)ReadInt(codeArray->childNodes->item[4], L"script live data flag") != 0;

            // Arguments/Variables
            ReadVariables(file, codeArray->childNodes->item[5], codeArray->childNodes->item[7]);

            // Commands
            ReadCommands(file, codeArray->childNodes->item[6], codeArray->childNodes->item[8]);

            // Command ID
            file.CommandID = ReadValue(codeArray->childNodes->item[9], L"script command ID");

            // Return file
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
      ScriptFileReader::ReaderPtr  ScriptFileReader::GetCommandReader(ScriptFile& script, CommandType type, XmlNodePtr& cmdBranch)
      {
         // Verify node is an array
         int size = ReadArray(cmdBranch, L"script command branch");

         // Examine command ID (from first/second node)
         int id = ReadInt(cmdBranch->childNodes->item[type == CommandType::Standard ? 0 : 1], L"script command ID");

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
      int  ScriptFileReader::ReadArray(XmlNodePtr& node, const WCHAR* help)
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
         throw FileFormatException(HERE, GuiString(L"Cannot read %s <sval> node", help));
      }

      
      /// <summary>Reads both the standard and auxiliary commands, then interlaces them in the correct order</summary>
      /// <param name="script">The script.</param>
      /// <param name="stdBranch">The standard commands branch.</param>
      /// <param name="auxBranch">The auxiliary commands branch.</param>
      void  ScriptFileReader::ReadCommands(ScriptFile&  script, XmlNodePtr& stdBranch, XmlNodePtr& auxBranch)
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
            if (std[i].Syntax.ID != CMD_HIDDEN_JUMP)
               script.Commands.push_back( std[i] );
         }

         // Translate all commands/parameters
         for (ScriptCommand& cmd : script.Commands)
         {
            // Replace label number parameters with label name params
            if (cmd.Syntax.ID == CMD_GOTO_LABEL || cmd.Syntax.ID == CMD_GOTO_SUB)
            {
               ScriptCommand& label = std[cmd.Parameters[0].Value.Int];
               cmd.Parameters.clear();
               cmd.Parameters.push_back(label.Parameters[0]);
            }

            // Translate
            cmd.Translate(script);
         }
      }


      /// <summary>Reads the engine version</summary>
      /// <param name="node">The sval node.</param>
      /// <returns>Equivilent game version</returns>
      GameVersion ScriptFileReader::ReadEngineVersion(XmlNodePtr& node)
      {
         int val = ReadInt(node, L"script engine version");

         // TODO: Parse engine version
         return GameVersion::TerranConflict;
      }
      
      /// <summary>Reads an integer value from a sval node</summary>
      /// <param name="node">The sval node</param>
      /// <returns>Integer value</returns>
      int  ScriptFileReader::ReadInt(XmlNodePtr& node, const WCHAR* help)
      {
         // Ensure node is script value
         ReadElement(node, L"sval");

         // Ensure type is int
         if (ReadAttribute(node, L"type") != L"int")
            throw FileFormatException(HERE, GuiString(L"Cannot read %s <sval> node", help));

         // Read value
         return _wtoi(ReadAttribute(node, L"val").c_str());
      }
      
      /// <summary>Reads the string value from a sval node</summary>
      /// <param name="node">The node.</param>
      /// <returns>String value</returns>
      wstring  ScriptFileReader::ReadString(XmlNodePtr& node, const WCHAR* help)
      {
         // Ensure node is script value
         ReadElement(node, L"sval");

         // Ensure type is string
         if (ReadAttribute(node, L"type") != L"string")
            throw FileFormatException(HERE, GuiString(L"Cannot read %s <sval> node", help));

         // Read value
         return ReadAttribute(node, L"val");
      }

      /// <summary>Reads string or int value from a script value node</summary>
      /// <param name="node">The node.</param>
      /// <returns>String/Int value</returns>
      ParameterValue  ScriptFileReader::ReadValue(XmlNodePtr& node, const WCHAR* help)
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
         throw FileFormatException(HERE, GuiString(L"Cannot read %s <sval> node", help));
      }

      /// <summary>Reads the variables codearray branch</summary>
      /// <param name="script">The script.</param>
      /// <param name="varBranch">The variable branch.</param>
      /// <param name="argBranch">The argument branch.</param>
      void  ScriptFileReader::ReadVariables(ScriptFile&  script, XmlNodePtr& varBranch, XmlNodePtr& argBranch)
      {
         wstring name;

         // Read ScriptVariables from {name,id} pair
         for (int i = 0; i < varBranch->childNodes->length; i++)
         {
            name = ReadString(varBranch->childNodes->item[i], L"script variable name");
            script.Variables.push_back( ScriptVariable(VariableType::Variable, name, i+1) );
         }
         
         // Modify arguments with extra properties
         for (int i = 0; i < argBranch->childNodes->length; i++)
         {
            XmlNodePtr arg(argBranch->childNodes->item[i]);
            script.Variables[i].Type        = VariableType::Argument;
            script.Variables[i].ValueType   = (ParameterType)ReadInt(arg->childNodes->item[0], L"script argument type");
            script.Variables[i].Description = ReadString(arg->childNodes->item[1], L"script argument description");
         }

      }

   }
}