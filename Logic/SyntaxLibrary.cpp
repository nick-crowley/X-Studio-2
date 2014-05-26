#include "stdafx.h"
#include "SyntaxLibrary.h"
#include "CommandLexer.h"
#include "FileStream.h"
#include "LegacySyntaxFileReader.h"
#include "SyntaxFileReader.h"
#include "SyntaxFileWriter.h"

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
      /// <exception cref="Logic::InvalidValueException">Invalid value in syntax file</exception>
      /// <exception cref="Logic::IOException">An I/O error occurred</exception>
      UINT  SyntaxLibrary::Enumerate(WorkerData* data)
      {
         try
         {
            SyntaxFile file;

            // Clear previous contents
            Clear();

            // Feedback
            data->SendFeedback(Cons::Heading, ProgressType::Operation, 1, L"Loading command syntax...");

            // Ensure syntax/legacy file exists
            if (!AppPath(L"Data\\Commands.xml").Exists() && AppPath(L"Data\\Command Syntax.txt").Exists())
               throw FileNotFoundException(HERE, AppPath(L"Data\\Commands.xml"));

            // Use new format file if present
            AppPath path(L"Data\\Commands.xml");
            if (!path.Exists())
               path = AppPath(L"Data\\Command Syntax.txt");
            
            // Load file
            Console << L"Reading syntax file: " << path << "...";
            StreamPtr fs( new FileStream(path, FileMode::OpenExisting, FileAccess::Read) );

            // Use appropriate parser
            if (path.HasExtension(L".xml"))
               Add( file = SyntaxFileReader(fs).ReadFile() );
            else
               Add( file = LegacySyntaxFileReader(fs).ReadFile() );

            // Feedback
            data->SendFeedback(ProgressType::Info, 2, VString(L"Loaded '%s'", file.GetIdent().c_str()));
            Console << Cons::Success << Cons::White << file.GetIdent() << ENDL;

            // Feedback
            Console << "Read " << Commands.size() << " commands" << ENDL;
         }
         catch (ExceptionBase& e) {
            Console << Cons::Failure << e.Message << ENDL;
            throw;
         }

         // Return commands count
         return Commands.size();
      }

      /// <summary>Upgrades a legacy custom syntax file into new format.</summary>
      /// <param name="legacy">legacy file path.</param>
      /// <param name="upgrade">new file path.</param>
      /// <param name="merge">Whether to merge library contents with legacy file</param>
      /// <exception cref="Logic::FileNotFoundException">Legacy file not found</exception>
      /// <exception cref="Logic::ComException">COM Error</exception>
      /// <exception cref="Logic::IOException">IO error</exception>
      void  SyntaxLibrary::Upgrade(const Path& legacy, const Path& upgrade, bool merge) const
      {
         // Feedback
         Console << "Upgrading legacy commands " << legacy << " to " << upgrade << " merge=" << merge << ENDL;

         // Read legacy
         StreamPtr fs( new FileStream(legacy, FileMode::OpenExisting, FileAccess::Read) );
         auto in = LegacySyntaxFileReader(fs).ReadFile();

         // MERGE: Add existing commands
         if (merge)
            for (auto& cmd : Commands)
               in.Commands.Add(cmd.second);

         // Feedback
         Console << "Writing upgraded syntax..." << ENDL;

         // Write upgraded
         StreamPtr out( new FileStream(upgrade, FileMode::CreateAlways, FileAccess::Write) );
         SyntaxFileWriter w(out);
         w.Write(in, L"Upgraded Custom Syntax", L"v1.0");
         w.Close();
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
            if (it->second.IsCompatible(ver))
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
      /// <param name="g">Command group, or CB_ERR for all groups</param>
      /// <returns>Array of matching Syntax</returns>
      CmdSyntaxArray  SyntaxLibrary::Query(const wstring& str, GameVersion ver, CommandGroup g /*= CB_ERR*/) const
      {
         CmdSyntaxArray results;
         bool  hasQuery = !str.empty(),
               hasGroup = ((int)g != CB_ERR);

         // Search commands
         for (const auto& pair : Commands)
         {
            CommandSyntaxRef syntax = pair.second;

            // Check compatibility
            if (syntax.Group == CommandGroup::HIDDEN || !syntax.IsCompatible(ver))
               continue;

            // Check group (if any)
            if (hasGroup && syntax.Group != g)
               continue;

            // Check text (if any)
            if (hasQuery && !syntax.Text.Contains(str, false))
               continue;

            // Success
            results.push_back(&syntax);
         }

         return results;
      }

		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------
      
      /// <summary>Merges a syntax file into the library</summary>
      /// <param name="f">The file</param>
      void  SyntaxLibrary::Add(SyntaxFile& f)
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
            
            // Skip commands that are manually matched by parser
            if (syntax.ID == CMD_NOP || syntax.ID == CMD_COMMENT || syntax.ID == CMD_COMMAND_COMMENT || syntax.ID == CMD_EXPRESSION)
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

            // Attempt Insert command [throws if duplicate]
            NameTree.Insert(syntax, tokens.begin(), tokens.end()); 
         }
      }

   }
}

