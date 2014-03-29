#include "stdafx.h"
#include "ScriptCommand.h"
#include "ScriptFile.h"
#include "CommandLexer.h"
#include "SyntaxLibrary.h"
#include <algorithm>

namespace Logic
{
   namespace Scripts
   {
      /// <summary>Unrecognised script command sentinel value</summary>
      ScriptCommand  ScriptCommand::Unrecognised(nullptr);

      /// <summary>Determines whether parameter usage is scriptName</summary>
      /// <param name="s">parameter</param>
      const ParameterPredicate  ScriptCommand::IsScriptNameParam = [](const ScriptParameter& s) 
      {
         return s.Syntax.Usage == ParameterUsage::ScriptName;
      };
      
      /// <summary>Determines whether parameter usage is StringID</summary>
      /// <param name="s">parameter</param>
      const ParameterPredicate  ScriptCommand::IsStringIDParam = [](const ScriptParameter& s) 
      {
         return s.Syntax.Usage == ParameterUsage::StringID;
      };

      /// <summary>Determines whether parameter usage is PageID</summary>
      /// <param name="s">parameter</param>
      const ParameterPredicate  ScriptCommand::IsPageIDParam = [](const ScriptParameter& s) 
      {
         return s.Syntax.Usage == ParameterUsage::PageID;
      };

      /// <summary>Determines whether value of argument name-value pair is DT_NULL</summary>
      const ArgumentPairPredicate  ScriptCommand::IsArgumentNull = [](const ArgumentPair& a) 
      { 
         return a.second.Type == DataType::Null; 
      };

      /// <summary>Determines whether script parameter is DT_NULL</summary>
      /// <param name="s">parameter</param>
      const ParameterPredicate  ScriptCommand::IsParameterNull = [](const ScriptParameter& s) 
      {
         return s.Type == DataType::Null;
      };

      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Creates sentinel value (private ctor)</summary>
      /// <param name="unnamed">Required to make signature unique from default ctor</param>
      ScriptCommand::ScriptCommand(void*)
         : Syntax(CommandSyntax::Unrecognised), RefIndex(0)
      {
      }

      /// <summary>Create a command PARSED FROM COMMAND TEXT</summary>
      /// <param name="text">Entire command text</param>
      /// <param name="syntax">Command syntax</param>
      /// <param name="params">Parameters in DISPLAY ORDER</param>
      /// <param name="cmdComment">Whether command is commented</param>
      ScriptCommand::ScriptCommand(const wstring& text, CommandSyntaxRef syntax, ParameterArray& params, bool cmdComment)
         : Syntax(syntax), RefIndex(0), Text(text), Commented(cmdComment)
      {
         // Re-order parameters by physical index
         for (auto& ps : Syntax.Parameters)
            Parameters += params[ps.DisplayIndex];

         // VARGS: Append arguments [+trailing NULL if appropriate]
         if (Syntax.IsVArgument())
         {
            // SCRIPT-CALL: Count
            if (Syntax.Is(CMD_CALL_SCRIPT))
               Parameters += ScriptParameter(ParameterSyntax::StructuralCount, DataType::INTEGER, params.size()-Syntax.ParameterCount);

            // Append supplied parameters followed by DT_NULL parameters
            for (UINT i = Syntax.ParameterCount; i < Syntax.MaxParameters; ++i)
            {
               // Present: Supply parameter
               if (i < params.size())
                  Parameters += params[i]; 

               // Missing: Append remaining DT_NULL parameters or stop
               else if (Syntax.VArgParams == VArgMethod::PadNull)
                  Parameters += ScriptParameter(ParameterSyntax::VArgParameter, DataType::Null, 0);
               else
                  break;   
            }
         }
      }

