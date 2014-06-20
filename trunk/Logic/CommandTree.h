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
         class CommandTree;
         
         /// <summary>Shared pointer to a parse tree node</summary>
         class LogicExport CommandNodePtr : public shared_ptr<CommandTree> 
         {
            // --------------------- CONSTRUCTION ----------------------
         public:
            CommandNodePtr() : shared_ptr<CommandTree>(nullptr)
            {}
            CommandNodePtr(CommandTree* node) : shared_ptr<CommandTree>(node)
            {}

            // ---------------------- ACCESSORS ------------------------	

            // ----------------------- MUTATORS ------------------------

            // -------------------- REPRESENTATION ---------------------
         };

            
         /// <summary>Represents a script command and its descendants, if any</summary>
         class LogicExport CommandTree 
         {
            friend class ::Testing::LogicTests;

            // ------------------------ TYPES --------------------------
         protected:
            /// <summary>CommandTree array iterator</summary>
            typedef CommandNodeList::const_iterator   NodeIterator;

            /// <summary>CommandTree predicate</summary>
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
            /// <summary>Forward-only iterator for nodes in a CommandTree</summary>
            template <typename TRAVERSAL>
            class Iterator : public std::iterator<std::forward_iterator_tag, CommandTree>
            {
               friend class CommandTree;
               
               // ------------------------ TYPES --------------------------
            protected:
               
               // --------------------- CONSTRUCTION ----------------------
            protected:
               /// <summary>Creates an end iterator</summary>
               /// <param name="r">Root node</param>
               /// <exception cref="Logic::ArgumentNullException">Root is nullptr</exception>
               Iterator(CommandTree* r) 
                  : Root(r), Position(nullptr)
               {
                  REQUIRED(r);
               }

               /// <summary>Creates a start iterator</summary>
               /// <param name="r">Root node</param>
               /// <param name="n">Start node</param>
               /// <exception cref="Logic::ArgumentNullException">Traversal, root node, or start node are nullptr</exception>
               Iterator(CommandTree* r, CommandTree* n) 
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
               CommandTree& operator*() const
               { 
                  if (!Position)
                     throw InvalidOperationException(HERE, L"Cannot dereference iterator");

                  return *Position; 
               }

               /// <summary>Get pointer to current node</summary>
               /// <returns></returns>
               /// <exception cref="Logic::InvalidOperationException">Iterator cannot be dereferenced</exception>
               CommandTree* operator->() const
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
               CommandTree*   Root;        // Root node of tree
               CommandTree*   Position;    // Current position
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
               virtual void VisitNode(CommandTree* n) = 0;
               virtual void VisitRoot(CommandTree* n) {}
            };

            /// <summary>Distinguishes variables and constants from their usage</summary>
            class ConstantVisitor : public Visitor
            {
               // ------------------------ TYPES --------------------------
            protected:
               // --------------------- CONSTRUCTION ----------------------
            public:
               ConstantVisitor(ScriptFile& s, ErrorArray& e);
               virtual ~ConstantVisitor();
		 
               NO_COPY(ConstantVisitor);	// Uncopyable
               NO_MOVE(ConstantVisitor);	// Unmovable

               // ---------------------- ACCESSORS ------------------------			

               // ----------------------- MUTATORS ------------------------
            public:
               void VisitNode(CommandTree* n) override;

               // -------------------- REPRESENTATION ---------------------
            protected:
               ErrorArray& Errors;     // Errors collection
               ScriptFile& Script;     // Script file
            };

            /// <summary>Generates a ScriptCommand from a node</summary>
            class GeneratingVisitor : public Visitor
            {
               // ------------------------ TYPES --------------------------
            protected:
               // --------------------- CONSTRUCTION ----------------------
            public:
               GeneratingVisitor(ScriptFile& s, ErrorArray& e);
               virtual ~GeneratingVisitor();
		 
               NO_COPY(GeneratingVisitor);	// Uncopyable
               NO_MOVE(GeneratingVisitor);	// Unmovable

               // ---------------------- ACCESSORS ------------------------			

               // ----------------------- MUTATORS ------------------------
            public:
               void VisitNode(CommandTree* n) override;

               // -------------------- REPRESENTATION ---------------------
            protected:
               ErrorArray& Errors;     // Errors collection
               ScriptFile& Script;     // Script file
            };
            
            /// <summary>Finalizes linkage between nodes</summary>
            class FinalizingVisitor : public Visitor
            {
               // ------------------------ TYPES --------------------------
            protected:
               // --------------------- CONSTRUCTION ----------------------
            public:
               FinalizingVisitor(ErrorArray& e);
               virtual ~FinalizingVisitor();
		 
               NO_COPY(FinalizingVisitor);	// Uncopyable
               NO_MOVE(FinalizingVisitor);	// Unmovable

               // ---------------------- ACCESSORS ------------------------			

               // ----------------------- MUTATORS ------------------------
            public:
               void VisitNode(CommandTree* n) override;

               // -------------------- REPRESENTATION ---------------------
            protected:
               ErrorArray& Errors;     // Errors collection
            };

            /// <summary>Performs linking between nodes</summary>
            class LinkingVisitor : public Visitor
            {
               // ------------------------ TYPES --------------------------
            protected:
               // --------------------- CONSTRUCTION ----------------------
            public:
               LinkingVisitor(ErrorArray& e);
               virtual ~LinkingVisitor();
		 
               NO_COPY(LinkingVisitor);	// Uncopyable
               NO_MOVE(LinkingVisitor);	// Unmovable

               // ---------------------- ACCESSORS ------------------------			

               // ----------------------- MUTATORS ------------------------
            public:
               void VisitNode(CommandTree* n) override;

               // -------------------- REPRESENTATION ---------------------
            protected:
               ErrorArray& Errors;     // Errors collection
            };

            /// <summary>Prints nodes to the console</summary>
            class PrintingVisitor : public Visitor
            {
               // ------------------------ TYPES --------------------------
            protected:
               // --------------------- CONSTRUCTION ----------------------
            public:
               PrintingVisitor();
               virtual ~PrintingVisitor();
		 
               NO_COPY(PrintingVisitor);	// Uncopyable
               NO_MOVE(PrintingVisitor);	// Unmovable

               // ---------------------- ACCESSORS ------------------------			

               // ----------------------- MUTATORS ------------------------
            public:
               void VisitNode(CommandTree* n) override;
               void VisitRoot(CommandTree* n) override;

               // -------------------- REPRESENTATION ---------------------
            };

            /// <summary>Assigns standard command indicies to nodes</summary>
            class IndexingVisitor : public Visitor
            {
               // ------------------------ TYPES --------------------------
            protected:
               // --------------------- CONSTRUCTION ----------------------
            public:
               IndexingVisitor(UINT& init);
               virtual ~IndexingVisitor();
		 
               NO_COPY(IndexingVisitor);	// Uncopyable
               NO_MOVE(IndexingVisitor);	// Unmovable

               // ---------------------- ACCESSORS ------------------------			

               // ----------------------- MUTATORS ------------------------
            public:
               void VisitNode(CommandTree* n) override;

               // -------------------- REPRESENTATION ---------------------
            protected:
               UINT&  NextIndex; // Next available index
            };

            /// <summary>Maps each variable name to a unique ID, and locates all label definitions</summary>
            class VariableVisitor : public Visitor
            {
               // ------------------------ TYPES --------------------------
            protected:
               // --------------------- CONSTRUCTION ----------------------
            public:
               VariableVisitor(ScriptFile& s, ErrorArray& e);
               virtual ~VariableVisitor();
		 
               NO_COPY(VariableVisitor);	// Uncopyable
               NO_MOVE(VariableVisitor);	// Unmovable

               // ---------------------- ACCESSORS ------------------------			

               // ----------------------- MUTATORS ------------------------
            public:
               void VisitNode(CommandTree* n) override;

               // -------------------- REPRESENTATION ---------------------
            protected:
               ErrorArray& Errors;     // Errors collection
               ScriptFile& Script;     // Script file
            };

            // --------------------- CONSTRUCTION ----------------------
         public:
            CommandTree();
            CommandTree(Conditional cnd, CommandSyntaxRef syntax, ParameterArray& params, const CommandLexer& lex, UINT line, bool commented);
            CommandTree(Conditional cnd, CommandSyntaxRef syntax, ParameterArray& infix, ParameterArray& postfix, const CommandLexer& lex, UINT line, bool commented);
            ~CommandTree();

         protected:
            CommandTree(const CommandTree& parent, const CommandTree* target);
            CommandTree(const CommandTree& macro, const CommandTree& expanded);

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
            static CommandTree  EndOfScript;
#endif
            // --------------------- PROPERTIES ------------------------
         public:
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
               return Iterator<T>(FindRoot(), const_cast<CommandTree*>(this));
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

            void          FindAll(const wstring& name, SymbolType type, SymbolList& results) const;
            BranchLogic   GetBranchLogic() const;
            GuiString     GetLineCode() const;
            bool          Is(UINT ID) const;
            bool          Is(CommandType t) const;
            void          Print(int depth = 0) const;
            void          ToList(CommandNodeList& l) const;
               
         protected:
            CommandTree*  FindAncestor(BranchLogic l) const;
            NodeIterator  FindChild(const CommandTree* child) const;
            CommandTree*  FindConditionalAlternate() const;
            CommandTree*  FindConditionalEnd() const;
            CommandTree*  FindLabel(const wstring& name) const;
            CommandTree*  FindNextCommand() const;
            CommandTree*  FindNextSibling() const;
            CommandTree*  FindPrevSibling() const;
            CommandTree*  FindRoot() const;
            CommandTree*  FindSibling(NodeDelegate d, const wchar* help) const;
            CommandTree*  GetLastExecutableChild() const;
            wstring       GetScriptCallName() const;
            bool          HasExecutableChild() const;
            bool          IsRoot() const;
            ErrorToken    MakeError(const GuiString& msg) const;
            ErrorToken    MakeError(const GuiString& msg, const ScriptToken& tok) const;
            void          VerifyLogic(ErrorArray& errors) const;
            void          VerifyParameter(const ScriptParameter& p, UINT index, const ScriptFile& script, ErrorArray& errors) const;
            void          VerifyTermination(ErrorArray& errors) const;
               
            // ----------------------- MUTATORS ------------------------
         public:
            void           Accept(Visitor& v);
            CommandNodePtr Add(CommandNodePtr node);
            void           Compile(ScriptFile& script, ErrorArray& errors);
            void           Verify(ScriptFile& script, ErrorArray& errors);
            
         protected:
            CommandNodePtr   ExpandCommand(const wstring& txt, GameVersion v);
            CommandNodeList  ExpandDimArray(ScriptFile& script);
            CommandNodeList  ExpandForLoop(ScriptFile& script);
            CommandNodeList  ExpandForEach(ScriptFile& script);
            void  ExpandMacros(ScriptFile& script, ErrorArray& errors);
            void  FinalizeLinkage(ErrorArray& errors);
            void  GenerateCommands(ScriptFile& script, ErrorArray& errors);
            void  IdentifyConstants(ScriptFile& script, ErrorArray& errors);
            void  IdentifyVariables(ScriptFile& script, ErrorArray& errors);
            void  IndexCommands(UINT& next);
            void  InsertJump(NodeIterator pos, const CommandTree* target);
            void  LinkCommands(ErrorArray& errors);
            void  MoveChildren(CommandTree& from, CommandTree& to);
            void  RevertCommandComment(CommandTree* child);
            void  ReplaceChild(CommandTree* oldChild, CommandTree* newChild);
            void  VerifyCommand(const ScriptFile& script, ErrorArray& errors);
            
            // -------------------- REPRESENTATION ---------------------
         public:
            CommandTree*       Parent;        // Parent node
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
            const CommandTree* JumpTarget;    // Destination of unconditional-jmp or jump-if-false
            UINT               Index;         // 0-based standard codearray index
            InputState         State;         // Debug: processing state
            NameGenerator      IteratorNames;
         };
      }
   }
}

using namespace Logic::Scripts::Compiler;
