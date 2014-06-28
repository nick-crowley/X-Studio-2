#pragma once
#include "CommandNode.h"

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         class CommandTree;

         
         /// <summary>Distinguishes variables and constants from their usage</summary>
         class ConstantIdentifier : public CommandNode::Visitor
         {
            // ------------------------ TYPES --------------------------
         protected:
            // --------------------- CONSTRUCTION ----------------------
         public:
            ConstantIdentifier(ScriptFile& s, ErrorArray& e);
            virtual ~ConstantIdentifier();
		 
            NO_COPY(ConstantIdentifier);	// Uncopyable
            NO_MOVE(ConstantIdentifier);	// Unmovable

            // ---------------------- ACCESSORS ------------------------			

            // ----------------------- MUTATORS ------------------------
         public:
            void VisitNode(CommandNode* n) override;

            // -------------------- REPRESENTATION ---------------------
         protected:
            ErrorArray& Errors;     // Errors collection
            ScriptFile& Script;     // Script file
         };
            
         /// <summary>Generates a ScriptCommand from a node</summary>
         class CommandGenerator : public CommandNode::Visitor
         {
            // ------------------------ TYPES --------------------------
         protected:
            // --------------------- CONSTRUCTION ----------------------
         public:
            CommandGenerator(ScriptFile& s, ErrorArray& e);
            virtual ~CommandGenerator();
		 
            NO_COPY(CommandGenerator);	// Uncopyable
            NO_MOVE(CommandGenerator);	// Unmovable

            // ---------------------- ACCESSORS ------------------------			

            // ----------------------- MUTATORS ------------------------
         public:
            void VisitNode(CommandNode* n) override;

            // -------------------- REPRESENTATION ---------------------
         protected:
            ErrorArray& Errors;     // Errors collection
            ScriptFile& Script;     // Script file
         };
            
         /// <summary>Verifies command execution type and parameters</summary>
         class CommandVerifier : public CommandNode::Visitor
         {
            // ------------------------ TYPES --------------------------
         protected:
            // --------------------- CONSTRUCTION ----------------------
         public:
            CommandVerifier(ScriptFile& s, ErrorArray& e);
            virtual ~CommandVerifier();
		 
            NO_COPY(CommandVerifier);	// Uncopyable
            NO_MOVE(CommandVerifier);	// Unmovable

            // ---------------------- ACCESSORS ------------------------			

            // ----------------------- MUTATORS ------------------------
         public:
            void VisitNode(CommandNode* n) override;

         protected:
            void VerifyParameter(CommandNode* n, const ScriptParameter& p, UINT index, ErrorArray& err);

            // -------------------- REPRESENTATION ---------------------
         protected:
            ErrorArray& Errors;     // Errors collection
            ScriptFile& Script;     // Script file
         };

         /// <summary>Finalizes linkage between nodes</summary>
         class LinkageFinalizer : public CommandNode::Visitor
         {
            // ------------------------ TYPES --------------------------
         protected:
            // --------------------- CONSTRUCTION ----------------------
         public:
            LinkageFinalizer(ErrorArray& e);
            virtual ~LinkageFinalizer();
		 
            NO_COPY(LinkageFinalizer);	// Uncopyable
            NO_MOVE(LinkageFinalizer);	// Unmovable

            // ---------------------- ACCESSORS ------------------------			

            // ----------------------- MUTATORS ------------------------
         public:
            void VisitNode(CommandNode* n) override;

            // -------------------- REPRESENTATION ---------------------
         protected:
            ErrorArray& Errors;     // Errors collection
         };

         /// <summary>Verifies the branching logic of nodes</summary>
         class LogicVerifier : public CommandNode::Visitor
         {
            // ------------------------ TYPES --------------------------
         protected:
            // --------------------- CONSTRUCTION ----------------------
         public:
            LogicVerifier(ErrorArray& e);
            virtual ~LogicVerifier();
		 
            NO_COPY(LogicVerifier);	// Uncopyable
            NO_MOVE(LogicVerifier);	// Unmovable

            // ---------------------- ACCESSORS ------------------------			

            // ----------------------- MUTATORS ------------------------
         public:
            void VisitNode(CommandNode* n) override;

            // -------------------- REPRESENTATION ---------------------
         protected:
            ErrorArray& Errors;     // Errors collection
         };
            
         /// <summary>Replaces macro commands with their non-macro equivilents</summary>
         class MacroExpander : public CommandNode::Visitor
         {
            // ------------------------ TYPES --------------------------
         protected:
            /// <summary>Generates iterator variable names</summary>
            class NameGenerator
            {
            public:
               /// <summary>Initializes a NameGenerator</summary>
               NameGenerator() : LastID(1)
               {}

               /// <summary>Generates another iterator variable name</summary>
               /// <returns></returns>
               wstring  GetNext()
               {
                  return VString(L"$XS.Iterator%d", LastID++);
               }

            protected:
               int  LastID;    // Last ID used
            };

            // --------------------- CONSTRUCTION ----------------------
         public:
            MacroExpander(ScriptFile& s, ErrorArray& e);
            virtual ~MacroExpander();
		 
            NO_COPY(MacroExpander);	// Uncopyable
            NO_MOVE(MacroExpander);	// Unmovable

            // ---------------------- ACCESSORS ------------------------			

            // ----------------------- MUTATORS ------------------------
         public:
            void VisitNode(CommandNode* n) override;

         protected:
            CommandNodePtr   ExpandCommand(CommandNode* n, const wstring& txt, GameVersion v);
            CommandNodeList  ExpandDimArray(CommandNode* n);
            CommandNodeList  ExpandForLoop(CommandNode* n);
            CommandNodeList  ExpandForEach(CommandNode* n);

            // -------------------- REPRESENTATION ---------------------
         protected:
            ErrorArray&   Errors;         // Errors collection
            ScriptFile&   Script;         // Script file
            NameGenerator IteratorNames;  // Iterate name generator
         };

         /// <summary>Assigns standard command indicies to nodes</summary>
         class NodeIndexer : public CommandNode::Visitor
         {
            // ------------------------ TYPES --------------------------
         protected:
            // --------------------- CONSTRUCTION ----------------------
         public:
            NodeIndexer(UINT& init);
            virtual ~NodeIndexer();
		 
            NO_COPY(NodeIndexer);	// Uncopyable
            NO_MOVE(NodeIndexer);	// Unmovable

            // ---------------------- ACCESSORS ------------------------			

            // ----------------------- MUTATORS ------------------------
         public:
            void VisitNode(CommandNode* n) override;

            // -------------------- REPRESENTATION ---------------------
         protected:
            UINT&  NextIndex; // Next available index
         };

         /// <summary>Performs linking between nodes</summary>
         class NodeLinker : public CommandNode::Visitor
         {
            // ------------------------ TYPES --------------------------
         protected:
            // --------------------- CONSTRUCTION ----------------------
         public:
            NodeLinker(ErrorArray& e);
            virtual ~NodeLinker();
		 
            NO_COPY(NodeLinker);	// Uncopyable
            NO_MOVE(NodeLinker);	// Unmovable

            // ---------------------- ACCESSORS ------------------------			

            // ----------------------- MUTATORS ------------------------
         public:
            void VisitNode(CommandNode* n) override;

            // -------------------- REPRESENTATION ---------------------
         protected:
            ErrorArray& Errors;     // Errors collection
         };

         /// <summary>Prints nodes to the console</summary>
         class NodePrinter : public CommandNode::Visitor
         {
            // ------------------------ TYPES --------------------------
         protected:
            // --------------------- CONSTRUCTION ----------------------
         public:
            NodePrinter(const CommandTree& t);
            virtual ~NodePrinter();
		 
            NO_COPY(NodePrinter);	// Uncopyable
            NO_MOVE(NodePrinter);	// Unmovable

            // ---------------------- ACCESSORS ------------------------			
         protected:
            UINT GetDepth(const CommandNode* n) const;

            // ----------------------- MUTATORS ------------------------
         public:
            void VisitNode(CommandNode* n) override;
            void VisitRoot(CommandNode* n) override;

            // -------------------- REPRESENTATION ---------------------
         protected:
            const CommandTree& Tree;
         };
            
         /// <summary>Searches for all instances of a symbol</summary>
         class SymbolSearcher : public CommandNode::Visitor
         {
            // ------------------------ TYPES --------------------------
         protected:
            // --------------------- CONSTRUCTION ----------------------
         public:
            SymbolSearcher(const wstring& name, SymbolType type, SymbolList& results);
            virtual ~SymbolSearcher();
		 
            NO_COPY(SymbolSearcher);	// Uncopyable
            NO_MOVE(SymbolSearcher);	// Unmovable

            // ---------------------- ACCESSORS ------------------------			

            // ----------------------- MUTATORS ------------------------
         public:
            void VisitNode(CommandNode* n) override;

            // -------------------- REPRESENTATION ---------------------
         protected:
            SymbolList& Results;     // Errors collection
            wstring     Name;
            SymbolType  Type;
         };

         /// <summary>Verifies that all control paths lead to termination</summary>
         class TerminationVerifier : public CommandNode::Visitor
         {
            // ------------------------ TYPES --------------------------
         protected:
            // --------------------- CONSTRUCTION ----------------------
         public:
            TerminationVerifier(ErrorArray& e);
            virtual ~TerminationVerifier();
		 
            NO_COPY(TerminationVerifier);	// Uncopyable
            NO_MOVE(TerminationVerifier);	// Unmovable

            // ---------------------- ACCESSORS ------------------------			

            // ----------------------- MUTATORS ------------------------
         public:
            void VisitNode(CommandNode* n) override;

            // -------------------- REPRESENTATION ---------------------
         protected:
            ErrorArray& Errors;     // Errors collection
         };

         /// <summary>Maps each variable name to a unique ID, and locates all label definitions</summary>
         class VariableIdentifier : public CommandNode::Visitor
         {
            // ------------------------ TYPES --------------------------
         protected:
            // --------------------- CONSTRUCTION ----------------------
         public:
            VariableIdentifier(ScriptFile& s, ErrorArray& e);
            virtual ~VariableIdentifier();
		 
            NO_COPY(VariableIdentifier);	// Uncopyable
            NO_MOVE(VariableIdentifier);	// Unmovable

            // ---------------------- ACCESSORS ------------------------			

            // ----------------------- MUTATORS ------------------------
         public:
            void VisitNode(CommandNode* n) override;

            // -------------------- REPRESENTATION ---------------------
         protected:
            ErrorArray& Errors;     // Errors collection
            ScriptFile& Script;     // Script file
         };
      
   
         
   
      }
      
   
   }
}

using namespace Logic::Scripts::Compiler;









            