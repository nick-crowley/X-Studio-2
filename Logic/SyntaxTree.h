#pragma once
#include "ScriptToken.h"
#include "CommandSyntax.h"

namespace Logic
{
   namespace Scripts
   {
      /// <summary></summary>
      class SyntaxTree
      {
         // ------------------------ TYPES --------------------------
      private:
         /// <summary>Sentinel node map key for a parameter</summary>
         const wstring VARIABLE = L"¶¶¶¶¶";

         /// <summary>Nodes keyed by token text</summary>
         typedef map<wstring, SyntaxTree> NodeMap;

         /// <summary>Shared pointer to a command syntax</summary>
         typedef shared_ptr<CommandSyntax> CommandSyntaxPtr;

         // --------------------- CONSTRUCTION ----------------------
      public:
         SyntaxTree();
         SyntaxTree(const ScriptToken& t);

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
			
         // ---------------------- ACCESSORS ------------------------			
      public:
         CommandSyntaxRef Find(TokenIterator& pos, const TokenIterator& end, GameVersion ver, TokenList& params) const;
         void             Print(int depth = 1) const;

      private:
         const wstring&   GetKey(const ScriptToken& tok) const;
         CommandSyntaxRef GetSyntax() const;
         bool             HasSyntax() const;

         // ----------------------- MUTATORS ------------------------
      public:
         void  Clear();
         void  Insert(CommandSyntaxRef s, TokenIterator& pos, const TokenIterator& end);

         // -------------------- REPRESENTATION ---------------------
      private:
         NodeMap           Children;
         CommandSyntaxPtr  Syntax;
         ScriptToken       Token;
      };

   }
}



using namespace Logic::Scripts;



