#pragma once
#include "CommandSyntax.h"
#include "CommandLexer.h"
#include "CommandNodeList.h"
#include "ScriptCommand.h"
#include "ScriptParameter.h"
#include "ScriptToken.h"
#include "ErrorToken.h"
#include "Symbol.h"
#include "TreeTraversal.h"
#include <algorithm>

namespace Testing
{
   class LogicTests;
}

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         class CommandNode;
         
         /// <summary>Shared pointer to a parse tree node</summary>
         class LogicExport CommandNodePtr : public shared_ptr<CommandNode> 
         {
            // --------------------- CONSTRUCTION ----------------------
         public:
            CommandNodePtr() : shared_ptr<CommandNode>(nullptr)
            {}
            CommandNodePtr(CommandNode* node) : shared_ptr<CommandNode>(node)
            {}

            // ---------------------- ACCESSORS ------------------------	

            // ----------------------- MUTATORS ------------------------

            // -------------------- REPRESENTATION ---------------------
         };

            
         /// <summary>Represents a script command and its descendants, if any</summary>
         class LogicExport CommandNode 
         {
            friend class ::Testing::LogicTests;

            // ------------------------ TYPES --------------------------
         protected:
            /// <summary>CommandNode array iterator</summary>
            typedef CommandNodeList::const_iterator   NodeIterator;

            /// <summary>CommandNode predicate</summary>
            typedef function<bool (const CommandNodePtr&)>  NodeDelegate;

            /// <summary>Generates iterator variable names</summary>
            class NameGenerator
            {
            public:
               NameGenerator() : LastID(1)
               {}

               /// <summary>Generates another iterator variable name</summary>
               /// <returns></returns>
               wstring  GetNext()
               {
                  return VString(L"$XS.Iterator%d", LastID++);
               }

            protected:
               int  LastID;
            };

            /// <summary>Distinguishes tree state when printed to the console</summary>
            enum class InputState { Raw, Verified, Compiled };

         public:
            /// <summary>Forward-only iterator for nodes in a CommandNode</summary>
            template <typename TRAVERSAL>
            class Iterator : public std::iterator<std::forward_iterator_tag, CommandNode>
            {
               friend class CommandNode;
               
               // ------------------------ TYPES --------------------------
            protected:
               
               // --------------------- CONSTRUCTION ----------------------
            protected:
               /// <summary>Creates an end iterator</summary>
               /// <param name="r">Root node</param>
               /// <exception cref="Logic::ArgumentNullException">Root is nullptr</exception>
               Iterator(CommandNode* r) 
                  : Root(r), Position(nullptr)
               {
                  REQUIRED(r);
               }

               /// <summary>Creates a start iterator</summary>
               /// <param name="r">Root node</param>
               /// <param name="n">Start node</param>
               /// <exception cref="Logic::ArgumentNullException">Traversal, root node, or start node are nullptr</exception>
               Iterator(CommandNode* r, CommandNode* n) 
                  : Root(r), Position(n)
               {
                  REQUIRED(r);
                  REQUIRED(n);

                  // Add children of start node
                  Traversal.AddSuccessors(Position);
               }

            public:
               /// <summary>Copy create from an existing iterator</summary>
               /// <param name="r">Another iterator</param>
               Iterator(const Iterator& r)
                  : Root(r.Root), 
                    Position(r.Position), 
                    Traversal(r.Traversal)
               {
               }
         
               /// <summary>Move-create from an existing iterator</summary>
               /// <param name="r">Another iterator</param>
               Iterator(Iterator&& r)
                  : Root(r.Root), 
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
               CommandNode& operator*() const
               { 
                  if (!Position)
                     throw InvalidOperationException(HERE, L"Cannot dereference iterator");

                  return *Position; 
               }

               /// <summary>Get pointer to current node</summary>
               /// <returns></returns>
               /// <exception cref="Logic::InvalidOperationException">Iterator cannot be dereferenced</exception>
               CommandNode* operator->() const
               { 
                  if (!Position)
                     throw InvalidOperationException(HERE, L"Cannot dereference iterator");

                  return Position;
               }

               /// <summary>Compares two positions</summary>
               /// <param name="r">Another iterator</param>
               /// <returns></returns>
               bool operator==(const Iterator& r) const
               { 
                  return Root == r.Root && Position == r.Position; 
               }

               /// <summary>Compares two positions</summary>
               /// <param name="r">Another iterator</param>
               /// <returns></returns>
               bool operator!=(const Iterator& r) const
               { 
                  return Root != r.Root || Position != r.Position; 
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
                  Root = r.Root;
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
                  Root = r.Root;
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
               CommandNode*   Root;        // Root node of tree
               CommandNode*   Position;    // Current position
               TRAVERSAL      Traversal;   // Traversal type
            };

            /// <summary>Breadth-first command tree iterator</summary>
            typedef Iterator<BreadthTraversal>  BreadthIterator;

            /// <summary>Depth-first command tree iterator</summary>
            typedef Iterator<DepthTraversal>  DepthIterator;

            /// <summary>Base-class for all visitors</summary>
            class Visitor
            {
               // --------------------- CONSTRUCTION ----------------------
            public:
               virtual ~Visitor() {}
               
               // ---------------------- ACCESSORS ------------------------			
               
               // ----------------------- MUTATORS ------------------------
            public:
               virtual void VisitNode(CommandNode* n) = 0;
               virtual void VisitRoot(CommandNode* n) {}
            };

            /// <summary>Distinguishes variables and constants from their usage</summary>
            class ConstantIdentifier : public Visitor
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
            class CommandGenerator : public Visitor
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
            class CommandVerifier : public Visitor
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
            class LinkageFinalizer : public Visitor
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
            class LogicVerifier : public Visitor
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

            /// <summary>Assigns standard command indicies to nodes</summary>
            class NodeIndexer : public Visitor
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
            class NodeLinker : public Visitor
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
            class NodePrinter : public Visitor
            {
               // ------------------------ TYPES --------------------------
            protected:
               // --------------------- CONSTRUCTION ----------------------
            public:
               NodePrinter();
               virtual ~NodePrinter();
		 
               NO_COPY(NodePrinter);	// Uncopyable
               NO_MOVE(NodePrinter);	// Unmovable

               // ---------------------- ACCESSORS ------------------------			

               // ----------------------- MUTATORS ------------------------
            public:
               void VisitNode(CommandNode* n) override;
               void VisitRoot(CommandNode* n) override;

               // -------------------- REPRESENTATION ---------------------
            };
            
            /// <summary>Searches for all instances of a symbol</summary>
            class SymbolSearcher : public Visitor
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
            class TerminationVerifier : public Visitor
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
            class VariableIdentifier : public Visitor
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
            CommandNode();
            CommandNode(Conditional cnd, CommandSyntaxRef syntax, ParameterArray& params, const CommandLexer& lex, UINT line, bool commented);
            CommandNode(Conditional cnd, CommandSyntaxRef syntax, ParameterArray& infix, ParameterArray& postfix, const CommandLexer& lex, UINT line, bool commented);
            ~CommandNode();

         protected:
            CommandNode(const CommandNode& parent, const CommandNode* target);
            CommandNode(const CommandNode& macro, const CommandNode& expanded);

            // ------------------------ STATIC -------------------------
         protected:
            static NodeDelegate  isConditionalAlternate;
            static NodeDelegate  isConditionalEnd;
            static NodeDelegate  isExecutableCommand;
            static NodeDelegate  isStandardCommand;
            static NodeDelegate  isSkipIfCompatible;
            
            static const wchar*  GetString(InputState s);

#ifdef VALIDATION
            /// <summary>An invisible node that functions as a jump target with address 'script_length+1'</summary>
            static CommandNode  EndOfScript;
#endif
            // --------------------- PROPERTIES ------------------------
         public:
            PROPERTY_GET(bool,Empty,IsEmpty);
            PROPERTY_GET(BranchLogic,Logic,GetBranchLogic);
            PROPERTY_GET(GuiString,LineCode,GetLineCode);

            // ---------------------- ACCESSORS ------------------------		
         public:
            /// <summary>Get start iterator for this node</summary>
            /// <typeparam name="T">Traversal type</typeparam>
            /// <returns></returns>
            template <typename T>
            Iterator<T> begin() const
            {
               return Iterator<T>(FindRoot(), const_cast<CommandNode*>(this));
            }

            /// <summary>Get end iterator for this node</summary>
            /// <typeparam name="T">Traversal type</typeparam>
            /// <returns></returns>
            template <typename T>
            Iterator<T> end() const
            {
               return Iterator<T>(FindRoot());
            }

            DepthIterator begin() const;
            DepthIterator end() const;

            BranchLogic   GetBranchLogic() const;
            GuiString     GetLineCode() const;
            bool          Is(UINT ID) const;
            bool          Is(CommandType t) const;
            bool          IsEmpty() const;
               
         protected:
            CommandNode*  FindAncestor(BranchLogic l) const;
            NodeIterator  FindChild(const CommandNode* child) const;
            CommandNode*  FindConditionalAlternate() const;
            CommandNode*  FindConditionalEnd() const;
            CommandNode*  FindLabel(const wstring& name) const;
            CommandNode*  FindNextCommand() const;
            CommandNode*  FindNextSibling() const;
            CommandNode*  FindPrevSibling() const;
            CommandNode*  FindRoot() const;
            CommandNode*  FindSibling(NodeDelegate d, const wchar* help) const;
            CommandNode*  GetLastExecutableChild() const;
            wstring       GetScriptCallName() const;
            bool          HasExecutableChild() const;
            bool          IsRoot() const;
            ErrorToken    MakeError(const GuiString& msg) const;
            ErrorToken    MakeError(const GuiString& msg, const ScriptToken& tok) const;
            
            // ----------------------- MUTATORS ------------------------
         public:
            void           Accept(Visitor& v);
            CommandNodePtr Add(CommandNodePtr node);
            
         protected:
            CommandNodePtr   ExpandCommand(const wstring& txt, GameVersion v);
            CommandNodeList  ExpandDimArray(ScriptFile& script);
            CommandNodeList  ExpandForLoop(ScriptFile& script);
            CommandNodeList  ExpandForEach(ScriptFile& script);
            void  ExpandMacros(ScriptFile& script, ErrorArray& errors);
            void  InsertJump(NodeIterator pos, const CommandNode* target);
            void  MoveChildren(CommandNode& from, CommandNode& to);
            void  RevertCommandComment(CommandNode* child);
            void  ReplaceChild(CommandNode* oldChild, CommandNode* newChild);
            
            // -------------------- REPRESENTATION ---------------------
         public:
            CommandNode*       Parent;        // Parent node
            CommandNodeList    Children;      // Child commands

            ParameterArray     Parameters,    // script parameters in display order
                               Postfix;       // expression parameters in postfix order
            CommandSyntaxRef   Syntax;        // command syntax
            bool               CmdComment;    // Whether a command comment  [false for ordinary comments]
            GuiString          LineText;      // line text

            const UINT         LineNumber;    // 1-based line number
            const CHARRANGE    Extent;        // Start/end character offsets

         protected:
            Conditional        Condition;     // Conditional
            const CommandNode* JumpTarget;    // Destination of unconditional-jmp or jump-if-false
            UINT               Index;         // 0-based standard codearray index
            InputState         State;         // Debug: processing state
            NameGenerator      IteratorNames;
         };
      }
   }
}

using namespace Logic::Scripts::Compiler;
