#include "stdafx.h"
#include "../Logic/ScriptFile.h"
#include "../Logic/FileStream.h"
#include "../Logic/XFileInfo.h"
#include "../Logic/ScriptFileWriter.h"
#include "../Logic/IndentationStack.h"
#include "ScriptValidator.h"


namespace Testing
{
   namespace Scripts
   {
   
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Create a textual comparison script validator</summary>
      /// <param name="file">Full script path</param>
      ScriptValidator::ScriptValidator(Path file) : FullPath(file)
      {
      }

      ScriptValidator::~ScriptValidator()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------
      
      /// <summary>Compiles a script</summary>
      /// <param name="s">script</param>
      /// <param name="truePath">true path.</param>
      void  ScriptValidator::CompileScript(ScriptFile& s, Path truePath)
      {
         Console << "Parsing and compiling..." << Cons::Yellow << truePath << ENDL;

         // Parse command text
         auto ln = GetAllLines(s.Commands.Input);
         ScriptParser parser(s, ln, s.Game);

         // Compile
         if (parser.Errors.empty())
            parser.Compile();

         // Check for syntax errors
         if (!parser.Errors.empty())
         {
            // Print tree 
            parser.Print();

            // Print messages
            for (auto& e : parser.Errors)
               Console << "ERROR: " << e.Line << " : " << e.Message << " : " << e.Text << ENDL;

            // Abort
            throw ValidationException(HERE, GuiString(L"Unable to parse: %s", truePath.c_str()));
         }
      }
      
      /// <summary>Gets translated command text as line array</summary>
      /// <param name="commands">command list</param>
      /// <returns></returns>
      LineArray  ScriptValidator::GetAllLines(const CommandList& commands)
      {
         LineArray lines;
         transform(commands.begin(), commands.end(), back_inserter(lines), [](const ScriptCommand& cmd) {return cmd.Text;});
         return lines;
      }

      
      /// <summary>Prints a command list in tree format akin to the script parser.</summary>
      /// <param name="list">List of translated commands</param>
      void  ScriptValidator::PrintTree(const CommandList& list)
      {
         IndentationStack indent;

         // Header
         Console << ENDL << "Ln  Index  Logic            Text        " << Cons::Purple << Cons::Bold << L"DIRECT TRANSLATION";
         Console << ENDL << "-------------------------------------------------------" << ENDL; 
      
         // Print commands
         UINT LineNumber = 1, stdIndex = 0, auxIndex = 0;
         for (auto& cmd : list)
         {
            // Logic/Text
            GuiString logic(::GetString(cmd.Logic)),
                      txt(cmd.Text);
            Cons      colour(Cons::White);
            
            // Line#
            Console << GuiString(!cmd.Is(CMD_HIDDEN_JUMP) ? L"%03d: " : L"---: ", LineNumber);

            // Index/RefIndex
            if (cmd.Is(CommandType::Standard) && !cmd.Commented)
               Console << GuiString(L"%03d: ", stdIndex);
            else
               Console << Cons::Yellow << GuiString(L"%03d: ", auxIndex) << Cons::White;
            
            // Logic
            switch (cmd.Logic)
            {
            // Conditional:
            default: 
               colour = Cons::Cyan;
               for (auto ps : cmd.Syntax.Parameters)
                  if (ps.IsRetVar())
                  {
                     UINT jumpTarget = ReturnValue(cmd.Parameters[ps.PhysicalIndex].Value.Int).Destination;
                     if (jumpTarget)
                        txt = GuiString(L"Jump-if-false: %d", jumpTarget);
                     break;
                  }
               break;

            // NOP:
            case BranchLogic::NOP:
               colour = Cons::Yellow;
               logic = cmd.Commented ? L"Cmd" : L"NOP";
               break;

            // Command:
            case BranchLogic::None:
               if (cmd.Is(CMD_HIDDEN_JUMP) || cmd.Is(CMD_GOTO_LABEL) || cmd.Is(CMD_GOTO_SUB))
               {
                  colour = Cons::Green; 
                  logic = cmd.Is(CMD_HIDDEN_JUMP) ? L"Jmp" : L"Goto";
                  txt = GuiString(L"Unconditional: %s", cmd.Parameters[0].Value.ToString().c_str());
               }
               else if (cmd.Is(CMD_DEFINE_LABEL))
               {
                  colour = Cons::Purple;
                  logic = L"Proc";
               }
               else if (cmd.Is(CMD_RETURN))
               {
                  colour = Cons::Cyan;
                  logic = L"Ret";
               }
               else if (cmd.Syntax == CommandSyntax::Unrecognised)
               {  // Print entire line in red
                  Console << (colour = Cons::Red);
                  logic = L"???";
               }
               else
                  logic = L"Cmd";
               break;
            }

            // Print
            indent.PreDisplay(cmd);
            Console << Indent(indent.Size) << colour << logic << Cons::White << L" : " << colour << txt << ENDL;
            indent.PostDisplay(cmd);

            // Advance line/index
            if (cmd.Is(CommandType::Standard) && !cmd.Commented)
               ++stdIndex;
            else
               ++auxIndex;

            if (!cmd.Is(CMD_HIDDEN_JUMP))
               ++LineNumber;
         }
      
         Console << ENDL;
      }