      /// <summary>Create an expression PARSED FROM COMMAND TEXT</summary>
      /// <param name="text">Entire expression text</param>
      /// <param name="syntax">Expression syntax</param>
      /// <param name="infix">RetVar + Parameters in INFIX ORDER</param>
      /// <param name="postfix">Parameters in POSTFIX ORDER</param>
      /// <param name="cmdComment">Whether expression is commented</param>
      ScriptCommand::ScriptCommand(const wstring& text, CommandSyntaxRef syntax, ParameterArray& infix, ParameterArray& postfix, bool cmdComment)
         : Syntax(syntax), RefIndex(0), Text(text), Commented(cmdComment)
      {
         // RetVar
         Parameters += infix[0];

         // Commented Expression: Supply infix tuples only
         if (Commented)
         {
            // Infix Count:  (minus RetVar)
            Parameters += ScriptParameter(ParameterSyntax::StructuralCount, DataType::INTEGER, infix.size()-1);

            // Infix Parameter tuples
            for (UINT i = 1; i < infix.size(); ++i)
               Parameters += infix[i];
         }
         else
         {
            // Postfix Count:
            Parameters += ScriptParameter(ParameterSyntax::StructuralCount, DataType::INTEGER, postfix.size());

            // Postfix: Append verbatim   (Syntax is 'ExpressionParameter')
            Parameters += postfix;
                     
            // Infix Count:  (minus RetVar)
            Parameters += ScriptParameter(ParameterSyntax::StructuralCount, DataType::INTEGER, infix.size()-1);

            // Infix: Use 'InfixParameter' syntax to define single nodes.  +ve == operator, -ve == 1-based index into postfix array
            for (UINT i = 1; i < infix.size(); ++i)
               if (infix[i].Type == DataType::OPERATOR || infix[i].Type == DataType::UNARY_OPERATOR)
                  // Operator: Preserve type/value, change syntax
                  Parameters += ScriptParameter(ParameterSyntax::InfixParameter, infix[i].Type, infix[i].Value);
               else
               {
                  // Lookup postfix param
                  if (find(postfix.begin(), postfix.end(), infix[i]) == postfix.end())
                     throw AlgorithmException(HERE, L"Unable to find postfix parameter");

                  // Operand: Calculate Negative 1-based array index 
                  int index = find(postfix.begin(), postfix.end(), infix[i]) - postfix.begin();
                  Parameters += ScriptParameter(ParameterSyntax::InfixParameter, DataType::INTEGER, -(index+1));
               }
         }
      }

      /// <summary>Create a STANDARD command READ FROM A FILE</summary>
      /// <param name="syntax">command syntax.</param>
      /// <param name="params">Parameters in PHYSICAL ORDER</param>
      ScriptCommand::ScriptCommand(CommandSyntaxRef syntax, ParameterArray& params, bool comment)
         : Syntax(syntax), RefIndex(0), Parameters(params), Commented(comment)
      {
      }

      /// <summary>Create an AUXILIARY command READ FROM A FILE</summary>
      /// <param name="syntax">command syntax.</param>
      /// <param name="ref">Index of associated standard command</param>
      /// <param name="params">Parameters in PHYSICAL ORDER</param>
      ScriptCommand::ScriptCommand(CommandSyntaxRef syntax, UINT  ref, ParameterArray& params, bool comment)
         : Syntax(syntax), RefIndex(ref), Parameters(params), Commented(comment)
      {
      }
      
      ScriptCommand::~ScriptCommand()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      /// <summary>Gets BranchLogic string</summary>
      /// <param name="l">logic</param>
      /// <returns></returns>
      LogicExport const WCHAR* GetString(BranchLogic l)
      {
         static const WCHAR* str[] = {L"None", L"NOP", L"If", L"While", L"SkipIf", L"Else", L"ElseIf", L"End", L"Break", L"Continue"};
         return str[l];
      }

      // ------------------------------- PUBLIC METHODS -------------------------------
      
      /// <summary>Gets the jump destination of a goto/gosub command</summary>
      /// <returns></returns>
      /// <exception cref="Logic::InvalidOperationException">Command is not goto/gosub -or- label number parameter is missing</exception>
      UINT ScriptCommand::GetJumpDestination() const
      {
         // Validate command ID
         if (!Syntax.Is(CMD_GOTO_LABEL) && !Syntax.Is(CMD_GOTO_SUB))
            throw InvalidOperationException(HERE, VString(L"Cannot get jump destination for a '%s' command", Syntax.Text.c_str()));

         // Validate label name parameter
         if (Parameters.size() == 0 || Parameters[0].Value.Type != ValueType::Int)
            throw InvalidOperationException(HERE, GuiString(L"Missing jump destination parameter"));

         return Parameters[0].Value.Int;
      }
      
