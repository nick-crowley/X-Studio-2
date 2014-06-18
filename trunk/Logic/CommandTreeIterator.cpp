#include "stdafx.h"
#include "CommandTree.h"

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {

         // -------------------------------- CONSTRUCTION --------------------------------

         /// <summary>Creates an end iterator</summary>
         /// <param name="r">Root node</param>
         /// <exception cref="Logic::ArgumentNullException">Root is nullptr</exception>
         CommandTree::Iterator::Iterator(CommandTree* r) : Root(r), Position(nullptr)
         {
            REQUIRED(r);
         }
         
         /// <summary>Creates a start iterator</summary>
         /// <param name="r">Root node</param>
         /// <param name="n">Start node</param>
         /// <exception cref="Logic::ArgumentNullException">Either node is nullptr</exception>
         CommandTree::Iterator::Iterator(CommandTree* r, CommandTree* n) : Root(r), Position(n)
         {
            REQUIRED(r);
            REQUIRED(n);

            // Add children to the stack
            Visit(n);
         }

         /// <summary>Copy create from an existing iterator</summary>
         /// <param name="r">Another iterator</param>
         CommandTree::Iterator::Iterator(const Iterator& r)
            : Root(r.Root), 
              Position(r.Position), 
              Nodes(r.Nodes)
         {
         }
         
         /// <summary>Move-create from an existing iterator</summary>
         /// <param name="r">Another iterator</param>
         CommandTree::Iterator::Iterator(Iterator&& r)
            : Root(r.Root), 
              Position(r.Position), 
              Nodes(std::ref(r.Nodes))
         {
            r.Position = nullptr;
         }

         CommandTree::Iterator::~Iterator()
         {
         }

         // ------------------------------- STATIC METHODS -------------------------------

         // ------------------------------- PUBLIC METHODS -------------------------------

         /// <summary>Get reference to current node</summary>
         /// <returns></returns>
         CommandTree& CommandTree::Iterator::operator*() const
         { 
            return *Position; 
         }

         /// <summary>Get pointer to current node</summary>
         /// <returns></returns>
         CommandTree* CommandTree::Iterator::operator->() const
         { 
            return Position;
         }

         /// <summary>Compares two positions</summary>
         /// <param name="r">Another iterator</param>
         /// <returns></returns>
         bool CommandTree::Iterator::operator==(const Iterator& r) const
         { 
            return Root == r.Root && Position == r.Position; 
         }

         /// <summary>Compares two positions</summary>
         /// <param name="r">Another iterator</param>
         /// <returns></returns>
         bool CommandTree::Iterator::operator!=(const Iterator& r) const
         { 
            return Root != r.Root || Position != r.Position; 
         }

         /// <summary>Assignent operator</summary>
         /// <param name="r">Another iterator</param>
         /// <returns></returns>
         CommandTree::Iterator& CommandTree::Iterator::operator=(const Iterator& r)
         {
            // Self=assignment
            if (this == &r)
               return *this;

            // Copy details
            Root = r.Root;
            Position = r.Position;
            Nodes = r.Nodes;

            return *this;
         }
         
         /// <summary>Move-Assignent operator</summary>
         /// <param name="r">Another iterator</param>
         /// <returns></returns>
         CommandTree::Iterator& CommandTree::Iterator::operator=(Iterator&& r)
         {
            // Self=assignment
            if (this == &r)
               return *this;

            // Move details
            Root = r.Root;
            Position = r.Position;
            Nodes = std::ref(r.Nodes);

            r.Position = nullptr;
            return *this;
         }

         /// <summary>Advances the iterator</summary>
         /// <returns>New position</returns>
         /// <exception cref="Logic::AlgorithmException">Stack corruption</exception>
         /// <exception cref="Logic::ArgumentNullException">Missing node</exception>
         /// <exception cref="Logic::InvalidOperationException">Iterator cannot be advanced</exception>
         CommandTree::Iterator& CommandTree::Iterator::operator++()
         { 
            Advance();
            return *this;
         }

         /// <summary>Advances the iterator</summary>
         /// <param name="">Ignored</param>
         /// <returns>Previous position</returns>
         /// <exception cref="Logic::AlgorithmException">Stack corruption</exception>
         /// <exception cref="Logic::ArgumentNullException">Missing node</exception>
         /// <exception cref="Logic::InvalidOperationException">Iterator cannot be advanced</exception>
         CommandTree::Iterator CommandTree::Iterator::operator++(int)
         {
            Iterator tmp(*this); 
            operator++(); 
            return tmp;
         }

         // ------------------------------ PROTECTED METHODS -----------------------------
         
         /// <summary>Advances position to the next node.</summary>
         /// <exception cref="Logic::AlgorithmException">Invalid stack state</exception>
         /// <exception cref="Logic::ArgumentNullException">Node is nullptr</exception>
         /// <exception cref="Logic::InvalidOperationException">Iterator cannot be advanced</exception>
         void CommandTree::Iterator::Advance()
         {
            // End: Error
            if (!Position)
               throw InvalidOperationException(HERE, L"Iterator cannot be advanced");

            // Non-final node: Advance position + visit next node
            if (!Nodes.empty())
               Visit(Position = Pop());

            // Final node: Move to 'End' state
            else
               Position = nullptr;
         }

         /// <summary>Pushes a node to the stack</summary>
         /// <param name="t">node</param>
         /// <exception cref="Logic::ArgumentNullException">Node is nullptr</exception>
         void CommandTree::Iterator::Push(CommandTree* t)
         {
            REQUIRED(t);

            Nodes.push_front(t);
         }

         /// <summary>Pops the next node from the stack</summary>
         /// <returns></returns>
         /// <exception cref="Logic::AlgorithmException">Stack is empty</exception>
         CommandTree* CommandTree::Iterator::Pop()
         {
            // Verify state
            if (Nodes.empty())
               throw AlgorithmException(HERE, L"Cannot pop empty stack");

            return Nodes.front();
         }
         
         /// <summary>Adds a nodes successors to the stack</summary>
         /// <param name="n">Node to visit</param>
         /// <exception cref="Logic::ArgumentNullException">Node is nullptr</exception>
         void CommandTree::Iterator::Visit(CommandTree* n)
         {
            REQUIRED(n);

            // Push children to stack in reverse order
            for (auto it = n->Children.rbegin(); it != n->Children.rend(); ++it)
               Push(it->get());
         }

         // ------------------------------- PRIVATE METHODS ------------------------------
   
      }
   }

}
