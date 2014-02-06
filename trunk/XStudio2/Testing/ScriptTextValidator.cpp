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

      ScriptTextValidator::ScriptTextValidator(Path file) : FullPath(file)
      {
      }

      ScriptTextValidator::~ScriptTextValidator()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      void  ScriptTextValidator::Validate()
      {
         // Read input file
         ScriptFileReader r(StreamPtr(new FileStream(FullPath, FileMode::OpenExisting, FileAccess::Read)));
         auto input = r.ReadFile(FullPath, false);

         // Parse command text
         ScriptParser parser(input, GetAllLines(input.Commands.Input), input.Game);
         parser.Compile();

         // Write output file
         ScriptFileWriter w(StreamPtr(new FileStream(L"", FileMode::CreateAlways, FileAccess::Write)));
         w.Write(input);
         w.Close();

         // Read output file
         ScriptFileReader r2(StreamPtr(new FileStream(L"", FileMode::OpenExisting, FileAccess::Read)));
         auto output = r2.ReadFile(L"", false);

         // Compare
         Compare(input, output);
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

      // ------------------------------- PRIVATE METHODS ------------------------------
      
      /// <summary>Compares the specified in.</summary>
      /// <param name="in">original script</param>
      /// <param name="out">compiled copy</param>
      void  ScriptTextValidator::Compare(const ScriptFile& in, const ScriptFile& out)
      {
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

