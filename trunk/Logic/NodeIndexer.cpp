#include "stdafx.h"
#include "CommandNode.h"

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         // -------------------------------- CONSTRUCTION --------------------------------

         /// <summary>Create standard command indexer visitor</summary>
         /// <param name="init">Initial index</param>
         CommandNode::NodeIndexer::NodeIndexer(UINT& init) : NextIndex(init)
         {
         }

         /// <summary>Nothing</summary>
         CommandNode::NodeIndexer::~NodeIndexer()
         {
         }

         // ------------------------------- STATIC METHODS -------------------------------

         // ------------------------------- PUBLIC METHODS -------------------------------
         
         /// <summary>Assigns standard command indicies to nodes</summary>
         /// <param name="n">Node</param>
         void  CommandNode::NodeIndexer::VisitNode(CommandNode* n) 
         {
            // Standard command  
            if (!n->CmdComment && n->Is(CommandType::Standard))
               n->Index = NextIndex++;
         }

         // ------------------------------ PROTECTED METHODS -----------------------------

         // ------------------------------- PRIVATE METHODS ------------------------------
      }
   }
}
