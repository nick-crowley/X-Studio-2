#pragma once
#include "Common.h"
#include "SyntaxFile.h"
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
      public:
         /// <summary>Command syntax array</summary>
         typedef vector<const CommandSyntax*>  ResultCollection;
      
      private:
         /// <summary></summary>
         class SyntaxNode
         {
            // ------------------------ TYPES --------------------------
         private:
            /// <summary>Sentinel node map key for a parameter</summary>
            const wstring VARIABLE = L"¶¶¶¶¶";

            /// <summary>Nodes keyed by token text</summary>
            typedef map<wstring, SyntaxNode> NodeMap;

            /// <summary>Shared pointer to a command syntax</summary>
            typedef shared_ptr<CommandSyntax> SyntaxPtr;
         
            // --------------------- CONSTRUCTION ----------------------
         public:
            SyntaxNode();
            SyntaxNode(const ScriptToken& t);

            // ------------------------ STATIC -------------------------

            // --------------------- PROPERTIES ------------------------
			
            // ---------------------- ACCESSORS ------------------------			

         public:
            CommandSyntax  Find(TokenIterator& pos, const TokenIterator& end, GameVersion ver) const;
            void           Print(int depth = 1) const;

         private:
            const wstring& GetKey(const ScriptToken& tok) const;
            CommandSyntax  GetSyntax() const;
            bool           HasSyntax() const;

            // ----------------------- MUTATORS ------------------------

         public:
            void  Insert(const CommandSyntax& s, TokenIterator& pos, const TokenIterator& end);

            // -------------------- REPRESENTATION ---------------------

            NodeMap      Children;
            SyntaxPtr    Syntax;
            ScriptToken  Token;
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

         /// <summary>Finds syntax by name</summary>
         /// <param name="pos">First token</param>
         /// <param name="end">End of tokens</param>
         /// <param name="v">Game version</param>
         /// <returns>Syntax if found, otherwise sentinel syntax</returns>
         CommandSyntax  Identify(TokenIterator& pos, const TokenIterator& end, GameVersion ver) const;

         /// <summary>Search for all syntax containing a given term</summary>
         /// <param name="str">Search term</param>
         /// <param name="ver">Game version</param>
         /// <returns>Array of matching Syntax</returns>
         ResultCollection  Query(const wstring& str, GameVersion ver) const;

         /// <summary>Merges a syntax file into the library</summary>
         /// <param name="f">The file</param>
         void  Merge(SyntaxFile&& f);

		   // ----------------------- MUTATORS ------------------------

		   // -------------------- REPRESENTATION ---------------------

      public:
         const CommandSyntax  Unknown;

      private:
         SyntaxCollection  Commands;
         SyntaxNode        NameTree;
      };

   }
}



using namespace Logic::Scripts;



