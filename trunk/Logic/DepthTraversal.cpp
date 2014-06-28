#include "stdafx.h"
#include "CommandNode.h"
#include "TreeTraversal.h"

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {

         // -------------------------------- CONSTRUCTION --------------------------------

         // ------------------------------- STATIC METHODS -------------------------------

         // ------------------------------- PUBLIC METHODS -------------------------------

         /// <summary>Adds successors nodes to the queue</summary>
         /// <param name="n">Node to visit</param>
         /// <exception cref="Logic::ArgumentNullException">Node is nullptr</exception>
         void DepthTraversal::AddSuccessors(CommandNode* n)
         {
            REQUIRED(n);

            // Push children to stack in reverse order
            for (auto it = n->Children.rbegin(); it != n->Children.rend(); ++it)
               push_front(it->get());
         }

         /// <summary>Query whether traversal is empty</summary>
         /// <returns></returns>
         bool DepthTraversal::Empty() const
         {
            return empty();
         }
         
         /// <summary>Pops the next node from the stack</summary>
         /// <returns></returns>
         /// <exception cref="Logic::AlgorithmException">Stack is empty</exception>
         CommandNode* DepthTraversal::GetSuccessor()
         {
            // Verify state
            if (Empty())
               throw AlgorithmException(HERE, L"Cannot pop empty stack");

            // Pop stack
            auto n = front();
            pop_front();
            return n;
         }

         // ------------------------------ PROTECTED METHODS -----------------------------
         
         // ------------------------------- PRIVATE METHODS ------------------------------
   
      }
   }

}
