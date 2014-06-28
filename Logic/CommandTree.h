#pragma once
#include "CommandNode.h"

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         /// <summary></summary>
         class CommandTree
         {
            // ------------------------ TYPES --------------------------
         protected:
            
            // --------------------- CONSTRUCTION ----------------------
         public:
            CommandTree();
            virtual ~CommandTree();

            DEFAULT_COPY(CommandTree);	// Default copy semantics
            DEFAULT_MOVE(CommandTree);	// Default move semantics

            // ------------------------ STATIC -------------------------

            // --------------------- PROPERTIES ------------------------
         public:
            PROPERTY_GET(bool,Empty,IsEmpty);

            // ---------------------- ACCESSORS ------------------------		
         public:
            /// <summary>Get start iterator for tree</summary>
            /// <typeparam name="T">Traversal type</typeparam>
            /// <returns></returns>
            template <typename T>
            CommandNode::Iterator<T> begin() const
            {
               return CommandNode::Iterator<T>(FindRoot(), const_cast<CommandNode*>(this));
            }

            /// <summary>Get finish iterator for tree</summary>
            /// <typeparam name="T">Traversal type</typeparam>
            /// <returns></returns>
            template <typename T>
            CommandNode::Iterator<T> end() const
            {
               return CommandNode::Iterator<T>(FindRoot());
            }

            /// <summary>Get start iterator for tree</summary>
            /// <returns></returns>
            CommandNode::DepthIterator begin() const
            {
               return Root->begin();
            }

            /// <summary>Get finish iterator for tree</summary>
            /// <returns></returns>
            CommandNode::DepthIterator end() const
            {
               return Root->begin();
            }

            void  FindAll(const wstring& name, SymbolType type, SymbolList& results) const;
            bool  IsEmpty() const;
            void  Print(int depth = 0);
            void  ToList(CommandNodeList& l) const;

            // ----------------------- MUTATORS ------------------------
         public:
            CommandNodePtr Add(CommandNodePtr node);
            void           Compile(ScriptFile& script, ErrorArray& errors);
            void           Transform(CommandNode::Visitor& v);
            void           Verify(ScriptFile& script, ErrorArray& errors);
            
            // -------------------- REPRESENTATION ---------------------
         protected:
            CommandNodePtr  Root;            // Parse tree
         };
      }
   }
}

using namespace Logic::Scripts;