      /// <summary>Gets the name of the label defined or referenced by this command.</summary>
      /// <returns></returns>
      /// <exception cref="Logic::InvalidOperationException">Command is not label definition/reference -or- name parameter is missing</exception>
      wstring  ScriptCommand::GetLabelName() const
      {
         // Validate command ID
         if (!Syntax.Is(CMD_DEFINE_LABEL) && !Syntax.Is(CMD_GOTO_LABEL) && !Syntax.Is(CMD_GOTO_SUB))
            throw InvalidOperationException(HERE, VString(L"Cannot get label name for a '%s' command", Syntax.Text.c_str()));

         // Validate label name parameter
         if (Parameters.size() == 0 || Parameters[0].Value.Type != ValueType::String)
            throw InvalidOperationException(HERE, GuiString(L"Missing label name parameter"));

         // Return name
         return Parameters[0].Value.String;
      }

      /// <summary>Identify the type of branching conditional used (if any)</summary>
      /// <returns></returns>
      BranchLogic  ScriptCommand::GetLogic() const
      {
         // Command comments do not use 'CMD_COMMAND_COMMENT' ID on input, they are only distinguished by 'commented' flag
         if (Commented)
            return BranchLogic::NOP;

         switch (Syntax.ID)
         {
         case CMD_END:      return BranchLogic::End;
         case CMD_ELSE:     return BranchLogic::Else;
         case CMD_BREAK:    return BranchLogic::Break;
         case CMD_CONTINUE: return BranchLogic::Continue;

         case CMD_COMMAND_COMMENT:
         case CMD_COMMENT: 
         case CMD_NOP:     
            return BranchLogic::NOP;
         
         default:
            // Find Return parameter, if any
            auto it = find_if(Parameters.begin(), Parameters.end(), [](const ScriptParameter& p) { return p.Syntax.IsRetVar(); });
            
            // RetVar/Discard
            if (it == Parameters.end() || it->Value.Type == ValueType::String)
               return BranchLogic::None;

            // RetVar/Discard
            ReturnValue retVar(it->Value.Int);
            if (retVar.ReturnType == ReturnType::ASSIGNMENT || retVar.ReturnType == ReturnType::DISCARD)
               return BranchLogic::None;
               
            // Conditional
            switch (retVar.Conditional)
            {
            case Conditional::IF:      
            case Conditional::IF_NOT:        return BranchLogic::If;
            case Conditional::WHILE:         
            case Conditional::WHILE_NOT:     return BranchLogic::While;

            case Conditional::ELSE_IF:  
            case Conditional::ELSE_IF_NOT:   return BranchLogic::ElseIf;

            case Conditional::SKIP_IF:  
            case Conditional::SKIP_IF_NOT:   return BranchLogic::SkipIf;

            default:
               return BranchLogic::None;
            }
         }
      }

      /// <summary>Gets the name of target script</summary>
      /// <returns>Name of script, or empty string if name was a variable</returns>
      /// <exception cref="Logic::InvalidOperationException">Command is not a script call -or- name parameter is missing</exception>
      wstring  ScriptCommand::GetScriptCallName() const
      {
         // Validate Command ID: 
         if (!Syntax.IsScriptCall())
            throw InvalidOperationException(HERE, VString(L"Cannot get script name for a '%s' command", Syntax.Text.c_str()));

         // Lookup script-call parameter
         auto param = find_if(Parameters.begin(), Parameters.end(), IsScriptNameParam);

         // Ensure present
         if (param == Parameters.end()) 
            throw InvalidOperationException(HERE, GuiString(L"Missing script name parameter"));

         // Return name (or empty string if variable)
         return param->Type == DataType::STRING ? param->Value.String : L"";
      }
      
