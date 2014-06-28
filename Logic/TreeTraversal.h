#pragma once
#include "CommandNode.h"

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         namespace Traversals
         {

            /// <summary>Provides different traversals to the tree Iterator</summary>
            interface ITraversal
            {
               // ---------------------- ACCESSORS ------------------------			
            public:
               /// <summary>Query whether traversal is empty</summary>
               /// <returns></returns>
               bool Empty() const;

               // ----------------------- MUTATORS ------------------------
            public:
               /// <summary>Adds successors nodes to the traversal</summary>
               /// <param name="n">Node to visit</param>
               /// <exception cref="Logic::ArgumentNullException">Node is nullptr</exception>
               void AddSuccessors(CommandNodePtr& n);

               /// <summary>Gets the next node in the traversal</summary>
               /// <returns></returns>
               /// <exception cref="Logic::AlgorithmException">Traversal is empty</exception>
               CommandNodePtr GetSuccessor();
            };

            /// <summary>Provides the iterator with a breadth first traversal</summary>
            class BreadthTraversal : public ITraversal, public std::deque<CommandNodePtr>
            {
               // --------------------- CONSTRUCTION ----------------------

               // ---------------------- ACCESSORS ------------------------			
            public:
               /// <summary>Query whether traversal is empty</summary>
               /// <returns></returns>
               bool Empty() const;

               // ----------------------- MUTATORS ------------------------
            public:
               /// <summary>Adds successors nodes to the traversal</summary>
               /// <param name="n">Node to visit</param>
               /// <exception cref="Logic::ArgumentNullException">Node is nullptr</exception>
               void AddSuccessors(CommandNodePtr& n);

               /// <summary>Gets the next node in the traversal</summary>
               /// <returns></returns>
               /// <exception cref="Logic::AlgorithmException">Traversal is empty</exception>
               CommandNodePtr GetSuccessor();
            };
               
            /// <summary>Provides the iterator with a depth first traversal</summary>
            class DepthTraversal : public ITraversal, public std::deque<CommandNodePtr>
            {
               // --------------------- CONSTRUCTION ----------------------

               // ---------------------- ACCESSORS ------------------------			
            public:
               /// <summary>Query whether traversal is empty</summary>
               /// <returns></returns>
               bool Empty() const;

               // ----------------------- MUTATORS ------------------------
            public:
               /// <summary>Adds successors nodes to the traversal</summary>
               /// <param name="n">Node to visit</param>
               /// <exception cref="Logic::ArgumentNullException">Node is nullptr</exception>
               void AddSuccessors(CommandNodePtr& n);

               /// <summary>Gets the next node in the traversal</summary>
               /// <returns></returns>
               /// <exception cref="Logic::AlgorithmException">Traversal is empty</exception>
               CommandNodePtr GetSuccessor();
            };

         }
      }
   }
}

using namespace Logic::Scripts::Compiler::Traversals;
