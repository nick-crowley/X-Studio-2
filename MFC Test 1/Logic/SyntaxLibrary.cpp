#include "stdafx.h"
#include "SyntaxLibrary.h"
#include "CommandLexer.h"

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

      /// <summary>Finds syntax by ID</summary>
      /// <param name="id">command ID</param>
      /// <param name="ver">Game version</param>
      /// <returns></returns>
      /// <exception cref="Logic::SyntaxNotFoundException">Not found</exception>
      CommandSyntax  SyntaxLibrary::Find(UINT id, GameVersion ver) const
      { 
         // Search all syntax with matching ID for a compatible version
         for (auto it = Commands.find(id); it != Commands.end() && it->first == id; ++it)
            if (it->second.Versions & (UINT)ver)
               return it->second;

         // Not found
         throw SyntaxNotFoundException(HERE, id, ver);
      }

      /// <summary>Finds syntax by name</summary>
      /// <param name="pos">First token</param>
      /// <param name="end">End of tokens</param>
      /// <param name="v">Game version</param>
      /// <returns>Syntax if found, otherwise sentinel syntax</returns>
      CommandSyntax  SyntaxLibrary::Identify(TokenIterator& pos, const TokenIterator& end, GameVersion ver) const
      {
         return NameTree.Find(pos, end, ver);
      }

      /// <summary>Merges a syntax file into the library</summary>
      /// <param name="f">The file</param>
      void  SyntaxLibrary::Merge(SyntaxFile&& f)
      { 
         // Merge commands table
         Commands.Merge(std::move(f)); 

         // Insert into syntax tree
         for (auto& pair : Commands)
         {
            CommandSyntax& syntax = pair.second;
            
            try
            {
               // Skip commands that are manually matched by parser
               if (syntax.ID == CMD_COMMENT || syntax.ID == CMD_COMMAND_COMMENT || syntax.ID == CMD_EXPRESSION)
                  continue;

               // Lex syntax string
               CommandLexer lex(syntax.Text);
               TokenArray tokens;

               // Copy token array excluding RefObj & RetVar
               for (const ScriptToken& t : lex.Tokens)
               {
                  if (t.Type != TokenType::Variable)
                     tokens.push_back(t);
                  else
                  {
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
               Console << Colour::Red << L"Unable to insert syntax: " << syntax.Text << L" : " << Colour::Yellow << e.Message << ENDL;
            }
         }
      }

      /// <summary>Search for all syntax containing a given term</summary>
      /// <param name="str">Search term</param>
      /// <param name="ver">Game version</param>
      /// <returns>Array of matching Syntax</returns>
      SyntaxLibrary::ResultCollection  SyntaxLibrary::Query(const wstring& str, GameVersion ver) const
      {
         ResultCollection results;

         // Search commands
         for (const auto& pair : Commands)
         {
            const CommandSyntax& syntax = pair.second;

            // Check compatibility. Check search term (if any)
            if (syntax.Group != CommandGroup::HIDDEN 
             && syntax.IsCompatible(ver) 
             && (!str.length() || syntax.Text.find(str) != wstring::npos))
               results.push_back(&syntax);
         }

         return results;
      }

		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------




      // -------------------------------- CONSTRUCTION --------------------------------

      SyntaxLibrary::SyntaxNode::SyntaxNode() : Syntax(nullptr)
      {
      }

      SyntaxLibrary::SyntaxNode::SyntaxNode(const ScriptToken& t) : Token(t), Syntax(nullptr)
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------
      
      /// <summary>Finds syntax by text</summary>
      /// <param name="pos">First token</param>
      /// <param name="end">End of tokens</param>
      /// <param name="ver">Desired game version</param>
      /// <returns>Requested syntax if found/compatible, otherwise sentinel syntax</returns>
      CommandSyntax SyntaxLibrary::SyntaxNode::Find(TokenIterator& pos, const TokenIterator& end, GameVersion ver) const
      {
         // EOF: Return of this node (if any)
         if (pos >= end)
            return HasSyntax() && Syntax->IsCompatible(ver) ? *Syntax : SyntaxLib.Unknown;

         // Lookup next token
         auto pair = Children.find( GetKey(*pos) );
               
         // Not found: Return sentinel
         if (pair == Children.end())
            return SyntaxLib.Unknown;

         // Found: Search children
         return pair->second.Find(++pos, end, ver);
      };

      /// <summary>Inserts new syntax into the tree</summary>
      /// <param name="s">The syntax</param>
      /// <param name="pos">First token</param>
      /// <param name="end">End of tokens</param>
      /// <exception cref="Logic::GenericException">Syntax conflicts with existing syntax</exception>
      void  SyntaxLibrary::SyntaxNode::Insert(const CommandSyntax& s, TokenIterator& pos, const TokenIterator& end)
      {
         if (pos < end)
         {
            // (Insert/Lookup) next child
            auto pair = Children.insert( NodeMap::value_type(GetKey(*pos), SyntaxNode(*pos)) );

            // Check child against next token
            pair.first->second.Insert(s, ++pos, end);
         }
         else
         {
            // Ensure not duplicate
            if (HasSyntax())
               throw GenericException(HERE, GuiString(L"The command syntax '%s' (id:%d) is already present", Syntax->Text.c_str(), Syntax->ID));

            // EndOfInput: Store syntax here
            Syntax = SyntaxPtr(new CommandSyntax(s));
         }
      }

      /// <summary>Prints node and all children to the console</summary>
      /// <param name="depth">Current depth</param>
      void SyntaxLibrary::SyntaxNode::Print(int depth) const
      {
         // DEBUG: Abort
         if (depth == 3)
            return;

         // Print node
         wstring indent(depth, L' ');
         Console << indent << L"Token: " << Colour::Yellow << Token.Text << Colour::White << L"   Syntax: " << Colour::Yellow << GetSyntax().Text << ENDL;

         // Print children
         for (auto c : Children)
            c.second.Print(depth+1);
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

      /// <summary>Gets the node map lookup key for a token</summary>
      /// <param name="tok">The token</param>
      /// <returns></returns>
      const wstring&  SyntaxLibrary::SyntaxNode::GetKey(const ScriptToken& tok) const
      {
         switch (tok.Type)
         {
         case TokenType::Operator:  
         case TokenType::Keyword:  
         case TokenType::Text:      
            return tok.Text;

         default: 
            return VARIABLE;
         }
      }

      /// <summary>Get the syntax at this node, if any, otherwise sentinel syntax</summary>
      /// <returns></returns>
      CommandSyntax  SyntaxLibrary::SyntaxNode::GetSyntax() const
      {
         return HasSyntax() ? *Syntax : SyntaxLib.Unknown;
      }

      /// <summary>Determine whether node has syntax</summary>
      /// <returns></returns>
      bool  SyntaxLibrary::SyntaxNode::HasSyntax() const
      {
         return Syntax != nullptr;
      }
   }
}

