#include "stdafx.h"
#include "SyntaxTree.h"
#include "CommandLexer.h"

namespace Logic
{
   namespace Scripts
   {

      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Create root</summary>
      SyntaxTree::SyntaxTree() : Syntax(nullptr)
      {
      }

      /// <summary>Create tree node</summary>
      /// <param name="t">Token in the command syntax associated with this command</param>
      SyntaxTree::SyntaxTree(const ScriptToken& t) : Token(t), Syntax(nullptr)
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------
      
      /// <summary>Clears syntax tree</summary>
      void  SyntaxTree::Clear()
      {
         Children.clear();
         Syntax = nullptr;
      }

      /// <summary>Finds syntax by text</summary>
      /// <param name="pos">First token</param>
      /// <param name="end">End of tokens</param>
      /// <param name="ver">Desired game version</param>
      /// <param name="params">Parameter tokens</param>
      /// <returns>Requested syntax if found/compatible, otherwise sentinel syntax</returns>
      CommandSyntaxRef SyntaxTree::Find(TokenIterator& pos, const TokenIterator& end, GameVersion ver, TokenList& params) const
      {
         // EOF: Return syntax @ this node (if any)
         if (pos >= end)
            return HasSyntax() && Syntax->IsCompatible(ver) ? *Syntax : CommandSyntax::Unrecognised;

         // Variable Arguments: Terminate early to prevent analysing arguments
         else if (HasSyntax() && Syntax->IsVArgument())
            return *Syntax;

         // PARAM: Store token
         if (pos->IsParameter())
            params.push_back(*pos);

         // Lookup next token
         auto pair = Children.find( GetKey(*pos) );
               
         // Not found: Return sentinel
         if (pair == Children.end())
            return CommandSyntax::Unrecognised;

         // Found: Search children
         return pair->second.Find(++pos, end, ver, params);
      };

      /// <summary>Inserts new syntax into the tree</summary>
      /// <param name="s">The syntax</param>
      /// <param name="pos">First token</param>
      /// <param name="end">End of tokens</param>
      /// <exception cref="Logic::AlgorithmException">Syntax conflicts with existing syntax</exception>
      void  SyntaxTree::Insert(CommandSyntaxRef s, TokenIterator& pos, const TokenIterator& end)
      {
         if (pos < end)
         {
            // (Insert/Lookup) next child
            auto elem = Children.insert( NodeMap::value_type(GetKey(*pos), SyntaxTree(*pos)) ).first;

            // Check child against next token
            elem->second.Insert(s, ++pos, end);
         }
         else
         {
            // Ensure not duplicate
            if (HasSyntax())
               throw AlgorithmException(HERE, VString(L"The command syntax '%s' (id:%d) is already present", Syntax->Text.c_str(), Syntax->ID));

            // EndOfInput: Store syntax here
            Syntax = CommandSyntaxPtr(new CommandSyntax(s));
         }
      }

      /// <summary>Prints node and all children to the console</summary>
      /// <param name="depth">Current depth</param>
      void SyntaxTree::Print(int depth) const
      {
         // DEBUG: Abort
         if (depth == 3)
            return;

         // Print node
         Console << Indent(depth) << L"Token: " << Cons::Yellow << Token.Text << Cons::White << L"   Syntax: " << Cons::Yellow << GetSyntax().Text << ENDL;

         // Print children
         for (auto c : Children)
            c.second.Print(depth+1);
      }

      // ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

      /// <summary>Gets the node map lookup key for a token</summary>
      /// <param name="tok">The token</param>
      /// <returns></returns>
      const wstring&  SyntaxTree::GetKey(const ScriptToken& tok) const
      {
         return tok.IsParameter() ? VARIABLE : tok.Text;
      }

      /// <summary>Get the syntax at this node, if any, otherwise sentinel syntax</summary>
      /// <returns></returns>
      CommandSyntaxRef  SyntaxTree::GetSyntax() const
      {
         return HasSyntax() ? *Syntax : CommandSyntax::Unrecognised;
      }

      /// <summary>Determine whether node has syntax</summary>
      /// <returns></returns>
      bool  SyntaxTree::HasSyntax() const
      {
         return Syntax != nullptr;
      }
   }
}