      /// <summary>Gets the ID of the string referenced by this command</summary>
      /// <returns></returns>
      /// <exception cref="Logic::InvalidOperationException">Command doesn't reference a string -or- parameter is missing</exception>
      StringID  ScriptCommand::GetStringReference() const
      {
         // Validate Command ID: 
         if (!Syntax.IsStringReference())
            throw InvalidOperationException(HERE, VString(L"Cannot get string reference for a '%s' command", Syntax.Text.c_str()));

         // Lookup stringID/pageID parameter
         auto id   = find_if(Parameters.begin(), Parameters.end(), IsStringIDParam),
              page = find_if(Parameters.begin(), Parameters.end(), IsPageIDParam);

         // Ensure both present
         if (id == Parameters.end() || page == Parameters.end()) 
            throw InvalidOperationException(HERE, GuiString(L"Missing string ID or page ID parameter"));

         // Return both
         return StringID(*page, *id);
      }
      
      /// <summary>Compare command ID</summary>
      /// <param name="ID">command id.</param>
      /// <returns></returns>
      bool  ScriptCommand::Is(UINT ID) const
      {
         return Syntax.ID == ID;
      }

      /// <summary>Compare command type</summary>
      /// <param name="t">command type.</param>
      /// <returns></returns>
      bool  ScriptCommand::Is(CommandType t) const
      {
         return Syntax.Type == t;
      }

      /// <summary>Finds an integer parameter, if any, at a given index</summary>
      /// <param name="index">Zero-based physical index.</param>
      /// <param name="val">On return this contains the integer.</param>
      /// <returns>True if found, otherwise False</returns>
      bool ScriptCommand::FindInteger(UINT index, int& val) const
      {
         val = 0;

         // Require DT_INTEGER
         if (index < Parameters.size() && Parameters[index].Type == DataType::INTEGER)
         {
            val = Parameters[index].Value.Int;
            return true;
         }

         return false;
      }
      
      /// <summary>Finds an operator parameter, if any, at a given index</summary>
      /// <param name="index">Zero-based physical index.</param>
      /// <param name="op">On return this contains the operator.</param>
      /// <returns>True if found, otherwise False</returns>
      bool ScriptCommand::FindOperator(UINT index, Operator& op) const
      {
         // Require DT_OPERATOR
         if (index < Parameters.size() && Parameters[index].Type == DataType::OPERATOR)
         {
            op = (Operator)Parameters[index].Value.Int;
            return true;
         }

         return false;
      }
      
      /// <summary>Finds a string parameter, if any, at a given index</summary>
      /// <param name="index">Zero-based physical index.</param>
      /// <param name="str">On return this contains the string text.</param>
      /// <returns>True if found, otherwise False</returns>
      bool ScriptCommand::FindString(UINT index, wstring& str) const
      {
         str.clear();

         // Require DT_STRING
         if (index < Parameters.size() && Parameters[index].Type == DataType::STRING)
         {
            str = Parameters[index].Value.String;
            return true;
         }

         return false;
      }

      /// <summary>Finds the return variable, if any</summary>
      /// <param name="varName">On return this contains the name of the variable [Without $ prefix]</param>
      /// <returns>True if found, otherwise False</returns>
      bool ScriptCommand::FindRetVar(wstring& varName) const
      {
         // Linear search. Require RetVar syntax + Variable value
         for (auto& p : Parameters)
            if (p.Syntax.IsRetVar() && p.IsVariable())
            {
               varName = GuiString(p.Text).Trim(L"$= ");
               return true;
            }

         return false;
      }
      
      /// <summary>Finds a variable parameter, if any, at a given index</summary>
      /// <param name="index">Zero-based physical index.</param>
      /// <param name="var">On return this contains the variable name [Without $ prefix]</param>
      /// <returns>True if found, otherwise False</returns>
      bool ScriptCommand::FindVariable(UINT index, wstring& var) const
      {
         // Require DT_VARIABLE
         if (index < Parameters.size() && Parameters[index].IsVariable())
         {
            var = GuiString(Parameters[index].Text).TrimLeft(L"$= ");
            return true;
         }

         return false;
      }