      /// <summary>Reads and translates a script without parsing or compiling it</summary>
      /// <param name="truePath">true path.</param>
      /// <param name="displayPath">path containing folder to use to resolve script-calls</param>
      /// <param name="dropJMPs">Whether to drop JMP commands</param>
      /// <returns></returns>
      ScriptFile  ScriptValidator::ReadScript(Path truePath, Path displayPath, bool dropJMPs)
      {
         Console << "Reading: " << Cons::Yellow << truePath << ENDL;

         // Read input file
         ScriptFileReader r(XFileInfo(truePath).OpenRead());
         return r.ReadFile(displayPath, false, dropJMPs);
      }

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Prints the command tree of the input file</summary>
      void  ScriptValidator::Print()
      {
         try
         {
            // Read script, including JMPs
            auto orig = ReadScript(FullPath, FullPath, false);
            PrintTree(orig.Commands.Input);
         }
         catch (ExceptionBase& e)
         {
            Console.Log(HERE, e, GuiString(L"Unable to print translation tree for '%s'", FullPath.c_str()));
         }
      }

      /// <summary>Validates the input file</summary>
      bool  ScriptValidator::Validate()
      {
         try
         {
            TempPath tmp;  

            Console << Cons::Heading << L"Validating: " << Cons::Yellow << FullPath << ENDL;

            // Read script, including JMPs
            auto orig = ReadScript(FullPath, FullPath, false);

            // Preserve copy of command list with JMPs
            auto orig_cmds = orig.Commands.Input;
            orig.Commands.Input.remove_if([](ScriptCommand& c) {return c.Is(CMD_HIDDEN_JUMP);} );
            auto orig_txt = GetAllLines(orig.Commands.Input);
            
            // Compile
            CompileScript(orig, FullPath);

            // Write copy
            Console << "Writing validation script: " << Cons::Yellow << tmp << Cons::White << "..." << ENDL;
            ScriptFileWriter w(StreamPtr(new FileStream(tmp, FileMode::CreateAlways, FileAccess::Write)));
            w.Write(orig);
            w.Close();

            // Read copy back in. Extract text
            auto copy = ReadScript(tmp, FullPath.Folder+tmp.FileName, true);  // Supply original folder to enable script-call resolution
            auto copy_txt = GetAllLines(copy.Commands.Input);

            // Preserve command list WITHOUT JMPS. Compile
            auto copy_cmds = copy.Commands.Input;
            CompileScript(copy, tmp);
            
            try
            {
               // Compare command text
               Console << Cons::Bold << "Comparing translated command text..." << ENDL;
               ScriptTextValidator::Compare(orig_txt, copy_txt);

               // Compare input command list
               Console << Cons::Bold << "Comparing intermediate code..." << ENDL;
               ScriptTextValidator::Compare(orig, copy);

               // Compare xml
               Console << Cons::Bold << "Comparing generated xml..." << ENDL;
               ScriptCodeValidator code(XFileInfo(FullPath).OpenRead(), XFileInfo(tmp).OpenRead());
               code.Compare();
            }
            // Failed: Print trees
            catch (ExceptionBase&)
            {
               Console << ENDL << "Raw Input tree: " << Cons::Yellow << FullPath;
               PrintTree(orig_cmds);

               Console << ENDL << "Compiled Output tree: " << Cons::Yellow << FullPath;
               ScriptParser copy_parser(copy, GetAllLines(copy_cmds), copy.Game);
               if (copy_parser.Errors.empty()) 
                  copy_parser.Compile();
               copy_parser.Print();
               throw;
            }

            // Success!
            Console << Cons::Green << "Validation Successful" << ENDL;
            return true;
         }
         catch (ExceptionBase& e)
         {
            Console.Log(HERE, e);
            Console << Cons::Error << "Validation FAILED" << ENDL;
            return false;
         }
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
      
   }
}

