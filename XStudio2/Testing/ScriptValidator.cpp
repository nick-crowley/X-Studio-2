#include "stdafx.h"
#include "../Logic/ScriptFile.h"
#include "../Logic/FileStream.h"
#include "../Logic/XFileInfo.h"
#include "../Logic/ScriptFileWriter.h"
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
         Console << "Parsing and compiling..." << Colour::Yellow << truePath << ENDL;

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


      /// <summary>Reads and translates a script without parsing or compiling it</summary>
      /// <param name="truePath">true path.</param>
      /// <param name="displayPath">path containing folder to use to resolve script-calls</param>
      /// <returns></returns>
      ScriptFile  ScriptValidator::ReadScript(Path truePath, Path displayPath)
      {
         Console << "Reading: " << Colour::Yellow << truePath << ENDL;

         // Read input file
         ScriptFileReader r(XFileInfo(truePath).OpenRead());
         return r.ReadFile(displayPath, false);
      }

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Validates the input file</summary>
      bool  ScriptValidator::Validate()
      {
         try
         {
            TempPath tmp;  

            Console << Cons::Heading << L"Validating: " << Colour::Yellow << FullPath << ENDL;

            // Read script. Extract/preserve cmds+text. Compile
            auto orig = ReadScript(FullPath, FullPath);
            auto orig_cmds = orig.Commands.Input;
            auto orig_txt = GetAllLines(orig_cmds);
            CompileScript(orig, FullPath);

            // Write copy
            Console << "Writing validation script: " << Colour::Yellow << tmp << Colour::White << "..." << ENDL;
            ScriptFileWriter w(StreamPtr(new FileStream(tmp, FileMode::CreateAlways, FileAccess::Write)));
            w.Write(orig);
            w.Close();

            // Read copy back in. Extract text. Compile
            auto copy = ReadScript(tmp, FullPath.Folder+tmp.FileName);  // Supply original folder to enable script-call resolution
            auto copy_cmds = copy.Commands.Input;
            auto copy_txt = GetAllLines(copy_cmds);
            CompileScript(copy, tmp);
            
            // Compare command text
            Console << Cons::Bold << "Comparing translated command text..." << ENDL;
            ScriptTextValidator::Compare(orig_txt, copy_txt);

            try
            {
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
               Console << ENDL << "Command tree: " << Colour::Yellow << FullPath;
               ScriptParser orig_parser(orig, GetAllLines(orig_cmds), orig.Game);
               if (orig_parser.Errors.empty()) 
                  orig_parser.Compile();
               orig_parser.Print();

               Console << ENDL << "Command tree: " << Colour::Yellow << tmp;
               ScriptParser copy_parser(copy, GetAllLines(copy_cmds), copy.Game);
               if (copy_parser.Errors.empty()) 
                  copy_parser.Compile();
               copy_parser.Print();
               
               throw;
            }

            // Success!
            Console << Colour::Green << "Validation Successful" << ENDL;
            return true;
         }
         catch (ExceptionBase& e)
         {
            Console.Log(HERE, e);
            Console << Colour::Red << "Validation FAILED" << ENDL;
            return false;
         }
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
      
   }
}

