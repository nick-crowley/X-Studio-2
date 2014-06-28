#pragma once
#include "TreeTraversal.h"
#include "TreeVisitors.h"

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
               CommandNodePtr operator*() const
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

            /// <summary>Distinguishes tree state when printed to the console</summary>
            enum class TreeState { Raw, Verified, Compiled };

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

            void      FindAll(const wstring& name, SymbolType type, SymbolList& results) const;
            TreeState GetState() const;
            bool      IsEmpty() const;
            void      Print() const;
            void      ToList(CommandNodeList& l) const;
            void      Visit(CommandNode::Visitor& v) const;

            // ----------------------- MUTATORS ------------------------
         public:
            void         Compile(ScriptFile& script, ErrorArray& errors);
            void         Transform(CommandNode::Visitor& v);
            void         Verify(ScriptFile& script, ErrorArray& errors);

            CommandTree& operator+=(const CommandNodePtr& r);

         protected:
            void  ExpandMacros(ScriptFile& script, ErrorArray& errors);
            
            // -------------------- REPRESENTATION ---------------------
         protected:
            CommandNodePtr Root;    // Root node of parse tree
            TreeState      State;   // processing state
         };
      }
   }
}

using namespace Logic::Scripts;
