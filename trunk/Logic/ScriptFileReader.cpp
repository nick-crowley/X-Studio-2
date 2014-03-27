#include "stdafx.h"
#include "ScriptFileReader.h"
#include <algorithm>
#include "FileStream.h"
#include "XFileInfo.h"
#include "XFileSystem.h"
#include "PreferencesLibrary.h"

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
      ScriptFileReader::ScriptFileReader(StreamPtr in) : ScriptValueReader(in)
      {
      }

      /// <summary>Nothing</summary>
      ScriptFileReader::~ScriptFileReader()
      {
      }
      
      /// <summary>Resolves the path of a script-call. If script cannot found the path is empty</summary>
      /// <param name="folder">Initial folder</param>
      /// <param name="script">Scriptname without extension</param>
      /// <exception cref="Logic::IOException">API function failed</exception>
      ScriptCallPath::ScriptCallPath(const Path& folder, const wstring& script)
      {
         // Generate .PCK path within input folder
         Path path(folder + (script+L".pck"));

         // Try PCK then XML
         if (path.Exists() || (path = path.RenameExtension(L".xml")).Exists())
            Assign(path.c_str());

         // Failed: Try game folder
         else
         {
            // Failed: Try 'scripts' subfolder of game folder
            path = XFileSystem::GetPath(PrefsLib.GameDataFolder, PrefsLib.GameDataVersion, XFolder::Scripts) + (script+L".pck");

            // Try PCK then XML
            if (path.Exists() || (path = path.RenameExtension(L".xml")).Exists())
               Assign(path.c_str());

            // Failed: empty path
         }
         
      }

      // ------------------------------- STATIC METHODS -------------------------------
      
      /// <summary>Reads the properties of an external script</summary>
      /// <param name="folder">folder to search</param>
      /// <param name="script">script name WITHOUT extension</param>
      /// <param name="silent">Skip feedback to console</param>
      /// <returns>ScriptFile containing properties only</returns>
      ScriptFile  ScriptFileReader::ReadExternalScript(Path folder, const wstring& script, bool silent)
      {
         try
         {
            // Feedback
            if (!silent)
               Console << L"  Resolving script call: " << Cons::Yellow << script << Cons::White << L"...";

            // Check for .PCK/.XML in folder and game-folder
            ScriptCallPath path(folder, script);
            if (!path.Exists())
               throw FileNotFoundException(HERE, folder+script);

            // Read script
            ScriptFile sf = ScriptFileReader(XFileInfo(path).OpenRead()).ReadFile(path, true);

            // Feedback
            if (!silent)
               Console << Cons::Green << "Success" << ENDL;
            return sf;
         }
         catch (ExceptionBase& e) {
            // Feedback
            if (!silent)
               Console << Cons::Error << "Failed: " << e.Message.Remove(L"\r\n") << ENDL;
            throw;
         }
      }

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
      ScriptFile ScriptFileReader::ReadFile(Path path, bool justProperties, bool dropJMPs)
      {
         try
         {
            ScriptFile file(path);

            // Read file
            LoadDocument();

            // Read properties
            file.Name        = ReadString(CodeArray, 0, L"script name");
            file.Game        = EngineVersionConverter::ToGame(ReadInt(CodeArray, 1, L"script engine version"));
            file.Description = ReadString(CodeArray, 2, L"script description");
            file.Version     = (UINT)ReadInt(CodeArray, 3, L"script version");
            file.LiveData    = (UINT)ReadInt(CodeArray, 4, L"script live data flag") != 0;

            // Arguments/Variables
            ReadVariables(file, GetChild(CodeArray, 5, L"variables branch"), GetChild(CodeArray, 7, L"codearray arguments branch")); 

            // Commands
            if (!justProperties)
               ReadCommands(file, GetChild(CodeArray, 6, L"standard commands branch"), GetChild(CodeArray, 8, L"auxiliary commands branch"), dropJMPs);

            // Command ID
            file.CommandID = ReadValue(CodeArray, 9, L"script command ID");

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
         //int size = ReadArray(cmdBranch, L"script command branch");

         // Examine command ID (from first/second node)
         int id = ReadInt(cmdBranch, (type == CommandType::Standard ? 0 : 1), L"script command ID");

         // Create appropriate reader
         switch (id)
         {
         // Simple Auxiliary: 
         case CMD_ELSE:
         case CMD_BREAK:
         case CMD_CONTINUE:
         case CMD_END:
         case CMD_NOP:
         case CMD_COMMENT:           return ReaderPtr(new AuxiliaryCommandReader(*this, script, cmdBranch));

         case CMD_COMMAND_COMMENT:   return ReaderPtr(new CommentedCommandReader(*this, script, cmdBranch));
         case CMD_CALL_SCRIPT:       return ReaderPtr(new ScriptCallCommandReader(*this, script, cmdBranch));
         case CMD_EXPRESSION:        return ReaderPtr(new ExpressionCommandReader(*this, script, cmdBranch));

         default:                    return ReaderPtr(new StandardCommandReader(*this, script, cmdBranch));
         }
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
      void  ScriptFileReader::ReadCommands(ScriptFile&  script, XmlNodePtr& stdBranch, XmlNodePtr& auxBranch, bool dropJMPs)
      {
         CommandArray  std;
         CommandList   aux; 

         // Read standard commands
         for (int i = 0; i < stdBranch->childNodes->length; i++)
            std.push_back( GetCommandReader(script, CommandType::Standard, GetChild(stdBranch, i, L"standard command"))->ReadCommand() );

         // Read auxiliary commands
         for (int i = 0; i < auxBranch->childNodes->length; i++)
            aux.push_back( GetCommandReader(script, CommandType::Auxiliary, GetChild(auxBranch, i, L"auxiliary command"))->ReadCommand() );

         // Interlace in correct order
         for (UINT i = 0; i < std.size(); i++)
         {
            // Insert any/all auxiliary commands preceeding next standard command
            while (!aux.empty() && aux.front().RefIndex <= i)
            {
               script.Commands.AddInput(aux.front());
               aux.pop_front();
            }

            // Drop/Keep JMP commands
            if (!dropJMPs || !std[i].Syntax.Is(CMD_HIDDEN_JUMP))
               // Insert standard command 
               script.Commands.AddInput(std[i]);
         }
         // Insert any trailing comments
         for (auto& c : aux)
            script.Commands.AddInput(c);


         // Translate all commands/parameters
         UINT line = 1;
         for (ScriptCommand& cmd : script.Commands.Input)
         {
            // GOTO/GOSUB: Replace label number parameters with label name params
            if (!cmd.Commented && (cmd.Is(CMD_GOTO_LABEL) || cmd.Is(CMD_GOTO_SUB)))
            {
               // Validate line number
               if (cmd.GetJumpDestination() >= std.size())
                  throw InvalidValueException(HERE, VString(L"Goto/gosub on line %d jumps to non-existent address %d", line, cmd.GetJumpDestination()));
               else if (!std[cmd.GetJumpDestination()].Is(CMD_DEFINE_LABEL))
                  throw InvalidValueException(HERE, VString(L"Goto/gosub on line %d jumps to non-label : %s", line, std[cmd.GetJumpDestination()].Syntax.Text.c_str()));
               
               // Convert label number -> label name
               cmd.SetLabelName( std[cmd.GetJumpDestination()].GetLabelName() );
            }
            // LABEL: Store for later
            else if (cmd.Is(CMD_DEFINE_LABEL))
               script.Labels.Add(cmd.GetLabelName(), line);

            // VARG SCRIPT-CALL: Load script properties
            else if (cmd.Syntax.IsVArgument())
            {
               wstring name;

               try 
               {  
                  // Find name of target script (may be a variable)
                  name = cmd.GetScriptCallName();

                  // Read unless previously read
                  if (!name.empty() && !script.ScriptCalls.Contains(name))
                     script.ScriptCalls.Add(name, ReadExternalScript(script.FullPath.Folder, name, false));
               }
               catch (ExceptionBase&) {
                  //if (e.ErrorID != ERROR_FILE_NOT_FOUND)
                     //Console.Log(HERE, e, VString(L"Unable to resolve '%s' call to external script '%s'", script.Name.c_str(), name.c_str()));
               }
            }

            // Translate
            cmd.Translate(script);

            // Distinguish constants from variables
            for (auto& p : cmd.Parameters)
               p.Identify(script);

            // Append command to offline buffer
            script.OfflineBuffer += cmd.Text;
            script.OfflineBuffer += L'\n';

            // Advance
            ++line;
         }
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
            // Variable
            name = ReadString(varBranch, i, L"script variable name");
            ScriptVariable& var = script.Variables.Add(name);

            // Argument: Read extra properties
            if (i < argBranch->childNodes->length)
            {
               XmlNodePtr arg = GetChild(argBranch, i, L"script argument branch");

               var.Type = VariableType::Argument;
               var.ParamType = (ParameterType)ReadInt(arg, 0, L"script argument type");
               var.Description = ReadString(arg, 1, L"script argument description");
            }
         }
      }

   }
}