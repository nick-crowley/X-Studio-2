#include "stdafx.h"
#include "CommandTree.h"
#include "ScriptFile.h"
#include "ScriptFileReader.h"

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         // -------------------------------- CONSTRUCTION --------------------------------
         
         /// <summary>Create visitor for identifying variables</summary>
         /// <param name="s">script</param>
         /// <param name="e">errors collection</param>
         CommandTree::VariableIdentifier::VariableIdentifier(ScriptFile& s, ErrorArray& e) : Script(s), Errors(e)
         {
         }
         
         /// <summary>Nothing</summary>
         CommandTree::VariableIdentifier::~VariableIdentifier()
         {
         }
         
         // ------------------------------- STATIC METHODS -------------------------------

         // ------------------------------- PUBLIC METHODS -------------------------------
         
         /// <summary>Maps each variable name to a unique ID, and locates all label definitions</summary>
         /// <param name="n">Node</param>
         void  CommandTree::VariableIdentifier::VisitNode(CommandTree* n) 
         {
            typedef reference_wrapper<ScriptParameter>  ParameterRef;

            // Do not enumerate the labels/variables of command comments  [But do include script-calls]
            if (!n->CmdComment)
            {
               // Add label definitions to script
               if (n->Is(CMD_DEFINE_LABEL) && !n->Parameters.empty()) 
               {
                  auto name = n->Parameters[0].Value.String;
                  // Ensure unique
                  if (!Script.Labels.Add(name, n->LineNumber))
                     Errors += n->MakeError(VString(L"Label '%s' already defined on line %d", name.c_str(), Script.Labels[name].LineNumber), n->Parameters[0].Token);
               }

               list<ParameterRef> params;

   #ifdef VALIDATION
               // For the sake of producing code that exactly duplicates egosoft code, build the variable names map
               // by enumerating variables in physical syntax order. (Requires all parameters be present)
               if (n->Parameters.size() == n->Syntax.ParameterCount)
                  for (const auto& ps : n->Syntax.Parameters)
                     params.push_back( ref(n->Parameters[ps.DisplayIndex]) );
               else 
   #endif
                  // Missing/Discarded/vArgs: Enumerate in display order
                  for (auto& p : n->Parameters)
                     params.push_back(ref(p));

               // Enumerate variables
               for (auto& ref : params)
               {
                  auto& p = ref.get();
                  
                  // Variable: Lookup/Store variable and increment usage
                  if (p.Type == DataType::VARIABLE && p.Value.Type == ValueType::String)
                     Script.Variables.Add(p.Value.String).Usage++;
               }
            }

            // Load script-calls for argument type-checking
            if (n->Syntax.IsScriptCall())
            {
               // Find name of target script (empty if a variable)
               wstring call = n->GetScriptCallName();

               try {  
                  // Skip reading if previously read
                  if (!call.empty() && !Script.ScriptCalls.Contains(call))
                     Script.ScriptCalls.Add(call, ScriptFileReader::ReadExternalScript(Script.FullPath.Folder, call));
               }
               catch (ExceptionBase&) {
                  //if (e.ErrorID != ERROR_FILE_NOT_FOUND)
                     //Console.Log(HERE, e, VString(L"Unable to resolve call to external script '%s'", call.c_str()));
               }
            }
         }
         
         
         // ------------------------------ PROTECTED METHODS -----------------------------

         // ------------------------------- PRIVATE METHODS ------------------------------
      }
   }
}