      /// <summary>Matches an 'alloc array' command and returns the array variable and array size</summary>
      /// <param name="varName">On return this contains the array variable.</param>
      /// <param name="size">On return this contains the allocation size.</param>
      /// <returns></returns>
      bool  ScriptCommand::MatchAllocArray(wstring& varName, int& size) const
      {
         size = 0;
         varName.clear();

         // Extract array variable and element count
         return Is(CMD_ARRAY_ALLOC) && FindRetVar(varName) && FindInteger(1, size);
      }


      /// <summary>Matches a 'for loop' initialization command.</summary>
      /// <param name="array">Name of array variable.</param>
      /// <param name="element">Required array index.</param>
      /// <param name="param">On return this contains the parameter assigned (if successful)</param>
      /// <returns></returns>
      bool  ScriptCommand::MatchAssignArray(const wstring& array, int element, const ScriptParameter*& param) const
      {
         wstring _array;
         int     _element;

         param = nullptr;

         // Ensure command has correct array variable and element index
         if (Is(CMD_ARRAY_ASSIGNMENT) 
          && FindVariable(0, _array) && array == _array 
          && FindInteger(1, _element) && element == _element)
         {
            // Extract assignment parameter
            param = &Parameters[2];
            return true;
         }

         return false;
      }
      

      /// <summary>Matches a 'for loop' initialization expression.</summary>
      /// <param name="iterator">On return this contains the name of iterator variable.</param>
      /// <param name="initial">On return this contains the initial value parameter</param>
      /// <param name="step">On return this contains the step value.</param>
      /// <returns></returns>
      /// <remarks>Matches '(iterator) = (iterator) ± (step_value)'</remarks>
      bool  ScriptCommand::MatchForLoopAdvance(const wstring& iterator, const int step) const
      {
         Operator op;
         wstring  _iterator;

         // Ensure 'while' expression with four components
         if (Is(CMD_EXPRESSION) && Parameters.size() == 4)
            // Match iterator variable
            if (FindVariable(1, _iterator) && iterator == _iterator && FindVariable(2, _iterator) && iterator == _iterator)
               // Match plus/minus
               if (FindOperator(2, op) && (step > 0 && op == Operator::Add || step < 0 && op == Operator::Subtract))
                  return true;

         return false;
      }
      
      /// <summary>Matches a 'for loop' conditional check.</summary>
      /// <param name="iterator">Name of iterator variable.</param>
      /// <param name="step">Step value.</param>
      /// <param name="limit">On return this contains the limit value parameter</param>
      /// <returns></returns>
      /// <remarks>Matches 'while (iterator) less/greater (final_value)'</remarks>
      bool  ScriptCommand::MatchForLoopCondition(const wstring& iterator, const int step, const ScriptParameter*& limit) const
      {
         Operator op;
         wstring _iterator;

         // Prepare
         limit = nullptr;

         // Ensure 'while' expression with four components
         if (Is(CMD_EXPRESSION) && Parameters.size() == 4 && Logic == BranchLogic::While)
            // Match iterator variable
            if (FindVariable(1, _iterator) && iterator == _iterator)
               // Match greater/less
               if (FindOperator(2, op) && (step > 0 && op == Operator::Less || step < 0 && op == Operator::Greater))
               {
                  // Extract limit value 
                  limit = &Parameters[3];
                  return true;
               }

         return false;
      }
      
      /// <summary>Matches a 'for loop' initialization expression.</summary>
      /// <param name="iterator">On return this contains the name of iterator variable.</param>
      /// <param name="initial">On return this contains the initial value parameter</param>
      /// <param name="step">On return this contains the step value.</param>
      /// <returns></returns>
      /// <remarks>Matches '(iterator) = (inital_value) ± (step_value)'</remarks>
      bool  ScriptCommand::MatchForLoopInitialize(wstring& iterator, const ScriptParameter*& initial, int& step) const
      {
         Operator op;

         // Prepare
         initial = nullptr;
         iterator.clear();
         step = 0;

         // Ensure expression has four components
         if (Is(CMD_EXPRESSION) && Parameters.size() == 4)
            // Ensure step is an integer and operator is ±
            if (FindInteger(3, step) && FindOperator(2, op) && (op == Operator::Add || op == Operator::Subtract))
            {
               // Extract initial value 
               initial = &Parameters[1];
               return true;
            }

         return false;
      }
      
