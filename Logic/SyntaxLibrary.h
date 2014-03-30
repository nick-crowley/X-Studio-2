#pragma once

#include "SyntaxFile.h"
#include "SyntaxTree.h"
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
            /// <summary>Adds command syntax</summary>
            /// <param name="s">The syntax</param>
            void  Add(CommandSyntax& s)
            { 
               insert(value_type(s.ID, s)); 
            }
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
         void              Upgrade(const Path& legacy, const Path& upgrade, bool merge) const;

		   // ----------------------- MUTATORS ------------------------
      public:
         void  Clear();
         UINT  Enumerate(WorkerData* data);

      private:
         void  Add(SyntaxFile& f);

		   // -------------------- REPRESENTATION ---------------------

      private:
         CommandCollection  Commands;
         GroupCollection    Groups;
         SyntaxTree         NameTree;
      };

   }
}



using namespace Logic::Scripts;



