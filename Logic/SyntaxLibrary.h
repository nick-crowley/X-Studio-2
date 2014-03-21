#pragma once

#include "SyntaxFile.h"
#include "ScriptToken.h"
#include "BackgroundWorker.h"

// Syntax library singleton
#define SyntaxLib SyntaxLibrary::Instance

namespace Logic
{
   namespace Scripts
   {
      /// <summary></summary>
      class LogicExport SyntaxLibrary
      {
         // ------------------------ TYPES --------------------------
      public:
         /// <summary>User customized Command group collection</summary>
         typedef set<CommandGroup>  GroupCollection;

         /// <summary>Command syntax collection organised by ID</summary>
         class CommandCollection : public multimap<UINT, CommandSyntax>
         {
         public:
            void  Add(CommandSyntax& s) 
            { 
               insert(value_type(s.ID, s)); 
            }
         };
      
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
            typedef shared_ptr<CommandSyntax> CommandSyntaxPtr;

            // --------------------- CONSTRUCTION ----------------------
         public:
            SyntaxNode();
            SyntaxNode(const ScriptToken& t);

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
         GroupCollection   GetGroups() const;
         CommandSyntaxRef  Find(UINT id, GameVersion ver) const;
         CommandSyntaxRef  Identify(TokenIterator& pos, const TokenIterator& end, GameVersion ver, TokenList& params) const;
         CmdSyntaxArray    Query(const wstring& str, GameVersion ver, CommandGroup g = (CommandGroup)CB_ERR) const;

		   // ----------------------- MUTATORS ------------------------
      public:
         void  Clear();
         UINT  Enumerate(WorkerData* data);

      private:
         void  Merge(SyntaxFile&& f);

		   // -------------------- REPRESENTATION ---------------------

      private:
         CommandCollection  Commands;
         GroupCollection    Groups;
         SyntaxNode         NameTree;
      };

   }
}



using namespace Logic::Scripts;



