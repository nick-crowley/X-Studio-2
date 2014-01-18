#pragma once
#include "Common.h"
#include "SyntaxFile.h"
#include "CommandHash.h"
#include "ScriptToken.h"

// Syntax library singleton
#define SyntaxLib SyntaxLibrary::Instance

namespace Logic
{
   namespace Scripts
   {
         
      /// <summary></summary>
      class SyntaxLibrary
      {
         // ------------------------ TYPES --------------------------
      private:
         /// <summary></summary>
         typedef multimap<wstring,CommandSyntax>  HashCollection;

         /// <summary></summary>
         class SyntaxNode
         {
            // ------------------------ TYPES --------------------------
         private:
            /// <summary>Sentinel value for a parameter</summary>
            const wstring VARIABLE = L"¶¶¶¶¶";

            /// <summary>Nodes keyed by token text</summary>
            typedef map<wstring, SyntaxNode> NodeMap;

            /// <summary>Shared pointer to a command syntax</summary>
            typedef shared_ptr<CommandSyntax> SyntaxPtr;
         
            // --------------------- CONSTRUCTION ----------------------
         public:
            SyntaxNode() : Syntax(nullptr)
            {}
            SyntaxNode(const ScriptToken& t) : Token(t), Syntax(nullptr)
            {}

            // ------------------------ STATIC -------------------------

            // --------------------- PROPERTIES ------------------------
			
            // ---------------------- ACCESSORS ------------------------			

            CommandSyntax Find(list<ScriptToken>& tokens) const
            {
               // Empty: Return syntax / sentinel
               if (tokens.empty())
                  return GetSyntax();

               // Lookup next token
               auto pair = Children.find( GetKey(tokens.front()) );
               
               // Not found: Return sentinel
               if (pair == Children.end())
                  return SyntaxLib.Unknown;

               // Found: Search children
               tokens.pop_front();
               return pair->second.Find(tokens);
            };

            const wstring& GetKey(const ScriptToken& tok) const
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
            
            CommandSyntax  GetSyntax() const
            {
               return HasSyntax() ? *Syntax : SyntaxLib.Unknown;
            }

            bool HasSyntax() const
            {
               return Syntax != nullptr;
            }

            void Print(int depth = 1) const
            {
               if (depth == 3)
                  return;
               wstring indent(depth, L' ');
               Console << indent << L"Token: " << Colour::Yellow << Token.Text << Colour::White << L"   Syntax: " << Colour::Yellow << GetSyntax().Text << ENDL;
               for (auto c : Children)
                  c.second.Print(depth+1);
            }

            // ----------------------- MUTATORS ------------------------

            void  Add(const CommandSyntax& s, list<ScriptToken>& tokens)
            {
               // Empty: Store syntax at this node
               if (tokens.empty())
               {
                  if (HasSyntax())
                     throw ArgumentException(HERE, L"s", GuiString(L"Syntax already present: (id:%d) %s", Syntax->ID, Syntax->Text.c_str()));

                  // Store syntax 
                  Syntax = SyntaxPtr(new CommandSyntax(s));
               }
               else
               {
                  // (Insert/Lookup) next token/child
                  auto t = tokens.front();
                  auto pair = Children.insert( NodeMap::value_type(GetKey(t), SyntaxNode(t)) );

                  // Remove token, recurse into new/existing child
                  tokens.pop_front();
                  pair.first->second.Add(s, tokens);
               }
            }

            // -------------------- REPRESENTATION ---------------------

            NodeMap        Children;
            SyntaxPtr      Syntax;
            ScriptToken    Token;
         };

         // --------------------- CONSTRUCTION ----------------------

      private:
         SyntaxLibrary();
      public:
         virtual ~SyntaxLibrary();

         // ------------------------ STATIC -------------------------

      public:
         static SyntaxLibrary  Instance;

         // --------------------- PROPERTIES ------------------------
			
		   // ---------------------- ACCESSORS ------------------------			

      public:
         /// <summary>Finds syntax by ID</summary>
         /// <param name="id">command ID</param>
         /// <param name="ver">Game version</param>
         /// <returns></returns>
         /// <exception cref="Logic::SyntaxNotFoundException">Not found</exception>
         CommandSyntax  Find(UINT id, GameVersion ver) const;

         /// <summary>Finds syntax by Hash</summary>
         /// <param name="h">command hash</param>
         /// <param name="ver">Game version</param>
         /// <returns>Syntax if found, otherwise 'Unknown'</returns>
         CommandSyntax  Identify(const CommandHash& h, GameVersion v) const;

         /// <summary>Merges a syntax file into the library</summary>
         /// <param name="f">The file</param>
         void  Merge(SyntaxFile&& f);

		   // ----------------------- MUTATORS ------------------------

		   // -------------------- REPRESENTATION ---------------------

      public:
         const CommandSyntax  Unknown;

      private:
         SyntaxCollection  Commands;
         HashCollection    Hashes;
         SyntaxNode        Tree;
      };

   }
}



using namespace Logic::Scripts;