      /// <summary>Replaces a label number parameter with a label name parameter.</summary>
      /// <param name="name">The name of the label</param>
      /// <exception cref="Logic::InvalidOperationException">Command is not goto/gosub</exception>
      void  ScriptCommand::SetLabelName(const wstring& name)
      {
         // Validate command ID
         if (!Syntax.Is(CMD_GOTO_LABEL) && !Syntax.Is(CMD_GOTO_SUB))
            throw InvalidOperationException(HERE, VString(L"Cannot set label name for a '%s' command", Syntax.Text.c_str()));

         // Replace parameters
         Parameters.clear();
         Parameters.push_back( ScriptParameter(ParameterSyntax::LabelNameParameter, DataType::STRING, name) );
      }

      /// <summary>Translates the text of a command READ FROM FILE</summary>
      /// <param name="f">Script used for variable names lookup</param>
      void ScriptCommand::Translate(ScriptFile& f)
      {
         CommandLexer lex(Syntax.Text, false);
         bool         Param = false;

         // Translate parameters
         for (ScriptParameter& p : Parameters)
            p.Translate(f);

         // Insert/Replace syntax '$n' markers with parameter text
         for (const ScriptToken& tok : lex.Tokens)
         {
            // Marker: Insert parameter text
            if (tok.Type == TokenType::Variable || tok.Type == TokenType::Comment)  // Lexer identifies comment syntax as 'comment'
            {
               int i = (tok.Text[1]-48);
               Text.append( Parameters[i].Text );
            }
            else // Text: Insert verbatim
               Text.append(tok.Text);
         }
         // Trim leading spaces
         Text = Text.TrimLeft(L" ");

         // Expressions: Print components
         if (Is(CMD_EXPRESSION))
            for (UINT i = 2; i < Parameters.size(); ++i)
               Text.append(Parameters[i].Text);

         // Varg: Print pairs/delimited arguments
         else if (Syntax.IsVArgument())
         {
            vector<ArgumentPair> vargs;
            wstring script;

            switch (Syntax.VArgument)
            {
            // CommaDelimited: Print delimited list
            case VArgSyntax::CommaDelimited:
               // Iterate thru varg parameters
               for (UINT p = Syntax.ParameterCount; p < Parameters.size(); ++p)
                  // Drop 'null' arguments iff remainder are 'null' 
                  if (!all_of(Parameters.begin()+p, Parameters.end(), IsParameterNull))
                     Text.append( VString(p==Syntax.ParameterCount ? L" %s" : L", %s", Parameters[p].Text.c_str()) );
               break;

            // ScriptCalls: Print argument name/value pairs
            case VArgSyntax::NamedPairs:
               // Get script name
               script = GetScriptCallName();
            
               // Populate argument name/value pairs   [start at first varg parameter]
               for (UINT p = Syntax.ParameterCount, arg = 0; p < Parameters.size(); ++p, ++arg)
                  vargs.push_back( ArgumentPair(f.ScriptCalls.FindArgumentName(script, arg), Parameters[p]) );

               // Append text
               for (auto it = vargs.begin(); it != vargs.end(); ++it)
#ifndef STRICT_VALIDATION
                  // Drop 'null' arguments iff remainder are 'null' 
                  if (!all_of(it, vargs.end(), IsArgumentNull))
                     Text.append( VString(L" %s=%s", it->first.c_str(), it->second.Text.c_str()) );
#else
                  // Drop 'null' arguments iff remainder are 'null' (except for genuine 102 varg ScriptCalls)
                  if (Is(CMD_CALL_SCRIPT) || !all_of(it, vargs.end(), IsArgumentNull) )
                     Text.append( VString(L" %s=%s", it->first.c_str(), it->second.Text.c_str()) );
#endif
               break;
            }
         }
         
         // Concurrent: Insert 'start' keyword
         if (Syntax.Execution == ExecutionType::Concurrent)
            Text.insert(0, L"start ");

         // Commented: Insert comment operator
         if (Commented)
            Text.insert(0, L"* ");
      }

		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

   }
}

