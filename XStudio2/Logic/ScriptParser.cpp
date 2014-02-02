#include "stdafx.h"
#include "ScriptParser.h"
#include "ExpressionParser.h"
#include "GameObjectLibrary.h"
#include "ScriptObjectLibrary.h"
#include "CommandHash.h"

//#define PRINT_CONSOLE

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         // -------------------------------- CONSTRUCTION --------------------------------

         /// <summary>Creates a script parser</summary>
         /// <param name="lines">The lines to parse</param>
         /// <param name="v">The game version</param>
         /// <exception cref="Logic::ArgumentException">Line array is empty</exception>
         ScriptParser::ScriptParser(const LineArray& lines, GameVersion  v) : Input(lines), Version(v), Script(new CommandNode())
         {
            if (lines.size() == 0)
               throw ArgumentException(HERE, L"lines", L"Line count cannot be zero");

            // Parse input
            CurrentLine = Input.begin();
            ParseRoot();
         }


         ScriptParser::~ScriptParser()
         {
         }

         // ------------------------------- STATIC METHODS -------------------------------
         
         /// <summary>Generate an error for the entire line</summary>
         /// <param name="msg">The message</param>
         /// <param name="lex">The lexer</param>
         /// <returns></returns>
         ErrorToken  ScriptParser::MakeError(const wstring& msg, const CommandLexer& lex) const
         {
            return ErrorToken(msg, LineNumber, lex.count()?lex.begin()->Start:0, CurrentLine->length()-1);
         }

         /// <summary>Generate an error for a single token</summary>
         /// <param name="msg">The message</param>
         /// <param name="tok">The token</param>
         /// <returns></returns>
         ErrorToken  ScriptParser::MakeError(const wstring& msg, const TokenIterator& tok) const
         {
            return ErrorToken(msg, LineNumber, tok->Start, tok->End);
         }

         // ------------------------------- PUBLIC METHODS -------------------------------

         // ------------------------------ PROTECTED METHODS -----------------------------

         // ------------------------------- PRIVATE METHODS ------------------------------
         
         /// <summary>Reads the next node but returns the current node</summary>
         /// <returns>Node that was current before call</returns>
         ScriptParser::CommandNodePtr  ScriptParser::Advance()
         {
            CommandNodePtr tmp(CurrentNode);
            CurrentNode = ReadLine();
            return tmp;
         }
         
         /// <summary>Get the one-based line number of the current line</summary>
         /// <returns>One based line number</returns>
         UINT  ScriptParser::GetLineNumber() const
         {
            return CurrentLine - Input.begin() + 1;
         }

         /// <summary>Reads all commands in the script</summary>
         /// <returns></returns>
         /// <exception cref="Logic::ArgumentException">Error in parsing algorithm</exception>
         /// <exception cref="Logic::InvalidOperationException">Error in parsing algorithm</exception>
         void  ScriptParser::ParseRoot()
         {
            // Read first command
            Advance();

            while (CurrentNode)
            {  
               // Examine logic
               switch (CurrentNode->Logic)
               {
               // If: Add
               case BranchLogic::If:      
               case BranchLogic::While:  
                  ParseIf(Script->Add(Advance()));
                  break;
               
               // SkipIf: Add
               case BranchLogic::SkipIf:  
                  ParseSkipIf(Script->Add(Advance()));
                  break;

               // Else/Else-if: Add  (Invalid)
               case BranchLogic::ElseIf:  
               case BranchLogic::Else:    
                  ParseElse(Script->Add(Advance()));
                  break;
               
               // Command/NOP/Break/Continue/End: Add 
               default:
                  Script->Add(Advance());
                  break;
               }
            }
            
            // Verify tree
            //Script->Print(0);
            Script->Verify(Errors);
         }

         /// <summary>Reads current 'if' command and all descendants including 'end'</summary>
         /// <returns></returns>
         /// <exception cref="Logic::ArgumentException">Error in parsing algorithm</exception>
         /// <exception cref="Logic::InvalidOperationException">Error in parsing algorithm</exception>
         void ScriptParser::ParseIf(CommandNodePtr& If)
         {
            while (CurrentNode)
            {  
               // Examine command
               switch (CurrentNode->Logic)
               {
               // Command/NOP/Break/Continue: Add 
               default: 
                  If->Add(Advance());
                  break;

               // If: Add
               case BranchLogic::If:      
               case BranchLogic::While:  
                  ParseIf(If->Add(Advance()));  
                  break;

               // SkipIf: Add
               case BranchLogic::SkipIf:  
                  ParseSkipIf(If->Add(Advance()));  
                  break;

               // Else/Else-if: Add
               case BranchLogic::ElseIf:  
               case BranchLogic::Else:    
                  ParseElse(If->Add(Advance()));  
                  break;

               // End: Add/Stop
               case BranchLogic::End: 
                  If->Add(Advance());
                  return;
               }
            }
         }

         /// <summary>Reads current 'else' command and all descendants</summary>
         /// <returns></returns>
         /// <exception cref="Logic::ArgumentException">Error in parsing algorithm</exception>
         /// <exception cref="Logic::InvalidOperationException">Error in parsing algorithm</exception>
         void ScriptParser::ParseElse(CommandNodePtr& Else)
         {
            while (CurrentNode)
            {  
               // Examine command
               switch (CurrentNode->Logic)
               {
               // Command/NOP/Break/Continue: Add 
               default: 
                  Else->Add(Advance());
                  break;

               // If: Add
               case BranchLogic::If:      
               case BranchLogic::While:  
                  ParseIf(Else->Add(Advance()));  
                  break;

               // SkipIf: Add
               case BranchLogic::SkipIf:  
                  ParseSkipIf(Else->Add(Advance()));  
                  break;

               // End/Else/Else-if: Stop
               case BranchLogic::ElseIf:  
               case BranchLogic::Else:    
               case BranchLogic::End: 
                  return;
               }
            }
         }

         /// <summary>Reads current 'skip-if' command and all descendants</summary>
         /// <returns></returns>
         /// <exception cref="Logic::ArgumentException">Error in parsing algorithm</exception>
         /// <exception cref="Logic::InvalidOperationException">Error in parsing algorithm</exception>
         void ScriptParser::ParseSkipIf(CommandNodePtr& SkipIf)
         {
            // Read children
            while (CurrentNode)
            {
               // Examine command
               switch (CurrentNode->Logic)
               {
               // NOP: Add
               case BranchLogic::NOP:
                  SkipIf->Add(Advance());
                  break;

               // Command/Break/Continue: Add/Stop
               default: 
                  SkipIf->Add(Advance());
                  return;

               // If: Add/Stop  (Invalid)
               case BranchLogic::If:      
               case BranchLogic::While:  
                  ParseIf(SkipIf->Add(Advance()));  
                  return;

               // SkipIf: Add/Stop  (Invalid)
               case BranchLogic::SkipIf:  
                  ParseSkipIf(SkipIf->Add(Advance()));  
                  return;

               // Else/Else-if: Add/Stop (invalid)
               case BranchLogic::ElseIf:  
               case BranchLogic::Else:    
                  ParseElse(SkipIf->Add(Advance()));  
                  return;

               // End: Add/Stop (Invalid)
               case BranchLogic::End: 
                  SkipIf->Add(Advance());
                  return;
               }
            }
         }

         
         /// <summary>Matches an array variable and opening index operator</summary>
         /// <param name="lex">The lexer</param>
         /// <param name="pos">The start position.</param>
         /// <returns></returns>
         /// <remarks>The iterator is advanced beyond last matched token if successful, otherwise it is not moved</remarks>
         bool ScriptParser::MatchArrayIndex(const CommandLexer& lex, TokenIterator& pos) const
         {
            TokenIterator start = pos;

            // variable '['
            return lex.Match(pos, TokenType::Variable) && lex.Match(pos, TokenType::Operator, L"[") ? true : (pos=start, false);
         }
         
         /// <summary>Matches a return value and assignment operator</summary>
         /// <param name="lex">The lexer</param>
         /// <param name="pos">The start position.</param>
         /// <returns></returns>
         /// <remarks>The iterator is advanced beyond last matched token if successful, otherwise it is not moved</remarks>
         bool  ScriptParser::MatchAssignment(const CommandLexer& lex, TokenIterator& pos) const
         {
            TokenIterator start = pos;

            // Assignment: variable '='
            return lex.Match(pos, TokenType::Variable) && lex.Match(pos, TokenType::Operator, L"=") ? true : (pos=start, false);
         }

         /// <summary>Matches a valid conditional</summary>
         /// <param name="lex">The lexer</param>
         /// <param name="pos">The start position.</param>
         /// <returns></returns>
         /// <remarks>The iterator is advanced beyond last matched token if successful, otherwise it is not moved</remarks>
         bool  ScriptParser::MatchConditional(const CommandLexer& lex, TokenIterator& pos) const
         {
            TokenIterator start = pos;

            // If/while not?
            if (lex.Match(pos, TokenType::Keyword, L"if") || lex.Match(pos, TokenType::Keyword, L"while"))
            {
               lex.Match(pos, TokenType::Keyword, L"not");
               return true;
            }

            // SkipIf/DoIf not?
            if (lex.Match(pos, TokenType::Keyword, L"skip") || lex.Match(pos, TokenType::Keyword, L"do"))
               if (lex.Match(pos, TokenType::Keyword, L"if"))
               {
                  lex.Match(pos, TokenType::Keyword, L"not");
                  return true;
               }

            // ElseIf not?
            if (lex.Match(pos, TokenType::Keyword, L"else") && lex.Match(pos, TokenType::Keyword, L"if"))
            {
               lex.Match(pos, TokenType::Keyword, L"not");
               return true;
            }

            // Failed
            return (pos=start, false);
         }

         /// <summary>Matches a reference object and reference operator</summary>
         /// <param name="lex">The lexer</param>
         /// <param name="pos">The start position.</param>
         /// <returns></returns>
         /// <remarks>The iterator is advanced beyond last matched token if successful, otherwise it is not moved</remarks>
         bool ScriptParser::MatchReferenceObject(const CommandLexer& lex, TokenIterator& pos) const
         {
            TokenIterator start = pos;

            // (constant/variable/null '->')?
            return (lex.Match(pos, TokenType::ScriptObject) || lex.Match(pos, TokenType::Variable) || lex.Match(pos, TokenType::Null))
                 && lex.Match(pos, TokenType::Operator, L"->") ? true : (pos=start, false);
         }

         /// <summary>Matches a script argument {name,value} pair</summary>
         /// <param name="lex">The lexer</param>
         /// <param name="pos">The start position.</param>
         /// <returns></returns>
         /// <remarks>The iterator is advanced beyond last matched token if successful, otherwise it is not moved</remarks>
         bool ScriptParser::MatchScriptArgument(const CommandLexer& lex, TokenIterator& pos) const
         {
            TokenIterator start = pos;

            // (text '=' constant/variable/null/string/number)?
            return lex.Match(pos, TokenType::Text) && lex.Match(pos, TokenType::Operator, L"=") &&
                   (lex.Match(pos, TokenType::ScriptObject) 
                    || lex.Match(pos, TokenType::GameObject) 
                    || lex.Match(pos, TokenType::Variable)
                    || lex.Match(pos, TokenType::Number)
                    || lex.Match(pos, TokenType::String)
                    || lex.Match(pos, TokenType::Null))
                   ? true : (pos=start, false);
         }

         /// <summary>Matches a NOP or comment command</summary>
         /// <param name="lex">The lexer</param>
         /// <returns></returns>
         bool  ScriptParser::MatchComment(const CommandLexer& lex) const
         {
            TokenIterator pos = lex.begin();

            // Nop:
            if (lex.count() == 0)
               return true;

            // Comment: '*' text?
            if (lex.Match(pos, TokenType::Operator, L"*"))
               return lex.count() == 1 || (lex.Match(pos, TokenType::Comment) && lex.count() == 2);

            // Failed
            return false;
         }

         /// <summary>Matches a non-expression command</summary>
         /// <param name="lex">The lexer</param>
         /// <returns></returns>
         bool  ScriptParser::MatchCommand(const CommandLexer& lex) const
         {
            TokenIterator pos = lex.begin();
            
            // command = (assignment/conditional)? (constant/variable/null '->')? text/keyword/label

            // (assignment/conditional)?
            if (!MatchAssignment(lex, pos))
               MatchConditional(lex, pos);

            // (constant/variable/null '->')?
            MatchReferenceObject(lex, pos);

            // text/keyword/label/array-assignment
            return lex.Match(pos, TokenType::Text) || lex.Match(pos, TokenType::Keyword) || lex.Match(pos, TokenType::Label) || MatchArrayIndex(lex,pos);
         }

         /// <summary>Matches an expression command</summary>
         /// <param name="lex">The lexer</param>
         /// <returns></returns>
         bool ScriptParser::MatchExpression(const CommandLexer& lex) const
         {
            TokenIterator pos = lex.begin();

            // value = variable/literal/null
            // expression = (assignment/conditional) unary_operator? value (operator value)*

            // (assignment/conditional)
            if (!MatchAssignment(lex, pos) && !MatchConditional(lex, pos))
               return false;

            // Unary_operator?
            TokenIterator dummy = pos;
            if (lex.Match(dummy, TokenType::Operator, L"!") 
             || lex.Match(dummy, TokenType::Operator, L"-") 
             || lex.Match(dummy, TokenType::Operator, L"~")
             || lex.Match(dummy, TokenType::Operator, L"("))      // allow open bracket
               ++pos;

            // Value        
            if (lex.Match(pos, TokenType::Number) 
             || lex.Match(pos, TokenType::String) 
             || lex.Match(pos, TokenType::Variable)
             || lex.Match(pos, TokenType::Null)
             || lex.Match(pos, TokenType::GameObject) 
             || lex.Match(pos, TokenType::ScriptObject) )
               // (operator value)*   
               return pos == lex.end() 
                  || (lex.Match(pos, TokenType::Operator) && (pos-1)->Text != L"->");  // any non-refObj operator
            
            // Failed
            return false;
         }




         /// <summary>Reads the retVar and assignment operator tokens</summary>
         /// <param name="lex">The lexer</param>
         /// <param name="pos">The start position.</param>
         /// <returns>RetVar token</returns>
         /// <remarks>The iterator is advanced beyond last token read</remarks>
         TokenIterator  ScriptParser::ReadAssignment(const CommandLexer& lex, TokenIterator& pos)
         {
            TokenIterator retVar = pos;
            return (pos += 2, retVar);
         }

         /// <summary>Reads the conditional keyword tokens</summary>
         /// <param name="lex">The lexer</param>
         /// <param name="pos">The start position.</param>
         /// <returns>Conditional</returns>
         /// <remarks>The iterator is advanced beyond last token read</remarks>
         /// <exception cref="Logic::ArgumentException">Unrecognised conditional</exception>
         Conditional ScriptParser::ReadConditional(const CommandLexer& lex, TokenIterator& pos)
         {
            // 'if' 'not'?
            if (lex.Match(pos, TokenType::Keyword, L"if"))
               return lex.Match(pos, TokenType::Keyword, L"not") ? Conditional::IF_NOT : Conditional::IF;
               
            // 'while' 'not'?
            else if (lex.Match(pos, TokenType::Keyword, L"while"))
               return lex.Match(pos, TokenType::Keyword, L"not") ? Conditional::WHILE_NOT : Conditional::WHILE;

            // 'else' 'if' 'not'?
            else if (lex.Match(pos, TokenType::Keyword, L"else") && lex.Match(pos, TokenType::Keyword, L"if"))
               return lex.Match(pos, TokenType::Keyword, L"not") ? Conditional::ELSE_IF_NOT : Conditional::ELSE_IF;

            // 'skip' 'if' 'not'?
            else if (lex.Match(pos, TokenType::Keyword, L"skip") && lex.Match(pos, TokenType::Keyword, L"if")) 
               return lex.Match(pos, TokenType::Keyword, L"not") ? Conditional::SKIP_IF_NOT : Conditional::SKIP_IF;

            // 'do' 'if' 'not'?
            else if (lex.Match(pos, TokenType::Keyword, L"do") && lex.Match(pos, TokenType::Keyword, L"if")) 
               return lex.Match(pos, TokenType::Keyword, L"not") ? Conditional::SKIP_IF : Conditional::SKIP_IF_NOT;
               
            throw ArgumentException(HERE, L"pos", GuiString(L"Cannot read previously matched conditional: '%s'", lex.Input.c_str()));
         }
         
         /// <summary>Reads the reference object and reference operator tokens</summary>
         /// <param name="lex">The lexer</param>
         /// <param name="pos">The start position.</param>
         /// <returns>Reference Object token</returns>
         /// <remarks>The iterator is advanced beyond last token read</remarks>
         TokenIterator ScriptParser::ReadReferenceObject(const CommandLexer& lex, TokenIterator& pos)
         {
            // RefObj '->'
            TokenIterator refObj = pos;
            return (pos+=2, refObj);
         }


         /// <summary>Reads an entire NOP/comment command</summary>
         /// <param name="lex">The lexer</param>
         /// <returns>New NOP/Comment command node</returns>
         ScriptParser::CommandNodePtr ScriptParser::ReadComment(const CommandLexer& lex)
         {
            ParameterArray params;

            // Identify syntax
            CommandSyntaxRef syntax = SyntaxLib.Find(lex.count() == 0 ? CMD_NOP : CMD_COMMENT, Version);
            
            // Store comment, if any
            if (lex.count() == 2)
               params += ScriptParameter(syntax.Parameters[0], lex.Tokens[1]);

            return new CommandNode(Conditional::NONE, syntax, params, lex, LineNumber);
         }

         /// <summary>Reads an entire non-expression command</summary>
         /// <param name="lex">The lexer</param>
         /// <returns>New Non-expression command node</returns>
         /// <remarks>Grammar:
         ///    conditional = 'if'/'if not'/'while'/'while not'/'skip if'/'do if'
         ///    assignment = variable '='
         ///    
         ///    command = (assignment/conditional)? (constant/variable/null '->')? text/keyword/label
         ///
         /// </remarks>
         ScriptParser::CommandNodePtr  ScriptParser::ReadCommand(const CommandLexer& lex)
         {
            Conditional   condition = Conditional::DISCARD;
            TokenIterator pos = lex.begin(),
                          retVar = lex.end(),
                          refObj = lex.end();

            // Match: (assignment/conditional)? 
            if (MatchAssignment(lex, lex.begin()))
               retVar = ReadAssignment(lex, pos);

            else if (MatchConditional(lex, lex.begin()))
               condition = ReadConditional(lex, pos);

            // Match: (constant/variable/null '->')?
            if (MatchReferenceObject(lex, TokenIterator(pos)))
               refObj = ReadReferenceObject(lex, pos);

            // Match remaining tokens against a command
            TokenList tokens;
            CommandSyntaxRef syntax = SyntaxLib.Identify(pos, lex.end(), Version, tokens);  
            ParameterArray params;
            
            // Unrecognised: Highlight offending token / entire line
            if (syntax == CommandSyntax::Unknown)
               Errors += (pos == lex.end() ? MakeError(L"Unrecognised command", lex) : MakeError(L"Unexpected token in command", pos));
               
            else
            {
               // Allocate parameters
               //params.resize(syntax.Parameters.size());
            
               // Read/Match remaining tokens against parameter syntax
               for (const ParameterSyntax& ps : syntax.ParametersByDisplay)
               {
                  // RetVar: Use if present, otherwise condition. If neither, default to discard
                  if (ps.IsRetVar())
                     params += (lex.Valid(retVar) ? ScriptParameter(ps, *retVar) : ScriptParameter(ps, condition));
               
                  // RefObj: Ensure present
                  else if (ps.IsRefObj() && lex.Valid(refObj))
                     params += ScriptParameter(ps, *refObj);
               
                  else if (ps.IsRefObj())
                     Errors += MakeError(L"Missing reference object", lex);
               
                  // Parameter: ensure present
                  else if (!tokens.empty())
                  {
                     params += ScriptParameter(ps, tokens.front());
                     tokens.pop_front();
                  }
                  else
                  {  // Error: Missing parameter
                     Errors += MakeError(GuiString(L"Missing %s parameter", GetString(ps.Type).c_str()), lex);
                     break;
                  }
               }

               // SCRIPT-CALL: read argument/value pairs  
               if (syntax.Is(CMD_CALL_SCRIPT_VAR_ARGS))
               {
                  // Iterate thru triplets
                  for (TokenIterator arg = pos; arg < lex.end(); arg += 3)
                     // Match {name, '=', value} triplet
                     if (MatchScriptArgument(lex, TokenIterator(arg)))
                        // TODO: Match argument name
                        params.push_back(ScriptParameter(ParameterSyntax::ScriptCallArgument, arg[2]));
                     else 
                     {  // Error: Abort
                        Errors += MakeError(L"Expected script-call argument", arg);
                        break;
                     }
               }
            }

            return new CommandNode(condition, syntax, params, lex, LineNumber);
         }

         /// <summary>Reads an entire expression command</summary>
         /// <param name="lex">The lexer</param>
         /// <returns>New Expression command node</returns>
         /// <exception cref="Logic::ArgumentException">Error in expression parsing algorithm</exception>
         /// <exception cref="Logic::InvalidOperationException">Error in expression parsing algorithm</exception>
         /// <remarks>Grammar:
         ///    conditional = 'if'/'if not'/'while'/'while not'/'skip if'/'do if'
         ///    value = constant/variable/literal/null
         ///    assignment = variable '='
         ///    unary_operator = '!'/'-'/'~'
         /// 
         ///    expression = (assignment/conditional) unary_operator? value (operator value)*
         ///
         /// </remarks>
         ScriptParser::CommandNodePtr  ScriptParser::ReadExpression(const CommandLexer& lex)
         {
            Conditional    condition = Conditional::DISCARD;
            TokenIterator  pos = lex.begin();
            ParameterArray params, 
                           postfix;
            
            // Lookup syntax
            CommandSyntaxRef syntax = SyntaxLib.Find(CMD_EXPRESSION, Version);
            
            // Match: (assignment/conditional)
            if (MatchAssignment(lex, TokenIterator(pos)))
                params += ScriptParameter(syntax.Parameters[0], *ReadAssignment(lex, pos));
            else 
                params += ScriptParameter(syntax.Parameters[0], condition=ReadConditional(lex, pos));

            try
            {
               // Parse expression
               ExpressionParser expr(pos, lex.end());

               // Store ordered parameters
               for (const auto& tok : expr.InfixParams)
                  params += ScriptParameter(ParameterSyntax::ExpressionParameter, tok);

               for (const auto& tok : expr.PostfixParams)
                  postfix += ScriptParameter(ParameterSyntax::ExpressionParameter, tok);
            }
            catch (ExpressionParserException& e) {
               // Add to error collection
               Errors += (lex.Valid(e.Position) ? MakeError(e.Message, e.Position) : MakeError(e.Message, lex));

               // DEBUG: print tokens
               for (auto it = lex.begin(); it != lex.end(); ++it)
                  Console << (it==pos?L"  <*>":L"  ") << it->Text << L" " << GetString(it->Type) << ENDL;
            }

            return new CommandNode(condition, syntax, params, postfix, lex, LineNumber);
         }
         
         
         /// <summary>Parses a line into a command node, and advances the line iterator</summary>
         /// <param name="parent">Parent node</param>
         /// <param name="line">The line.</param>
         /// <returns>Single command node, or nullptr if EOF</returns>
         /// <exception cref="Logic::ArgumentException">Error in parsing algorithm</exception>
         /// <exception cref="Logic::InvalidOperationException">Error in parsing algorithm</exception>
         /// <exception cref="Logic::ExpressionParserException">Syntax error in expression</exception>
         /// <remarks>Grammar:
         /// 
         ///    conditional = 'if'/'if not'/'while'/'while not'/'skip if'/'do if'
         ///    value = constant/variable/literal/null
         ///    assignment = variable '='
         ///    comment = '*' text?
         ///    nop = ws*
         /// 
         ///    line = nop/comment/command/expression
         ///    command = (assignment/conditional)? (constant/variable/null '->')? text/keyword/label
         ///    expression = (assignment/conditional) unary_operator? value (operator value)*
         ///
         /// </remarks>
         ScriptParser::CommandNodePtr ScriptParser::ReadLine()
         {
            // EOF: Return
            if (CurrentLine == Input.end())
               return nullptr;

            // Lex current line
            CommandLexer   lex(*CurrentLine);
            CommandNodePtr node;
            
            // Comment/NOP:
            if (MatchComment(lex))
               node = ReadComment(lex);

            // Command:
            else if (MatchCommand(lex))
               node = ReadCommand(lex);

            // Expression:
            else if (MatchExpression(lex))
               node = ReadExpression(lex);
            
            else
            {
               // UNRECOGNISED: Generate empty node
               Errors += MakeError(L"Unable to parse command", lex);
               node = new CommandNode();
            }

            // Consume line + return node
            ++CurrentLine;
            return node;
         }


         
      }
   }
}
