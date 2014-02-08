#include "stdafx.h"
#include "../Logic/ScriptFile.h"
#include "../Logic/FileStream.h"
#include "../Logic/XFileInfo.h"
#include "../Logic/ScriptFileReader.h"
#include "../Logic/ScriptFileWriter.h"
#include "ScriptTextValidator.h"

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
      
      /// <summary>Compiles a script</summary>
      /// <param name="s">script</param>
      /// <param name="truePath">true path.</param>
      void  ScriptTextValidator::CompileScript(ScriptFile& s, Path truePath)
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
      LineArray  ScriptTextValidator::GetAllLines(const CommandList& commands)
      {
         LineArray lines;
         transform(commands.begin(), commands.end(), back_inserter(lines), [](const ScriptCommand& cmd) {return cmd.Text;});
         return lines;
      }


      /// <summary>Reads and translates a script without parsing or compiling it</summary>
      /// <param name="truePath">true path.</param>
      /// <param name="displayPath">path containing folder to use to resolve script-calls</param>
      /// <returns></returns>
      ScriptFile  ScriptTextValidator::ReadScript(Path truePath, Path displayPath)
      {
         Console << "Reading: " << Colour::Yellow << truePath << ENDL;

         // Read input file
         ScriptFileReader r(XFileInfo(truePath).OpenRead());
         return r.ReadFile(displayPath, false);
      }


      /// <summary>Create text mismatch exception</summary>
      /// <param name="src">throw source.</param>
      /// <param name="prop">name of property that mismatches</param>
      /// <param name="a">original</param>
      /// <param name="b">copy</param>
      /// <returns></returns>
      ValidationException  ScriptTextValidator::TextMismatch(const GuiString& src, const GuiString& prop, const GuiString& a, const GuiString& b)
      {
         return ValidationException(src, GuiString(L"%s mismatch: original='%s'\ncopy='%s'", prop.c_str(), a.c_str(), b.c_str()) );
      }
      
      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Validates the input file</summary>
      bool  ScriptTextValidator::Validate()
      {
         try
         {
            TempPath tmp;  

            Console << Cons::Heading << L"Validating: " << Colour::Yellow << FullPath << ENDL;

            // Read script. Extract text. Compile
            auto orig = ReadScript(FullPath, FullPath);
            auto orig_txt = GetAllLines(orig.Commands.Input);
            CompileScript(orig, FullPath);

            // Write copy
            Console << "Writing validation script: " << Colour::Yellow << tmp << Colour::White << "..." << ENDL;
            ScriptFileWriter w(StreamPtr(new FileStream(tmp, FileMode::CreateAlways, FileAccess::Write)));
            w.Write(orig);
            w.Close();

            // Read copy back in. Extract text. Compile
            auto copy = ReadScript(tmp, FullPath.Folder+tmp.FileName);  // Supply original folder to enable script-call resolution
            auto copy_txt = GetAllLines(copy.Commands.Input);
            CompileScript(copy, tmp);
            
            // Compare files
            Console << Cons::Bold << "Comparing translated command text..." << ENDL;
            Compare(orig_txt, copy_txt);

            // Compare files
            Console << Cons::Bold << "Comparing intermediate code..." << ENDL;
            Compare(orig, copy);

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
      
      /// <summary>Perform textual comparison of a script and it's validation copy</summary>
      /// <param name="in">original script</param>
      /// <param name="out">compiled copy</param>
      bool  ScriptTextValidator::Compare(const LineArray& in, const LineArray& out)
      {
         // Variables/Arguments count
         if (in.size() != out.size())
            throw TextMismatch(HERE, L"command count", GuiString(L"%d", in.size()), GuiString(L"%d", out.size()));

         else 
         {
            UINT line = 1;
            for (auto c1 = in.begin(), c2 = out.begin(); c1 != in.end(); ++c1, ++c2)
            {
               // Skip comparison of comments
               /*if (!c1->empty() && !c2->empty() && c1->front() == '*' && c2->front() == '*')
                  continue;*/
               // Compare command text
               if (*c1 != *c2)
                  throw TextMismatch(HERE, GuiString(L"(line %d) command text", line), *c1, *c2);
            
               ++line;
            }
         }

         return true;
      }

      /// <summary>Perform comparison of commands generated by compiler between script and it's validation copy</summary>
      /// <param name="in">original script</param>
      /// <param name="out">compiled copy</param>
      bool  ScriptTextValidator::Compare(const ScriptFile& in, const ScriptFile& out)
      {
         // Properties
         if (in.Name != out.Name)
            throw TextMismatch(HERE, L"name", in.Name, out.Name);
         if (in.Description != out.Description)
            throw TextMismatch(HERE, L"description", in.Description, out.Description);
         if (in.Version != out.Version)
            throw TextMismatch(HERE, L"version", GuiString(L"%d", in.Version), GuiString(L"%d", out.Version));
         if (in.Game != out.Game)
            throw TextMismatch(HERE, L"game version", VersionString(in.Game), VersionString(out.Game));
         if (in.GetCommandName() != out.GetCommandName())
            throw TextMismatch(HERE, L"command ID", in.GetCommandName(), out.GetCommandName());

         // Variables/Arguments count
         if (in.Variables.Count != out.Variables.Count)
            throw TextMismatch(HERE, L"variable count", GuiString(L"%d", in.Variables.Count), GuiString(L"%d", out.Variables.Count));

         // Variables/Arguments
         else for (auto v1 = in.Variables.begin(), v2 = out.Variables.begin(); v1 != in.Variables.end(); ++v1, ++v2)
            Compare(*v1, *v2);

         // Command count
         if (in.Commands.Input.size() != out.Commands.Input.size())
            throw TextMismatch(HERE, L"command count", GuiString(L"%d", in.Commands.Input.size()), GuiString(L"%d", out.Commands.Input.size()));
         else 
         {
            UINT line = 1;
            // Commands
            for (auto c1 = in.Commands.Input.begin(), c2 = out.Commands.Input.begin(); c1 != in.Commands.Input.end(); ++c1, ++c2)
            {
               Compare(line, *c1, *c2);
               ++line;
            }
         }

         return true;
      }

      /// <summary>Perform textual comparison two script variables</summary>
      /// <param name="in">original variable</param>
      /// <param name="out">compiled copy</param>
      bool  ScriptTextValidator::Compare(const ScriptVariable& in, const ScriptVariable& out)
      {
         try
         {
            if (in.Name != out.Name)
               throw TextMismatch(HERE, GuiString(L"Arg/Var Name (id=%d)", in.ID), in.Name, out.Name);
            if (in.ID != out.ID)
               throw TextMismatch(HERE, GuiString(L"Arg/Var ID '%s'", in.Name.c_str()), GuiString(L"%d", in.ID), GuiString(L"%d", out.ID));
            if (in.Type != out.Type)
               throw TextMismatch(HERE, GuiString(L"Arg/Var type flag of '%s'", in.Name.c_str()), GetString(in.Type), GetString(out.Type));

            if (in.Description != out.Description)
               throw TextMismatch(HERE, GuiString(L"Argument description '%s'", in.Name.c_str()), in.Description, out.Description);
            if (in.ValueType != out.ValueType)
               throw TextMismatch(HERE, GuiString(L"Argument Type '%s'", in.Name.c_str()), GetString(in.ValueType), GetString(out.ValueType));
         }
         catch (ExceptionBase&)
         {
            Console << "Orig: " << in << ENDL;
            Console << "Copy: " << out << ENDL;
            throw;
         }

         return true;
      }

      /// <summary>Perform textual comparison of two script commands</summary>
      /// <param name="line">1-based line number</param>
      /// <param name="in">original commands</param>
      /// <param name="out">compiled copy</param>
      bool  ScriptTextValidator::Compare(UINT line, const ScriptCommand& in, const ScriptCommand& out)
      {
         // Syntax + Text
         if (in.Syntax != out.Syntax)
            throw TextMismatch(HERE, GuiString(L"(line %d) command syntax", line), in.Syntax.Text, out.Syntax.Text);
         if (in.Text != out.Text)
            throw TextMismatch(HERE, GuiString(L"(line %d) command text", line), in.Text, out.Text);

         // Parameter count
         if (in.Parameters.size() != out.Parameters.size())
            throw TextMismatch(HERE, GuiString(L"(line %d) parameter count", line), GuiString(L"%d", in.Parameters.size()), GuiString(L"%d", out.Parameters.size()));
         else 
         {
            UINT param = 1;

            // Parameters
            for (auto p1 = in.Parameters.begin(), p2 = out.Parameters.begin(); p1 != in.Parameters.end(); ++p1, ++p2)
            {
               Compare(line, param, *p1, *p2);
               ++param;
            }
         }
         
         return true;
      }

      /// <summary>Perform textual comparison of two script parameters</summary>
      /// <param name="line">1-based line number</param>
      /// <param name="param">1-based parameter index</param>
      /// <param name="in">original parameter</param>
      /// <param name="out">compiled copy</param>
      bool  ScriptTextValidator::Compare(UINT line, UINT param, const ScriptParameter& in, const ScriptParameter& out)
      {
         try
         {
            // Type
            if (in.Syntax.Type != out.Syntax.Type)
               throw TextMismatch(HERE, GuiString(L"(line %d, param %d) syntax", line, param), GetString(in.Syntax.Type), GetString(out.Syntax.Type));

            // Text
            if (in.Text != out.Text)
               throw TextMismatch(HERE, GuiString(L"(line %d, param %d) text", line, param), in.Text, out.Text);

            // Value
            if (in.Value.Type != out.Value.Type)
               throw TextMismatch(HERE, GuiString(L"(line %d, param %d) value type", line, param), GetString(in.Value.Type), GetString(out.Value.Type));

            else if (in.Value.Type == ValueType::Int && in.Value.Int != out.Value.Int)
               throw TextMismatch(HERE, GuiString(L"(line %d, param %d) value", line, param), GuiString(L"%d", in.Value.Int), GuiString(L"%d", out.Value.Int));

            else if (in.Value.Type == ValueType::String && in.Value.String != out.Value.String)
               throw TextMismatch(HERE, GuiString(L"(line %d, param %d) value", line, param), in.Value.String, out.Value.String);
         }
         catch (ExceptionBase&)
         {
            Console << "Orig: " << in << ENDL;
            Console << "Copy: " << out << ENDL;
            throw;
         }

         return true;
      }

   }
}

