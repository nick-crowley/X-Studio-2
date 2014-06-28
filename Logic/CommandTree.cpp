#include "stdafx.h"
#include "CommandTree.h"
#include "PreferencesLibrary.h"
#include "ScriptFile.h"

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         // -------------------------------- CONSTRUCTION --------------------------------

         CommandTree::CommandTree() 
            : Root(new CommandNode()),
              State(TreeState::Raw)
         {
         }


         CommandTree::~CommandTree()
         {
         }

         // ------------------------------- STATIC METHODS -------------------------------
         
         /// <summary>Get input state string</summary>
         const wchar*  CommandTree::GetString(TreeState s)
         {
            switch (s)
            {
            case TreeState::Compiled:    return L"COMPILED";
            case TreeState::Verified:    return L"VERIFIED";
            case TreeState::Raw:         return L"UNVERIFIED";
            }

            return L"INVALID";
         }
         
         // ------------------------------- PUBLIC METHODS -------------------------------
         
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

            // Macros: Query whether macros are enabled
            if (PrefsLib.UseMacroCommands)
            {
               // Expand macros into std commands
               ExpandMacros(script, errors);

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
            State = TreeState::Compiled;
         }
         
         /// <summary>Finds all instances of a symbol.</summary>
         /// <param name="name">symbol name</param>
         /// <param name="type">symbol type</param>
         /// <param name="results">On return, this contains the results</param>
         void  CommandTree::FindAll(const wstring& name, SymbolType type, SymbolList& results) const
         {
            Visit(SymbolSearcher(name, type, results));
         }

         /// <summary>Gets the state of the tree</summary>
         /// <returns></returns>
         CommandTree::TreeState CommandTree::GetState() const
         {
            return State;
         }
         
         /// <summary>Query whether this tree is empty.</summary>
         /// <returns></returns>
         bool CommandTree::IsEmpty() const
         {
            return Root->IsEmpty();
         }

         /// <summary>Prints the entire tree to the console</summary>
         void  CommandTree::Print() const
         {
            // Print entire tree
            Visit(NodePrinter(*this));
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
            if (!any_of(begin(), end(), CommandNode::isStandardCommand))
               errors += Root->MakeError(L"No executable commands found");

            // [VALID] Verify all control paths lead to RETURN
            else if (errors.empty()) 
               Transform(termination);
            
            // Update state
            State = TreeState::Verified;
         }
         
         /// <summary>Transforms the tree by executing a visitor upon the entire tree</summary>
         /// <param name="v">The visitor.</param>
         void  CommandTree::Visit(CommandNode::Visitor& v) const
         {
            // Execute visitor
            for (auto& c : *this)
               c->Accept(v);
         }
         
         /// <summary>Append node as a child of the root</summary>
         /// <param name="r">node</param>
         /// <returns>Self</returns>
         CommandTree& CommandTree::operator+=(const CommandNodePtr& r)
         {
            // Set parent to Root
            r->Parent = Root.get();

            // Append child
            Root->Children.push_back(r);
            return *this;
         }

         // ------------------------------ PROTECTED METHODS -----------------------------

         /// <summary>Expands macro commands</summary>
         /// <param name="script">script file.</param>
         /// <param name="errors">Errors collection.</param>
         void  CommandTree::ExpandMacros(ScriptFile& script, ErrorArray& errors)
         {
            MacroExpander macros(script, errors);

            // Recurse into children  [Allowing for in-place modification of child list]
            for (DepthIterator it = ++begin(); it != end(); ++it)
            {
               // Expand macros
               it->Accept(macros);

               // Macro: Delete 
               auto n = it++;
               if ((*n)->Is(CommandType::Macro))
                  *n->Parent -= *n;
            }
            
            // Recurse into children  [Allowing for in-place modification of child list]
            //for (auto c = Children.begin(); c != Children.end(); )
            //{
            //   // Expand macros
            //   (*c)->ExpandMacros(script, errors);

            //   // Macro: Delete 
            //   auto n = c++;
            //   if ((*n)->Is(CommandType::Macro))
            //      Children.erase(n);
            //}
         }

         // ------------------------------- PRIVATE METHODS ------------------------------
      }
   }
}
