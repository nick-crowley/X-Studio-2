#include "stdafx.h"
#include "ScriptFileReader.h"
#include <algorithm>
#include "FileStream.h"
#include "XFileInfo.h"

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

      /// <summary>Reads the entire script file</summary>
      /// <returns>New script file</returns>
      /// <exception cref="Logic::ArgumentNullException">Missing node</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      /// <exception cref="Logic::FileFormatException">Corrupt XML / Missing elements / missing attributes</exception>
      /// <exception cref="Logic::InvalidValueException">Invalid script command</exception>
      /// <exception cref="Logic::InvalidOperationException">Invalid goto/gosub command</exception>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      ScriptFile ScriptFileReader::ReadFile(Path path, bool justProperties)
      {
         ScriptFile file;
         
         try
         {
            // Store folder
            Folder = path.Folder;

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
            if (!justProperties)
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
      /// <returns>Appropriate command reader</returns>
      /// <exception cref="Logic::ArgumentNullException">Missing node</exception>
      /// <exception cref="Logic::FileFormatException">Invalid file format</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
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
      /// <exception cref="Logic::ArgumentNullException">Node is null</exception>
      /// <exception cref="Logic::FileFormatException">Not an array sval node</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
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
         throw FileFormatException(HERE, GuiString(L"Cannot read %s from %s <sval> node with value '%s'", help, ReadAttribute(node, L"type").c_str(), ReadAttribute(node, L"val").c_str()));
      }

      
      /// <summary>Reads both the standard and auxiliary commands, then interlaces them in the correct order</summary>
      /// <param name="script">The script.</param>
      /// <param name="stdBranch">The standard commands branch.</param>
      /// <param name="auxBranch">The auxiliary commands branch.</param>
      /// <exception cref="Logic::ArgumentNullException">Missing node</exception>
      /// <exception cref="Logic::FileFormatException">Invalid file format</exception>
      /// <exception cref="Logic::InvalidOperationException">Invalid goto/gosub command</exception>
      /// <exception cref="Logic::InvalidValueException">Invalid goto/gosub command</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
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
            if (!std[i].Syntax.Is(CMD_HIDDEN_JUMP))
               script.Commands.push_back( std[i] );
         }

         // Translate all commands/parameters
         UINT line = 1;
         wstring name;
         for (ScriptCommand& cmd : script.Commands)
         {
            // GOTO/GOSUB: Replace label number parameters with label name params
            if (!cmd.Commented && (cmd.Is(CMD_GOTO_LABEL) || cmd.Is(CMD_GOTO_SUB)))
            {
               // Validate line number
               if (cmd.GetJumpDestination() >= std.size())
                  throw InvalidValueException(HERE, GuiString(L"Command on line %d references an invalid jump destination %d", line, cmd.GetJumpDestination()));
               
               // Convert label number -> label name
               cmd.SetLabelName( std[cmd.GetJumpDestination()].GetLabelName() );
            }
            // LABEL: Store for later
            else if (cmd.Is(CMD_DEFINE_LABEL))
               script.Labels.Add(cmd.GetLabelName(), line);

            // SCRIPT-CALL: Load script properties
            else if (cmd.Is(CMD_CALL_SCRIPT_VAR_ARGS) && !script.ScriptCalls.Contains(name = cmd.GetScriptCallName()))
            {
               try 
               {
                  script.ScriptCalls.Add(name, ReadExternalScript(name));
               }
               catch (ExceptionBase& e ) {
                  Console.Log(HERE, e, GuiString(L"Unable to resolve '%s' call to external script '%s'", script.Name.c_str(), name.c_str()));
               }
            }

            // Translate
            cmd.Translate(script);
            ++line;
         }
      }

      /// <summary>Reads the properties of an external script in the same folder</summary>
      /// <param name="name">script name</param>
      /// <returns>ScriptFile containing properties only</returns>
      ScriptFile  ScriptFileReader::ReadExternalScript(const wstring& name)
      {
         // Generate path
         Path path(Folder + (name + L".pck"));

         // Check for PCK and XML versions
         if (!path.Exists() && (path = path.RenameExtension(L".xml")).Exists() == false)
            throw FileNotFoundException(HERE, Folder+name);

         // Feedback
         Console << L"  Resolving script call: " << Colour::Yellow << name << Colour::White << L"..." << ENDL;

         // Read script
         XFileInfo f(path);
         return ScriptFileReader(f.OpenRead()).ReadFile(path, true);
      }


      /// <summary>Reads the engine version</summary>
      /// <param name="node">The sval node.</param>
      /// <returns>Equivilent game version</returns>
      /// <exception cref="Logic::ArgumentNullException">Missing node</exception>
      /// <exception cref="Logic::FileFormatException">Invalid file format</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      GameVersion ScriptFileReader::ReadEngineVersion(XmlNodePtr& node)
      {
         int val = ReadInt(node, L"script engine version");

         // TODO: Parse engine version
         return GameVersion::TerranConflict;
      }
      
      /// <summary>Reads an integer value from a sval node</summary>
      /// <param name="node">The sval node</param>
      /// <returns>Integer value</returns>
      /// <exception cref="Logic::ArgumentNullException">Node is null</exception>
      /// <exception cref="Logic::FileFormatException">Not an integer sval node</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      int  ScriptFileReader::ReadInt(XmlNodePtr& node, const WCHAR* help)
      {
         // Ensure node is script value
         ReadElement(node, L"sval");

         // Ensure type is int
         if (ReadAttribute(node, L"type") != L"int")
            throw FileFormatException(HERE, GuiString(L"Cannot read %s from %s <sval> node with value '%s'", help, ReadAttribute(node, L"type").c_str(), ReadAttribute(node, L"val").c_str()));

         // Read value
         return _wtoi(ReadAttribute(node, L"val").c_str());
      }
      
      /// <summary>Reads the string value from a sval node</summary>
      /// <param name="node">The node.</param>
      /// <returns>String value</returns>
      /// <exception cref="Logic::ArgumentNullException">Node is null</exception>
      /// <exception cref="Logic::FileFormatException">Not a string sval node</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      wstring  ScriptFileReader::ReadString(XmlNodePtr& node, const WCHAR* help)
      {
         // Ensure node is script value
         ReadElement(node, L"sval");

         // Ensure type is string
         if (ReadAttribute(node, L"type") != L"string")
            throw FileFormatException(HERE, GuiString(L"Cannot read %s from %s <sval> node with value '%s'", help, ReadAttribute(node, L"type").c_str(), ReadAttribute(node, L"val").c_str()));

         // Read value
         return ReadAttribute(node, L"val");
      }

      /// <summary>Reads string or int value from a script value node</summary>
      /// <param name="node">The node.</param>
      /// <returns>String/Int value</returns>
      /// <exception cref="Logic::ArgumentNullException">Node is null</exception>
      /// <exception cref="Logic::FileFormatException">Not a string/integer sval node</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
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
         throw FileFormatException(HERE, GuiString(L"Cannot read %s from %s <sval> node with value '%s'", help, ReadAttribute(node, L"type").c_str(), ReadAttribute(node, L"val").c_str()));
      }

      /// <summary>Reads the variables codearray branch</summary>
      /// <param name="script">The script.</param>
      /// <param name="varBranch">The variable branch.</param>
      /// <param name="argBranch">The argument branch.</param>
      /// <exception cref="Logic::ArgumentNullException">Missing node</exception>
      /// <exception cref="Logic::FileFormatException">Invalid file format</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      void  ScriptFileReader::ReadVariables(ScriptFile&  script, XmlNodePtr& varBranch, XmlNodePtr& argBranch)
      {
         wstring name;

         // Read ScriptVariables from {name,id} pair
         for (int i = 0; i < varBranch->childNodes->length; i++)
         {
            name = ReadString(varBranch->childNodes->item[i], L"script variable name");
            script.Variables.Add(name);
         }
         
         // Upgrade first N variables into arguments 
         for (int i = 0; i < argBranch->childNodes->length; i++)
         {
            XmlNodePtr arg(argBranch->childNodes->item[i]);

            ScriptVariable& var = script.Variables[i];
            var.Type        = VariableType::Argument;
            var.ValueType   = (ParameterType)ReadInt(arg->childNodes->item[0], L"script argument type");
            var.Description = ReadString(arg->childNodes->item[1], L"script argument description");
         }

      }

   }
}