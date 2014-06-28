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
         void BreadthTraversal::AddSuccessors(CommandNode* n)
         {
            REQUIRED(n);

            // Enqueue children to back of queue
            for (auto& c : n->Children)
               push_back(c.get());
         }

         /// <summary>Query whether traversal is empty</summary>
         /// <returns></returns>
         bool BreadthTraversal::Empty() const
         {
            return empty();
         }
         
         /// <summary>Pops the next node from the queue</summary>
         /// <returns></returns>
         /// <exception cref="Logic::AlgorithmException">Queue is empty</exception>
         CommandNode* BreadthTraversal::GetSuccessor()
         {
            // Verify state
            if (Empty())
               throw AlgorithmException(HERE, L"Cannot pop empty queue");

            // Dequeue next 
            auto n = front();
            pop_front();
            return n;
         }
         
         // ------------------------------ PROTECTED METHODS -----------------------------
         
         // ------------------------------- PRIVATE METHODS ------------------------------
   
      }
   }

}
