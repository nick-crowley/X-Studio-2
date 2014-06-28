#include "stdafx.h"
#include "CommandNode.h"
#include "ScriptFile.h"
#include "GameObjectLibrary.h"
#include "ScriptObjectLibrary.h"
#include "PreferencesLibrary.h"

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         // -------------------------------- CONSTRUCTION --------------------------------
         
         /// <summary>Create command verifying visitor</summary>
         /// <param name="s">script</param>
         /// <param name="e">errors collection</param>
         CommandNode::CommandVerifier::CommandVerifier(ScriptFile& s, ErrorArray& e) : Errors(e), Script(s)
         {
         }

         /// <summary>Nothing</summary>
         CommandNode::CommandVerifier::~CommandVerifier()
         {
         }

         // ------------------------------- STATIC METHODS -------------------------------

         // ------------------------------- PUBLIC METHODS -------------------------------

         /// <summary>Generates a ScriptCommand from the node</summary>
         /// <param name="n">Node</param>
         void  CommandNode::CommandVerifier::VisitNode(CommandNode* n) 
         {
            try
            {
               // Skip for unrecognised commands
               if (n->Syntax != CommandSyntax::Unrecognised)
               {
                  // CmdComment: Redirect verification errors into dummy queue
                  ErrorArray  commentErrors;
                  ErrorArray& errQueue = (n->CmdComment ? commentErrors : Errors);

                  // Check for invalid 'start' 
                  if (n->Condition == Conditional::START && n->Syntax.Execution == ExecutionType::Serial)
                     errQueue += n->MakeError(L"Command cannot be executed asynchronously");

                  // Check for missing 'start'
                  else if (n->Condition != Conditional::START && n->Syntax.Execution == ExecutionType::Concurrent)
                     errQueue += n->MakeError(L"Command must be executed asynchronously");

                  // Check for macro within 'skip-if'
                  if (n->Syntax.Is(CommandType::Macro) && n->Parent->Logic == BranchLogic::SkipIf)
                     errQueue += n->MakeError(L"Macros cannot be used within skip-if conditional");

                  // Verify parameter values/types
                  UINT index = 0;
                  for (const ScriptParameter& p : n->Parameters)
                     VerifyParameter(n, p, index++, errQueue);

                  // VARG: Verify argument count     [Genuine CallScript commands can have unlimited arguments]
                  if (n->Syntax.IsVArgument() && !n->Syntax.Is(CMD_CALL_SCRIPT) && !n->CmdComment)
                     if (n->Parameters.size() > n->Syntax.MaxParameters)
                        errQueue += n->MakeError(VString(L"Command may only have up to %d variable arguments", n->Syntax.VArgCount));

                  // Error in CmdComment: Silently revert to ordinary comment
                  if (n->CmdComment && !errQueue.empty())
                  {
                     // Replace self with commented node + delete self.
                     n->Parent->RevertCommandComment(n);    // NB: CmdComments can never have children, their branch logic is always
                     return;                                //     NOP even if they have a conditional, so no need to check children.
                  }
               }
            }
            catch (ExceptionBase& e) {
               Errors += n->MakeError(GuiString(L"Verification failed: ") + e.Message); 
            }
         }

         // ------------------------------ PROTECTED METHODS -----------------------------

         /// <summary>Verifies object/label names, and performs static type checking on standard and variable arguments</summary>
         /// <param name="n">node to verify</param>
         /// <param name="p">parameter to verify</param>
         /// <param name="index">parameter display index</param>
         /// <param name="err">active or dummy errors collection</param>
         void  CommandNode::CommandVerifier::VerifyParameter(CommandNode* n, const ScriptParameter& p, UINT index, ErrorArray& err) 
         {
            GameObjectLibrary::ObjectID obj;

            // Recognise game/script objects
            switch (p.Token.Type)
            {
            // GameObject: Ensure exists  (allow {SSTYPE_LASER@12} placeholders)
            case TokenType::GameObject:
               if (!GameObjectLib.Contains(p.Value.String) && !GameObjectLib.ParsePlaceholder(p.Value.String, obj))
                  err += n->MakeError(L"Unrecognised game object", p.Token);
               break;

            // ScriptObject: Ensure exists 
            case TokenType::ScriptObject:
               if (!ScriptObjectLib.Contains(p.Value.String))
                  err += n->MakeError(L"Unrecognised script object", p.Token);
               break;

            // Label: Ensure exists  [Don't check if commented]
            case TokenType::Label:
               if (!n->CmdComment)
                  if (!Script.Labels.Contains(p.Value.String))
                     err += n->MakeError(L"Unrecognised label", p.Token);
               break;
            }

            // Varg: Check param vs. target script arguments
            if (p.Syntax == ParameterSyntax::VArgParameter)
            {
               // ScriptCall: 
               if (n->Syntax.IsScriptCall())
               {  
                  // Find scriptName parameter
                  auto callName = n->GetScriptCallName();
                     
                  // Skip check if script-name present + script loaded
                  if (!callName.empty() && Script.ScriptCalls.Contains(callName))
                  {
                     auto call = Script.ScriptCalls.Find(callName);
                     auto argIndex = index - n->Syntax.ParameterCount;

                     // Verify argument exists
                     if (argIndex >= call.Variables.Arguments.Count)
                     {
                        // Create token representing argument name/value pair
                        ScriptToken tok(TokenType::Text, p.ArgName.Start, p.Token.End, p.ArgName.Text + L"=" + p.Token.Text);
                        err += n->MakeError(VString(L"'%s' only has %d arguments", callName.c_str(), call.Variables.Arguments.Count), tok);
                     }
                     // Verify argument name
                     else if (PrefsLib.CheckArgumentNames && !call.Variables.Contains(p.ArgName.Text))
                        err += n->MakeError(VString(L"'%s' does not have a '%s' argument", callName.c_str(), p.ArgName.Text.c_str()), p.ArgName);

#ifndef STRICT_VALIDATION   // Skip these checks when validating, some Egosoft scripts have missing script-call arguments
                     // Verify argument type
                     else if (PrefsLib.CheckArgumentTypes)
                     {
                        auto arg = call.Variables[p.ArgName.Text];
                     
                        // Verify argument order
                        if (arg.ID != argIndex)
                           err += n->MakeError(VString(L"argument out of order: '%s' must be at index %d", arg.Name.c_str(), arg.ID+1), p.ArgName);

                        // Verify argument type 
                        if (!ParameterSyntax::Verify(arg.ParamType, p.Type))
                           err += n->MakeError(VString(L"type mismatch - '%s' is not a valid %s", p.Text.c_str(), ::GetString(arg.ParamType).c_str()), p.Token);
                     }
#endif
                  }
               }
            }
            // Std parameter: Check value vs. type
            else if (!p.Syntax.Verify(p.Type))
               err += n->MakeError(VString(L"type mismatch - '%s' is not a valid %s", p.Text.c_str(), ::GetString(p.Syntax.Type).c_str()), p.Token);
         }

         // ------------------------------- PRIVATE METHODS ------------------------------
      }
   }
}
