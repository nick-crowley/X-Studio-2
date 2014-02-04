#include "stdafx.h"
#include "CommandNode.h"

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         // -------------------------------- CONSTRUCTION --------------------------------

         RootNode::RootNode()
         {}

         RootNode::RootNode(CommandNode* parent) : Parent(parent)
         {}

         RootNode::~RootNode()
         {}

         // ------------------------------- STATIC METHODS -------------------------------

         // ------------------------------- PUBLIC METHODS -------------------------------
         
         /// <summary>Add child node</summary>
         /// <param name="cmd">The command node</param>
         /// <returns>Command node</returns>
         CommandNodePtr  RootNode::Add(CommandNodePtr node)
         {
            // Set parent and append
            node->Parent = this;
            Children.push_back(node);
            return node;
         }

         /// <summary>Compiles the script.</summary>
         /// <param name="script">The script.</param>
         void  CommandNode::Compile(ScriptFile& script)
         {
            // Compile children
            for (auto c : Children)
               c->Compile(script);
         }

         /// <summary>Debug print</summary>
         /// <param name="depth">The depth.</param>
         void  CommandNode::Print(int depth) const
         {
            // Print Children
            for (auto c : Children)
               c->Print(depth+1);
         }

         /// <summary>Populates the script with label and variable names</summary>
         /// <param name="script">script.</param>
         void  CommandNode::Populate(ScriptFile& script) 
         {
            // Populate from children
            for (const auto& cmd : Children)
               cmd->Populate(script);
         }
         
         /// <summary>Verifies the entire tree</summary>
         /// <param name="script">script</param>
         /// <param name="errors">errors collection</param>
         void  CommandNode::Verify(const ScriptFile& script, ErrorArray& errors) const 
         {
            // Verify children
            for (const auto& cmd : Children)
               cmd->Verify(script, errors);

            // Ensure script has commands
            if (Children.size() == 0)
               errors += ErrorToken(L"No commands found", LineNumber, Extent);
            
            // Ensure last std command is RETURN
            else //if (find_if(Children.rbegin(), Children.rend(), [](CommandNodePtr& n){return n->Is(CommandType::Standard);}) == Children.rend())
            {
               auto last = Children.end()[-1];
               if (!last->Is(CMD_RETURN))
                  errors += ErrorToken(L"Last command in script must be 'return'", last->LineNumber, last->Extent);
            }

            /*else for (auto node = Children.rbegin(); node != Children.rend(); ++node)
            {
               if (node[0]->Is(CommandType::Auxiliary))
                  continue;
               else if (!node[0]->Is(CMD_RETURN))
                  errors += ErrorToken(L"Last command in script must be 'return'", node[0]->LineNumber, node[0]->Extent);
               break;
            }*/
         }

         // ------------------------------ PROTECTED METHODS -----------------------------

         // ------------------------------- PRIVATE METHODS ------------------------------
      }
   }
}

