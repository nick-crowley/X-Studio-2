#pragma once
#include "TreeTraversal.h"

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
         public:
            /// <summary>Forward-only iterator for nodes in a CommandNode</summary>
            template <typename TRAVERSAL>
            class Iterator : public std::iterator<std::forward_iterator_tag, CommandNodePtr>
            {
               friend class CommandTree;
               
               // ------------------------ TYPES --------------------------
            protected:
               
               // --------------------- CONSTRUCTION ----------------------
            protected:
               /// <summary>Creates an end iterator</summary>
               /// <param name="t">Tree</param>
               Iterator(const CommandTree& t) 
                  : Tree(&t), Position(nullptr)
               {
               }

               /// <summary>Creates a start iterator</summary>
               /// <param name="t">Tree</param>
               /// <param name="n">Start node</param>
               /// <exception cref="Logic::ArgumentNullException">start node is nullptr</exception>
               Iterator(const CommandTree& t, CommandNodePtr& n) 
                  : Tree(&t), Position(n)
               {
                  REQUIRED(n);

                  // Add children of start node
                  Traversal.AddSuccessors(Position);
               }

            public:
               /// <summary>Copy create from an existing iterator</summary>
               /// <param name="r">Another iterator</param>
               Iterator(const Iterator& r)
                  : Tree(r.Tree), 
                    Position(r.Position), 
                    Traversal(r.Traversal)
               {
               }
         
               /// <summary>Move-create from an existing iterator</summary>
               /// <param name="r">Another iterator</param>
               Iterator(Iterator&& r)
                  : Tree(r.Tree), 
                    Position(r.Position), 
                    Traversal(std::ref(r.Traversal))
               {
                  r.Position = nullptr;
               }

               virtual ~Iterator()
               {
               }
               
               // ------------------------ STATIC -------------------------

               // --------------------- PROPERTIES ------------------------
			   
               // ---------------------- ACCESSORS ------------------------			
            public:
               /// <summary>Get reference to current node</summary>
               /// <returns></returns>
               /// <exception cref="Logic::InvalidOperationException">Iterator cannot be dereferenced</exception>
               CommandNodePtr& operator*() const
               { 
                  if (!Position)
                     throw InvalidOperationException(HERE, L"Cannot dereference iterator");

                  return Position; 
               }

               /// <summary>Get pointer to current node</summary>
               /// <returns></returns>
               /// <exception cref="Logic::InvalidOperationException">Iterator cannot be dereferenced</exception>
               CommandNode* operator->() const
               { 
                  if (!Position)
                     throw InvalidOperationException(HERE, L"Cannot dereference iterator");

                  return Position.get();
               }

               /// <summary>Compares two positions</summary>
               /// <param name="r">Another iterator</param>
               /// <returns></returns>
               bool operator==(const Iterator& r) const
               { 
                  return Tree == r.Tree && Position == r.Position; 
               }

               /// <summary>Compares two positions</summary>
               /// <param name="r">Another iterator</param>
               /// <returns></returns>
               bool operator!=(const Iterator& r) const
               { 
                  return Tree != r.Tree || Position != r.Position; 
               }

               // ----------------------- MUTATORS ------------------------
            public:
               /// <summary>Advances the iterator</summary>
               /// <returns>New position</returns>
               /// <exception cref="Logic::AlgorithmException">Traversal corruption</exception>
               /// <exception cref="Logic::ArgumentNullException">Missing node</exception>
               /// <exception cref="Logic::InvalidOperationException">Iterator cannot be advanced</exception>
               Iterator& operator++()
               { 
                  Advance();
                  return *this;
               }

               /// <summary>Advances the iterator</summary>
               /// <param name="">Ignored</param>
               /// <returns>Previous position</returns>
               /// <exception cref="Logic::AlgorithmException">Traversal corruption</exception>
               /// <exception cref="Logic::ArgumentNullException">Missing node</exception>
               /// <exception cref="Logic::InvalidOperationException">Iterator cannot be advanced</exception>
               Iterator operator++(int)
               {
                  Iterator tmp(*this); 
                  operator++(); 
                  return tmp;
               }

               /// <summary>Assignent operator</summary>
               /// <param name="r">Another iterator</param>
               /// <returns></returns>
               Iterator& operator=(const Iterator& r)
               {
                  // Self=assignment
                  if (this == &r)
                     return *this;

                  // Copy details
                  Tree = r.Tree;
                  Position = r.Position;
                  Traversal = r.Traversal;

                  return *this;
               }
         
               /// <summary>Move-Assignent operator</summary>
               /// <param name="r">Another iterator</param>
               /// <returns></returns>
               Iterator& operator=(Iterator&& r)
               {
                  // Self=assignment
                  if (this == &r)
                     return *this;

                  // Move details
                  Tree = r.Tree;
                  Position = r.Position;
                  Traversal = std::ref(r.Traversal);

                  r.Position = nullptr;
                  return *this;
               }

            protected:
               /// <summary>Advances position to the next node.</summary>
               /// <exception cref="Logic::AlgorithmException">Invalid traversal state</exception>
               /// <exception cref="Logic::ArgumentNullException">Node is nullptr</exception>
               /// <exception cref="Logic::InvalidOperationException">Iterator cannot be advanced</exception>
               void Advance()
               {
                  // End: Error
                  if (!Position)
                     throw InvalidOperationException(HERE, L"Iterator cannot be advanced");

                  // Non-final node: Advance position + visit next node
                  if (!Traversal.Empty())
                  {
                     Position = Traversal.GetSuccessor();
                     Traversal.AddSuccessors(Position);
                  }
                  // Final node: Move to 'End' state
                  else
                     Position = nullptr;
               }

               // -------------------- REPRESENTATION ---------------------
            protected:
               const CommandTree* Tree;        // Parent tree
               CommandNodePtr     Position;    // Current position
               TRAVERSAL          Traversal;   // Traversal type
            };

            /// <summary>Breadth-first command tree iterator</summary>
            typedef Iterator<BreadthTraversal>  BreadthIterator;

            /// <summary>Depth-first command tree iterator</summary>
            typedef Iterator<DepthTraversal>  DepthIterator;

         protected:
            /// <summary>Distinguishes tree state when printed to the console</summary>
            enum class TreeState { Raw, Verified, Compiled };

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
               NodePrinter(CommandTree& t);
               virtual ~NodePrinter();
		 
               NO_COPY(NodePrinter);	// Uncopyable
               NO_MOVE(NodePrinter);	// Unmovable

               // ---------------------- ACCESSORS ------------------------			

               // ----------------------- MUTATORS ------------------------
            public:
               void VisitNode(CommandNode* n) override;
               void VisitRoot(CommandNode* n) override;

               // -------------------- REPRESENTATION ---------------------
            protected:
               CommandTree& Tree;
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

            // --------------------- CONSTRUCTION ----------------------
         public:
            CommandTree();
            virtual ~CommandTree();

            DEFAULT_COPY(CommandTree);	// Default copy semantics
            DEFAULT_MOVE(CommandTree);	// Default move semantics

            // ------------------------ STATIC -------------------------
         public:
            static const wchar*  GetString(TreeState s);

            // --------------------- PROPERTIES ------------------------
         public:
            PROPERTY_GET(bool,Empty,IsEmpty);

            // ---------------------- ACCESSORS ------------------------		
         public:
            /// <summary>Get start iterator for tree</summary>
            /// <typeparam name="T">Traversal type</typeparam>
            /// <returns></returns>
            template <typename T>
            Iterator<T> begin() const
            {
               return Iterator<T>(*this, const_cast<CommandNodePtr&>(Root));
            }

            /// <summary>Get finish iterator for tree</summary>
            /// <typeparam name="T">Traversal type</typeparam>
            /// <returns></returns>
            template <typename T>
            Iterator<T> end() const
            {
               return Iterator<T>(*this);
            }

            /// <summary>Get start iterator for a depth traversal of the tree</summary>
            /// <returns></returns>
            DepthIterator begin() const
            {
               return begin<DepthTraversal>();
            }

            /// <summary>Get finish iterator for a depth traversal of the tree</summary>
            /// <returns></returns>
            DepthIterator end() const
            {
               return end<DepthTraversal>();
            }

            void  FindAll(const wstring& name, SymbolType type, SymbolList& results) const;
            bool  IsEmpty() const;
            void  Print() const;
            void  ToList(CommandNodeList& l) const;

            // ----------------------- MUTATORS ------------------------
         public:
            CommandNodePtr Add(CommandNodePtr node);
            void           Compile(ScriptFile& script, ErrorArray& errors);
            void           Transform(CommandNode::Visitor& v);
            void           Verify(ScriptFile& script, ErrorArray& errors);

         protected:
            void  ExpandMacros(ScriptFile& script, ErrorArray& errors);
            
            // -------------------- REPRESENTATION ---------------------
         protected:
            CommandNodePtr  Root;    // Root node of parse tree
            TreeState      State;   // processing state
         };
      }
   }
}

using namespace Logic::Scripts;
