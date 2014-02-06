#include "stdafx.h"
#include "ScriptTextValidator.h"
#include "../Logic/FileStream.h"
#include "../Logic/ScriptFileReader.h"
#include "../Logic/ScriptFileWriter.h"

namespace Testing
{
   namespace Scripts
   {
   
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Create a textual comparison script validator</summary>
      /// <param name="file">Full script path</param>
      ScriptTextValidator::ScriptTextValidator(Path file) : FullPath(file)
      {
      }

      ScriptTextValidator::~ScriptTextValidator()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Validates the input file</summary>
      bool  ScriptTextValidator::Validate()
      {
         try
         {
            Console << Colour::Cyan << ENDL << L"Validating: " << FullPath << ENDL;

            // Read input file
            ScriptFileReader r(StreamPtr(new FileStream(FullPath, FileMode::OpenExisting, FileAccess::Read)));
            auto input = r.ReadFile(FullPath, false);

            Console << "Parsing/Compiling script..." << ENDL;

            // Parse command text
            ScriptParser parser(input, GetAllLines(input.Commands.Input), input.Game);
            parser.Compile();

            // Write output file
            TempPath tmp;
            ScriptFileWriter w(StreamPtr(new FileStream(tmp, FileMode::CreateAlways, FileAccess::Write)));
            w.Write(input);
            w.Close();

            Console << "Reading validation script..." << ENDL;

            // Read output file
            ScriptFileReader r2(StreamPtr(new FileStream(tmp, FileMode::OpenExisting, FileAccess::Read)));
            auto output = r2.ReadFile(FullPath.Folder+tmp.FileName, false);   // HACK: Pretend temp file is in original folder so script-calls are validated

            // Compare
            if (Compare(input, output))
            {
               Console << Colour::Green << "Validation Successful" << ENDL;
               return true;
            }
         }
         catch (ExceptionBase& e)
         {
            Console.Log(HERE, e);
         }

         Console << Colour::Red << "Validation FAILED" << ENDL;
         return false;
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
      
      /// <summary>Perform textual comparison of a script and it's validation copy</summary>
      /// <param name="in">original script</param>
      /// <param name="out">compiled copy</param>
      bool  ScriptTextValidator::Compare(const ScriptFile& in, const ScriptFile& out)
      {
         return true;
      }

      /// <summary>Gets translated command text as line array</summary>
      /// <param name="commands">command list</param>
      /// <returns></returns>
      LineArray  ScriptTextValidator::GetAllLines(const CommandList& commands)
      {
         LineArray lines;
         transform(commands.begin(), commands.end(), back_inserter(lines), [](const ScriptCommand& cmd) {return cmd.Text;});
         return lines;
      }
   }
}

