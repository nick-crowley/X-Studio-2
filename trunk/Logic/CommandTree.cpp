#include "stdafx.h"
#include "CommandTree.h"

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         // -------------------------------- CONSTRUCTION --------------------------------

         CommandTree::CommandTree() : Root(new CommandNode())
         {
         }


         CommandTree::~CommandTree()
         {
         }

         // ------------------------------- STATIC METHODS -------------------------------

         // ------------------------------- PUBLIC METHODS -------------------------------
         
         /// <summary>Adds a node as a child of the root</summary>
         /// <param name="cmd">node to add</param>
         /// <returns>Input node</returns>
         CommandNodePtr  CommandTree::Add(CommandNodePtr node)
         {
            // Set parent to Root
            node->Parent = Root.get();

            // Append
            Root->Children.push_back(node);
            return node;
         }

         /// <summary>Query whether this tree is empty.</summary>
         /// <returns></returns>
         bool CommandTree::IsEmpty() const
         {
            return Root->IsEmpty();
         }

         /// <summary>Finds all instances of a symbol.</summary>
         /// <param name="name">symbol name</param>
         /// <param name="type">symbol type</param>
         /// <param name="results">On return, this contains the results</param>
         void  CommandTree::FindAll(const wstring& name, SymbolType type, SymbolList& results) const
         {
            Transform(CommandNode::SymbolSearcher(name, type, results));
         }
         
         /// <summary>Compiles the script.</summary>
         /// <param name="script">The script.</param>
         /// <exception cref="Logic::AlgorithmException">Error in linking algorithm</exception>
         void  CommandTree::Compile(ScriptFile& script, ErrorArray& errors)
         {
            UINT i = 0;
            CommandGenerator   generator(script, errors);
            ConstantIdentifier constants(script, errors);
            LinkageFinalizer   finalizer(errors);
            NodeIndexer        indexer(i);
            NodeLinker         linker(errors);
            VariableIdentifier variables(script, errors);

            // Macros: Expand macros into std commands 
            if (PrefsLib.UseMacroCommands)
            {
               Root->ExpandMacros(script, errors);

#ifdef VALIDATION
               // Clear previous IDs
               script.Clear();

               // Re-index variables to account for hidden iterator variables
               Transform(variables);

               // Re-identify constants
               Transform(constants);
#endif
            }

            // Linking/Indexing
            Transform(linker);
            Transform(indexer);
               
#ifdef VALIDATION
            // Set address of EOF
            CommandNode::EndOfScript.Index = i;     
#endif
            // Finalize linkage + generate commands
            Transform(finalizer);
            Transform(generator);
            
            // Update state
            State = CommandNode::InputState::Compiled;
         }
         
         /// <summary>Prints the entire tree to the console</summary>
         void  CommandTree::Print() const
         {
            CommandNode::NodePrinter v;
            // Print entire tree
            Transform(v);
         }
         
         /// <summary>Flattens the nodes of the tree (excluding the root) into a list.</summary>
         /// <param name="l">On return this contains all nodes of the tree</param>
         void  CommandTree::ToList(CommandNodeList& l) const
         {
            // Prepare
            l.clear();

            // Flatten children (if any) into list
            for (auto& c : *this)
               l.push_back(c);
         }

         /// <summary>Transforms the tree by executing a visitor upon the entire tree</summary>
         /// <param name="v">The visitor.</param>
         void  CommandTree::Transform(CommandNode::Visitor& v)
         {
            // Execute visitor
            for (auto& c : *this)
               c->Accept(v);
         }

         /// <summary>Verifies the entire tree</summary>
         /// <param name="script">script</param>
         /// <param name="errors">errors collection</param>
         void  CommandTree::Verify(ScriptFile& script, ErrorArray& errors) 
         {
            CommandGenerator    commands(script, errors);
            ConstantIdentifier  constants(script, errors);
            LogicVerifier       logic(errors);
            TerminationVerifier termination(errors);
            VariableIdentifier  variables(script, errors);

            // Identify labels/variables/constants
            Transform(variables);
            Transform(constants);

            // Verify commands+parameters
            Transform(commands);

            // branching logic
            Transform(logic);

            // Ensure script has std commands  [don't count break/continue]
            if (!any_of(begin(), end(), isStandardCommand))
               errors += Root->MakeError(L"No executable commands found");

            // [VALID] Verify all control paths lead to RETURN
            else if (errors.empty()) 
               Transform(termination);
            
            // Update state
            Root->State = CommandNode::InputState::Verified;
         }

         // ------------------------------ PROTECTED METHODS -----------------------------

         // ------------------------------- PRIVATE METHODS ------------------------------
      }
   }
}
