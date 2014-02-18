#include "stdafx.h"
#include "SyntaxLibrary.h"
#include "CommandLexer.h"
#include "FileStream.h"
#include "LegacySyntaxReader.h"

namespace Logic
{
   namespace Scripts
   {
      SyntaxLibrary  SyntaxLibrary::Instance;

      // -------------------------------- CONSTRUCTION --------------------------------

      SyntaxLibrary::SyntaxLibrary()
      {
      }


      SyntaxLibrary::~SyntaxLibrary()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------
      
      /// <summary>Clears the library</summary>
      void  SyntaxLibrary::Clear()
      {
         Commands.clear();
         NameTree.Clear();
         Groups.clear();
      }


      /// <summary>Populates the library from the syntax files</summary>
      /// <param name="data">Background worker data</param>
      /// <returns>Commands enumerated</returns>
      /// <exception cref="Logic::FileFormatException">Missing syntax component</exception>
      /// <exception cref="Logic::InvalidValueException">Unknown command group / parameter type</exception>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      UINT  SyntaxLibrary::Enumerate(WorkerData* data)
      {
         const Path path(L"D:\\My Projects\\XStudio2\\XStudio2\\Command Syntax.txt");

         // Clear previous contents
         Clear();

         // Feedback
         data->SendFeedback(Cons::Heading, ProgressType::Operation, 1, L"Loading command syntax...");

         try
         {
            // TODO: Check for new format syntax file

            // Feedback
            Console << L"Reading legacy syntax file: " << path << "...";
         
            // Load/Merge legacy syntax file
            StreamPtr fs( new FileStream(path, FileMode::OpenExisting, FileAccess::Read) );
            Merge( LegacySyntaxReader(fs).ReadFile() );

            // Feedback
            data->SendFeedback(ProgressType::Info, 2, GuiString(L"Loaded legacy syntax file '%s'", path.FileName.c_str()));
            Console << Cons::Green << L"Success" << ENDL;
         }
         catch (ExceptionBase& e) {
            Console << Cons::Error << L"Failed: " << e.Message << ENDL;
            throw;
         }

         // Return commands count
         return Commands.size();
      }


      /// <summary>Finds syntax by ID</summary>
      /// <param name="id">command ID</param>
      /// <param name="ver">Game version</param>
      /// <returns></returns>
      /// <exception cref="Logic::SyntaxNotFoundException">Not found</exception>
      CommandSyntaxRef  SyntaxLibrary::Find(UINT id, GameVersion ver) const
      { 
         // Search all syntax with matching ID for a compatible version
         for (auto it = Commands.find(id); it != Commands.end() && it->first == id; ++it)
            if (it->second.Versions & (UINT)ver)
               return it->second;

         // Not found
         throw SyntaxNotFoundException(HERE, id, ver);
      }

      /// <summary>Get the collection of defined command groups</summary>
      /// <returns></returns>
      SyntaxLibrary::GroupCollection  SyntaxLibrary::GetGroups() const
      {
         return Groups;
      }

      /// <summary>Identify syntax from tokens, and separate parameters</summary>
      /// <param name="pos">First token</param>
      /// <param name="end">End of tokens</param>
      /// <param name="v">Game version</param>
      /// <param name="params">Parameter tokens</param>
      /// <returns>Syntax if found, otherwise sentinel syntax</returns>
      CommandSyntaxRef  SyntaxLibrary::Identify(TokenIterator& pos, const TokenIterator& end, GameVersion ver, TokenList& params) const
      {
         params.clear();
         return NameTree.Find(pos, end, ver, params);
      }

      /// <summary>Search for all syntax containing a given term</summary>
      /// <param name="str">Search term</param>
      /// <param name="ver">Game version</param>
      /// <returns>Array of matching Syntax</returns>
      CmdSyntaxArray  SyntaxLibrary::Query(const wstring& str, GameVersion ver) const
      {
         CmdSyntaxArray results;

         // Search commands
         for (const auto& pair : Commands)
         {
            CommandSyntaxRef syntax = pair.second;

            // Check compatibility. Check search term (if any)
            if (syntax.Group != CommandGroup::HIDDEN && syntax.IsCompatible(ver) 
                && (!str.length() || syntax.Text.find(str) != wstring::npos))
               results.push_back(&syntax);
         }

         return results;
      }

		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------
      
      /// <summary>Merges a syntax file into the library</summary>
      /// <param name="f">The file</param>
      void  SyntaxLibrary::Merge(SyntaxFile&& f)
      { 
         // Merge commands with commands collection
         for (auto pair : f.Commands)
            Commands.Add(pair.second);

         // Merge group IDs with groups set
         for (auto pair : f.Groups)
            Groups.insert(pair.second); 

         // Add commands to syntax tree lookup
         for (auto& pair : Commands)
         {
            CommandSyntaxRef syntax = pair.second;
            
            try
            {
               // Skip commands that are manually matched by parser
               if (syntax.ID == CMD_COMMENT || syntax.ID == CMD_COMMAND_COMMENT || syntax.ID == CMD_EXPRESSION)
                  continue;

               // Lex syntax string
               CommandLexer lex(syntax.Text);
               TokenArray   tokens;

               // Duplicate token array WITHOUT RefObj & RetVar
               for (const ScriptToken& t : lex.Tokens)
               {
                  if (t.Type != TokenType::Variable)
                     tokens.push_back(t);
                  else
                  {
                     // Lookup parameter by physical index marker
                     auto param = syntax.Parameters[t.Text[1]-48];

                     if (!param.IsRefObj() && !param.IsRetVar())
                        tokens.push_back(t);
                  }
               }

               // Insert command
               NameTree.Insert(syntax, tokens.begin(), tokens.end()); 
            }
            catch (ExceptionBase& e)
            {
               Console << Cons::Error << L"Unable to insert syntax: " << syntax.Text << L" : " << Cons::Yellow << e.Message << ENDL;
            }
         }
      }

   }
}

