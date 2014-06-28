#include "stdafx.h"
#include "CommandTree.h"
#include "ScriptFile.h"
#include "ScriptParser.h"

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         // -------------------------------- CONSTRUCTION --------------------------------
         
         /// <summary>Create visitor for replacing macro commands with their non-macro equivilents</summary>
         /// <param name="s">script</param>
         /// <param name="e">errors collection</param>
         CommandTree::MacroExpander::MacroExpander(ScriptFile& s, ErrorArray& e) : Script(s), Errors(e)
         {
         }
         
         /// <summary>Nothing</summary>
         CommandTree::MacroExpander::~MacroExpander()
         {
         }
         
         // ------------------------------- STATIC METHODS -------------------------------

         // ------------------------------- PUBLIC METHODS -------------------------------
         
         /// <summary>Replaces macro commands with their non-macro equivilents</summary>
         /// <param name="n">Node to visit</param>
         void  CommandTree::MacroExpander::VisitNode(CommandNode* n) 
         {
            try
            {
               // DEBUG:
               //Console << "ExpandMacros: " << LineNumber << ": " << LineText << ENDL;

               // Don't expand commented macros
               if (!n->CmdComment && n->Is(CommandType::Macro))
               {
                  CommandNodeList nodes;

                  // Generate replacement nodes
                  switch (n->Syntax.ID)
                  {
                  case MACRO_DIM_ARRAY:  nodes = ExpandDimArray(n);   break;
                  case MACRO_FOR_LOOP:   nodes = ExpandForLoop(n);    break;
                  
                  case MACRO_FOR_EACH_COUNTER:  
                  case MACRO_FOR_EACH:   nodes = ExpandForEach(n);    break;
                  }

                  // Insert following self. All children have been moved to expanded commands.
                  n->Parent->Children.insert(++n->Parent->FindChild(n), nodes.begin(), nodes.end());
                  return;
               }
            }
            catch (ExceptionBase& e) {
               Errors += n->MakeError(GuiString(L"Macro expansion failed: ") + e.Message); 
            }
         }
         
         // ------------------------------ PROTECTED METHODS -----------------------------
         
         /// <summary>Generates an expanded node: new syntax/condition.parameters, with parent/line-num/line-text of current node</summary>
         /// <param name="txt">Command text.</param>
         /// <param name="v">game version</param>
         /// <returns></returns>
         CommandNodePtr  CommandTree::MacroExpander::ExpandCommand(CommandNode* n, const wstring& txt, GameVersion v)
         {
            // Generate new node
            return new CommandNode(*n, *ScriptParser::Generate(txt, v));
         }
         
         
         /// <summary>Generates the actual commands necessary to form the 'dim' macro</summary>
         /// <param name="n">Node being visited</param>
         /// <returns>List of expanded replacement commands</returns>
         /// <exception cref="Logic::AlgorithmException">macro parameters improperly verified</exception>
         /// <exception cref="Logic::InvalidOperationException">Not a 'dim' macro</exception>
         CommandNodeList  CommandTree::MacroExpander::ExpandDimArray(CommandNode* n)
         {
            CommandNodeList nodes;

            // Require 'DIM' command
            if (!n->Is(MACRO_DIM_ARRAY))
               throw InvalidOperationException(HERE, L"Command must be 'dim' macro");
            
            // Generate components
            auto size   = n->Parameters.size()-1;
            auto retVar = n->Parameters[0].Text.c_str();

            // Validate
            if (n->Parameters[0].Type != DataType::VARIABLE)
               throw AlgorithmException(HERE, L"'Dim' macro must be assigned to a variable");
            if (size < 1)
               throw AlgorithmException(HERE, L"'Dim' macro requires at least one element");

            // Generate '<retVar> = array alloc: size=<size>'
            VString cmd(L"%s = array alloc: size=%d", retVar, size);
            nodes += ExpandCommand(n, cmd, Script.Game);

            // Element assignments
            for (UINT i = 0; i < size; ++i)
            {
               auto value = n->Parameters[i+1].Text.c_str();
               
               // Generate '<array>[i] = <val>' 
               cmd = VString(L"%s[%d] = %s", retVar, i, value);
               nodes += ExpandCommand(n, cmd, Script.Game);
            }

            return nodes;
         }

         /// <summary>Generates the actual commands necessary to form the 'for loop' macro</summary>
         /// <param name="n">Node being visited</param>
         /// <returns>List of expanded replacement commands</returns>
         /// <exception cref="Logic::AlgorithmException">macro parameters improperly verified</exception>
         /// <exception cref="Logic::InvalidOperationException">Not a 'for loop' macro</exception>
         /// <remarks>Expands 'for $0 = $1 to $2 step $3' to
         ///
         /// (iterator) = (inital_value) ± (step_value)
         /// while (iterator) greater/less (final_value)
         /// (iterator) = (iterator) ± (step_value)</remarks>
         CommandNodeList  CommandTree::MacroExpander::ExpandForLoop(CommandNode* n)
         {
            CommandNodeList  nodes;
            ParameterArray   params,
                             postfix;

            // Require 'for $0 = $1 to $2 step $3'
            if (!n->Is(MACRO_FOR_LOOP))
               throw InvalidOperationException(HERE, L"Command must be 'for loop' macro");
            
            // Lookup components
            const wchar *iterator = n->Parameters[0].Text.c_str(),
                        *init_val = n->Parameters[1].Text.c_str(),
                        *last_val = n->Parameters[2].Text.c_str(),
                        *step_val = n->Parameters[3].Text.c_str();
            
            // Validate parameters
            if (n->Parameters[3].Type != DataType::INTEGER)
               throw AlgorithmException(HERE, L"Loop step must be an integer");

            // Determine direction
            int step = GuiString(step_val).ToInt();
            bool ascending = step > 0;

            // Init: (iterator) = (inital_value) ± (step_value)
            auto cmd = VString(L"%s = %s %s %d", iterator, init_val, (ascending ? L"-" : L"+"), (ascending ? step : -step));
            nodes += ExpandCommand(n, cmd, Script.Game);

            // Guard: while (iterator) less/greater (final_value)
            cmd = VString(L"while %s %s %s", iterator, (ascending ? L"<" : L">"), last_val);
            nodes += ExpandCommand(n, cmd, Script.Game);

            // Optimize using inc/dec if possible
            if (step == 1 || step == -1)
               // Advance: inc/dec (iterator)
               cmd = VString(L"%s %s", (ascending ? L"inc" : L"dec"), iterator);
            else 
               // Advance: (iterator) = (iterator) ± (step_value)
               cmd = VString(L"%s = %s %s %d", iterator, iterator, (ascending ? L"+" : L"-"), (ascending ? step : -step));
            
            // Add as child of 'while'
            nodes.back()->Add(ExpandCommand(n, cmd, Script.Game));     

            // Move all children of 'for loop' to 'while' expression
            n->MoveChildren(*nodes.back());

            return nodes;
         }
         
         /// <summary>Generates the actual commands necessary to form the 'foreach' macro</summary>
         /// <param name="n">Node being visited</param>
         /// <returns>List of expanded replacement commands</returns>
         /// <exception cref="Logic::AlgorithmException">macro parameters improperly verified</exception>
         /// <exception cref="Logic::InvalidOperationException">Not a 'foreach' macro</exception>
         /// <remarks>Expands 'for each $0 in array $1 [using counter $2]' to
         ///
         /// (iterator) = size of array (array)
         /// while (iterator)
         /// dec (iterator)
         /// (item_iterator) = (array)[(iterator)]</remarks>
         CommandNodeList  CommandTree::MacroExpander::ExpandForEach(CommandNode* n)
         {
            CommandNodeList  nodes;
            ParameterArray   params,
                             postfix;

            // Require 'for each $0 in array $1' or 'for each $0 in array $1 using counter $2'
            if (!n->Is(MACRO_FOR_EACH) && !n->Is(MACRO_FOR_EACH_COUNTER))
               throw InvalidOperationException(HERE, L"Command must be a 'foreach' macro");
            
            // Lookup components
            const wchar *item  = n->Parameters[0].Text.c_str(),
                        *array = n->Parameters[1].Text.c_str();

            // Iterator: Generate unique name if not specified by user
            GuiString iterator = n->Is(MACRO_FOR_EACH_COUNTER) ? n->Parameters[2].Text : IteratorNames.GetNext();
            if (!n->Is(MACRO_FOR_EACH_COUNTER))
               Script.Variables.Add(iterator.TrimLeft(L"$"));

            // (iterator) = size of array (array)
            VString init(L"%s = size of array %s", iterator.c_str(), array);
            nodes += ExpandCommand(n, init, Script.Game);

            // while (iterator)
            VString guard(L"while %s", iterator.c_str());
            nodes += ExpandCommand(n, guard, Script.Game);     

            // dec (iterator)
            VString advance(L"dec %s", iterator.c_str());
            nodes.back()->Add( ExpandCommand(n, advance, Script.Game) );    // Add as child of 'while'

            // (item_iterator) = (array)[(iterator)]
            VString access(L"%s = %s[%s]", item, array, iterator.c_str());
            nodes.back()->Add( ExpandCommand(n, access, Script.Game) );     // Add as child of 'while'

            // Move all children of 'foreach' to 'while' expression
            n->MoveChildren(*nodes.back());

            return nodes;
         }


         // ------------------------------- PRIVATE METHODS ------------------------------
      }
   }
}
