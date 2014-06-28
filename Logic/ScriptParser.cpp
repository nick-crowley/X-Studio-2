#include "stdafx.h"
#include "ScriptParser.h"
#include "ExpressionParser.h"
#include "GameObjectLibrary.h"
#include "ScriptObjectLibrary.h"
#include "SyntaxLibrary.h"
#include "CommandHash.h"
#include "ScriptFile.h"

/// <summary>Prints the parse tree post-verification and post-compilation</summary>
//#define DEBUG_PRINT

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         // -------------------------------- CONSTRUCTION --------------------------------

         /// <summary>Creates a parser for an entire script</summary>
         /// <param name="file">Script</param>
         /// <param name="lines">The lines to parse</param>
         /// <param name="v">The game version</param>
         /// <exception cref="Logic::ArgumentException">Line array is empty</exception>
         /// <exception cref="Logic::AlgorithmException">Error in parsing algorithm</exception>
         ScriptParser::ScriptParser(ScriptFile& file, const LineArray& lines, GameVersion  v) 
            : Input(lines), Version(v), Root(new CommandNode()), Script(file)
         {
            if (lines.size() == 0)
               throw ArgumentException(HERE, L"lines", L"Line count cannot be zero");

            // Clear script
            Script.Clear();

            // Parse input
            CurrentLine = Input.begin();
            ParseRoot();
         }

         ScriptParser::~ScriptParser()
         {
         }

         // ------------------------------- STATIC METHODS -------------------------------
         
         /// <summary>Generates a single command tree node from a line of text.</summary>
         /// <param name="line">line text.</param>
         /// <param name="v">version.</param>
         /// <returns>Command node</returns>
         /// <exception cref="Logic::AlgorithmException">Error in parsing algorithm</exception>
         CommandNodePtr  ScriptParser::Generate(const wstring& line, GameVersion v)
         {
            ScriptParser parser(ScriptFile(L""), {line}, v);

            // Sanity check
            if (parser.Empty)
               throw AlgorithmException(HERE, L"Parse tree has no children");

            // Retrieve node
            return parser.Root->Children.front();
         }

         /// <summary>Identifies the command from line of text.</summary>
         /// <param name="script">script.</param>
         /// <param name="line">line text.</param>
         /// <returns>Command syntax if successful, otherwise 'Unknown' sentinel</returns>
         CommandSyntaxRef  ScriptParser::Identify(ScriptFile script, const wstring& line)
         {
            try
            {
               ScriptParser parser(script, {line}, script.Game);

               // Sanity check
               if (parser.Empty)
                  throw AlgorithmException(HERE, L"Parse tree has no children");

               // Retrieve syntax of single node
               return parser.Root->Children.front()->Syntax;
            }
            catch (ExceptionBase& e) {
               Console.Log(HERE, e);
               return CommandSyntax::Unrecognised;
            }
         }
         
         /// <summary>Parses a single command from a line of text.</summary>
         /// <param name="script">script.</param>
         /// <param name="line">line text.</param>
         /// <returns>Command if successful, otherwise 'unrecognised' sentinel</returns>
         ScriptCommand  ScriptParser::Parse(ScriptFile script, const wstring& line)
         {
            try
            {
               ScriptParser parser(script, {line}, script.Game);

               // Sanity check
               if (parser.Empty)
                  throw AlgorithmException(HERE, L"Parse tree has no children");

               // Retrieve node
               auto node = parser.Root->Children.front();

               // Generate command
               if (!node->Is(CMD_EXPRESSION))
                  return ScriptCommand(node->LineText, node->Syntax, node->Parameters, node->CmdComment);
               else
                  return ScriptCommand(node->LineText, node->Syntax, node->Parameters, node->Postfix, node->CmdComment);
            }
            catch (ExceptionBase& e) {
               Console.Log(HERE, e);
               return ScriptCommand::Unrecognised;
            }
         }

         
         /// <summary>Print error token to console</summary>
         ConsoleWnd&  operator<<(ConsoleWnd& c, const ErrorToken& e)
         {
            return c << Cons::Error << "ERROR: " << Cons::White << e.Line << ": " << e.Message;
         }

         /// <summary>Print error array to console</summary>
         ConsoleWnd&  operator<<(ConsoleWnd& c, const ErrorArray& errs)
         {
            for (auto& e : errs)
               Console << e << ENDL;
            return c;
         }
         
         // ------------------------------- PUBLIC METHODS -------------------------------

         /// <summary>Reads all commands in the script</summary>
         /// <exception cref="Logic::InvalidOperationException">Script contains errors</exception>
         void  ScriptParser::Compile()
         {
            // Ensure error free
            if (!Errors.empty())
               throw InvalidOperationException(HERE, L"Cannot compile a script with errors");

            // Compile tree
            Tree.Compile(Script, Errors);

#ifdef DEBUG_PRINT
            Print();
#endif
         }

         /// <summary>Find all instances of a symbol</summary>
         /// <param name="name">symbol name</param>
         /// <param name="type">symbol type</param>
         /// <param name="results">On return, this contains the results</param>
         void  ScriptParser::FindAll(const wstring& name, SymbolType type, SymbolList& results) const
         {
            Tree.FindAll(name, type, results);
         }
         
         /// <summary>Query whether parse tree is empty</summary>
         /// <returns></returns>
         bool ScriptParser::IsEmpty() const
         {
            return Tree.Empty;
         }

         /// <summary>Get whether there are no errors</summary>
         bool ScriptParser::IsSuccessful() const
         {
            return Errors.empty();
         }

         /// <summary>Prints current state of the parse tree to the console</summary>
         void  ScriptParser::Print()
         {
            Root->Print(0);
         }
         
         /// <summary>Flattens the parse tree into a list of command nodes.</summary>
         /// <returns>List of command nodes.</returns>
         CommandNodeList  ScriptParser::ToList() const
         {
            CommandNodeList l;

            // Flatten all commands, excluding the root
            Root->ToList(l);
            return l;
         }

         // ------------------------------ PROTECTED METHODS -----------------------------

         // ------------------------------- PRIVATE METHODS ------------------------------
         
         /// <summary>Reads the next node but returns the current node</summary>
         /// <returns>Node that was current before call</returns>
         CommandNodePtr  ScriptParser::Advance()
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
         
         /// <summary>Generate an error for the entire line</summary>
         /// <param name="msg">The message</param>
         /// <param name="lex">The lexer</param>
         /// <returns></returns>
         ErrorToken  ScriptParser::MakeError(const wstring& msg, const CommandLexer& lex) const
         {
            return ErrorToken(msg, LineNumber, lex.Input.substr(lex.Extent.cpMin, lex.Extent.cpMax-lex.Extent.cpMin), lex.Extent);
         }

         /// <summary>Generate an error for a single token</summary>
         /// <param name="msg">The message</param>
         /// <param name="tok">The token</param>
         /// <returns></returns>
         ErrorToken  ScriptParser::MakeError(const wstring& msg, const TokenIterator& tok) const
         {
            return ErrorToken(msg, LineNumber, *tok);
         }
         
         /// <summary>Reads all commands in the script</summary>
         /// <returns></returns>
         /// <exception cref="Logic::AlgorithmException">Error in parsing algorithm</exception>
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
               case BranchLogic::ElseIf:  
               case BranchLogic::Else:    
                  ParseIfElse(Root->Add(Advance()));
                  break;
               
               // SkipIf: Add
               case BranchLogic::SkipIf:  
                  ParseSkipIf(Root->Add(Advance()));
                  break;

               // Command/NOP/Break/Continue/End: Add 
               default:
                  Root->Add(Advance());
                  break;
               }
            }
            
            // Verify tree
            Root->Verify(Script, Errors);

#ifdef DEBUG_PRINT
            Print();
#endif
         }

         /// <summary>Reads current 'if/else/else-if' command and all descendants including 'end'</summary>
         /// <returns></returns>
         /// <exception cref="Logic::AlgorithmException">Error in parsing algorithm</exception>
         void ScriptParser::ParseIfElse(CommandNodePtr& If)
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
                  ParseIfElse(If->Add(Advance()));  
                  break;

               // SkipIf: Add
               case BranchLogic::SkipIf:  
                  ParseSkipIf(If->Add(Advance()));  
                  break;

               // Else/Else-if: Add-to-Parent, Stop
               case BranchLogic::ElseIf:  
               case BranchLogic::Else:  
                  ParseIfElse(If->Parent->Add(Advance()));
                  return;

               // End: Add-to-Parent, Stop
               case BranchLogic::End:
                  If->Parent->Add(Advance());
                  return;
               }
            }
         }

         /// <summary>Reads current 'skip-if' command and all descendants</summary>
         /// <returns></returns>
         /// <exception cref="Logic::AlgorithmException">Error in parsing algorithm</exception>
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

               // Command/Break/Continue: Add, Stop
               default: 
                  SkipIf->Add(Advance());
                  return;

               // If: Add (Abort)
               case BranchLogic::If:      // Invalid
               case BranchLogic::While:   // Invalid
               case BranchLogic::ElseIf:  // Invalid 
               case BranchLogic::Else:    // Invalid
                  ParseIfElse(SkipIf->Add(Advance()));  
                  return;

               // SkipIf: Add (Abort)
               case BranchLogic::SkipIf:  // Invalid
                  ParseSkipIf(SkipIf->Add(Advance()));  
                  return;

               // End: Add (Abort)
               case BranchLogic::End:     // Invalid
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
            return lex.Match(pos, TokenType::Variable) && lex.Match(pos, TokenType::UnaryOp, L"[") ? true : (pos=start, false);
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
            return lex.Match(pos, TokenType::Variable) && lex.Match(pos, TokenType::BinaryOp, L"=") ? true : (pos=start, false);
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

            // Start
            if (lex.Match(pos, TokenType::Keyword, L"start"))
               return true;

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
                 && lex.Match(pos, TokenType::BinaryOp, L"->") ? true : (pos=start, false);
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
            return (lex.Match(pos, TokenType::Text) 
                 || lex.Match(pos, TokenType::Null)      // Argument names may be reserved words
                 || lex.Match(pos, TokenType::Keyword))  // Argument names may be reserved words

                 && lex.Match(pos, TokenType::BinaryOp, L"=") 

                 && (lex.Match(pos, TokenType::ScriptObject) 
                  || lex.Match(pos, TokenType::GameObject) 
                  || lex.Match(pos, TokenType::Variable)
                  || lex.Match(pos, TokenType::Number)
                  || lex.Match(pos, TokenType::String)
                  || lex.Match(pos, TokenType::Null))

                 ? true : (pos=start, false);
         }

         /// <summary>Matches a varg value</summary>
         /// <param name="lex">The lexer</param>
         /// <param name="pos">The start position.</param>
         /// <returns></returns>
         /// <remarks>The iterator is advanced beyond last matched token if successful, otherwise it is not moved</remarks>
         bool ScriptParser::MatchVArgument(const CommandLexer& lex, TokenIterator& pos) const
         {
            TokenIterator start = pos;

            // constant/variable/null/string/number
            return (lex.Match(pos, TokenType::ScriptObject) 
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
            if (lex.Match(pos, TokenType::BinaryOp, L"*"))
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
            if (lex.Match(pos, TokenType::UnaryOp, L"!") 
             || lex.Match(pos, TokenType::BinaryOp, L"-")    // Unary minus is identified as binary subtract by lexer, and converted by parser
             || lex.Match(pos, TokenType::UnaryOp, L"~")
             || lex.Match(pos, TokenType::UnaryOp, L"("))   
               return true;   // Simplify matching of complex expressions eg. ((, (-4 etc.  [Commands never start with unary operators anyway]

            // Value        
            if (lex.Match(pos, TokenType::Number) 
             || lex.Match(pos, TokenType::String) 
             || lex.Match(pos, TokenType::Variable)
             || lex.Match(pos, TokenType::Null)
             || lex.Match(pos, TokenType::GameObject) 
             || lex.Match(pos, TokenType::ScriptObject))
               return true;   // Matching TRUE here without checking for further operators. Expression parser will provide detailed errors (if any)
                              // Won't be confused with Command because that's already matched false

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
         /// <exception cref="Logic::AlgorithmException">Unrecognised conditional</exception>
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

            // 'start'
            else if (lex.Match(pos, TokenType::Keyword, L"start"))
               return Conditional::START;
               
            throw AlgorithmException(HERE, VString(L"Cannot read previously matched conditional: '%s'", lex.Input.c_str()));
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
         /// <returns>New NOP/Comment/CmdComment command node</returns>
         CommandNodePtr ScriptParser::ReadComment(const CommandLexer& lex)
         {
            ParameterArray params;
            CommandNodePtr node;

            // NOP: No processing required
            if (lex.count() == 0)
               return new CommandNode(Conditional::NONE, SyntaxLib.Find(CMD_NOP, Version), params, lex, LineNumber, false);

            // Re-lex line without the '*' operator
            CommentLexer lex2(lex.Input);

            // Commented Command:
            if (MatchCommand(lex2) && (node = ReadCommand(lex2, true)))
               return node;

            // Commented Expression:
            else if (MatchExpression(lex2) && (node = ReadExpression(lex2, true)))
               return node;

            // Comment: Assemble manually
            CommandSyntaxRef syntax = SyntaxLib.Find(CMD_COMMENT, Version);

            // Extract text (Insert placeholder if none)
            params += ScriptParameter(syntax.Parameters[0], lex.count()==2 ? lex.Tokens[1] : ScriptToken(TokenType::Comment, 1,1, L""));

            // Return comment
            return new CommandNode(Conditional::NONE, syntax, params, lex, LineNumber, false);
         }

         /// <summary>Reads an entire non-expression command</summary>
         /// <param name="lex">The lexer</param>
         /// <param name="comment">whether attempting to read as a command comment</param>
         /// <returns>New Non-expression command node</returns>
         /// <remarks>Grammar:
         ///    conditional = 'if'/'if not'/'while'/'while not'/'skip if'/'do if'
         ///    assignment = variable '='
         ///    
         ///    command = (assignment/conditional)? (constant/variable/null '->')? text/keyword/label
         ///
         /// </remarks>
         /// <exception cref="Logic::AlgorithmException">Error in expression parsing algorithm</exception>
         CommandNodePtr  ScriptParser::ReadCommand(const CommandLexer& lex, bool comment)
         {
            ErrorArray&   errQueue = (comment ? CommentErrors : Errors);  // Set appropriate error queue
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
            if (syntax == CommandSyntax::Unrecognised)
               errQueue += (pos == lex.end() ? MakeError(L"Unrecognised command", lex) : MakeError(L"Unexpected token in command", pos));
            else
            {
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
                     errQueue += MakeError(L"Missing reference object", lex);
               
                  // Parameter: ensure present
                  else if (!tokens.empty())
                  {
                     params += ScriptParameter(ps, tokens.front());
                     tokens.pop_front();
                  }
                  else
                  {  // Error: Missing parameter
                     errQueue += MakeError(VString(L"Missing %s parameter", GetString(ps.Type).c_str()), lex);
                     break;
                  }
               }

               // VARG 
               if (syntax.IsVArgument())
               {
                  switch (syntax.VArgument)
                  {
                  // EXTENSION: 
                  case VArgSyntax::CommaDelimited:
                     // Match {[val] [, val] [, val] ...} 
                     for (TokenIterator arg = pos; arg < lex.end(); ++arg)
                     {
                        // Match delimiter  (Except first argument)
                        if (arg != pos && !lex.Match(arg, TokenType::UnaryOp, L",")) 
                        {  // Error: Abort
                           errQueue += MakeError(L"Expected comma operator", arg);
                           break;
                        }
                        // Match value
                        else if (MatchVArgument(lex, TokenIterator(arg))) 
                           params.push_back(ScriptParameter(ParameterSyntax::VArgParameter, *arg));
                        else
                        {  // Error: Abort
                           errQueue += MakeError(L"Expected varg parameter", arg);
                           break;
                        }  
                     }
                     break;
                  
                  // SCRIPT-CALL: 
                  case VArgSyntax::NamedPairs:
                     // Match {name, '=', value} triplets
                     for (TokenIterator arg = pos; arg < lex.end(); arg += 3)
                        if (MatchScriptArgument(lex, TokenIterator(arg)))
                           params.push_back(ScriptParameter(arg[0], arg[2]));
                        else 
                        {  // Error: Abort
                           errQueue += MakeError(L"Expected script-call argument", arg);
                           break;
                        }
                     break;
                  }
               }
            }

            // Comment: Return nothing if command cannot be properly parsed
            if (comment && !errQueue.empty())
            {
               errQueue.clear();
               return nullptr;
            }

            // Return new command / commented-command
            return new CommandNode(condition, syntax, params, lex, LineNumber, comment);
         }

         /// <summary>Reads an entire expression command</summary>
         /// <param name="lex">The lexer</param>
         /// <param name="comment">whether attempting to read as a command comment</param>
         /// <returns>New Expression command node</returns>
         /// <remarks>Grammar:
         ///    conditional = 'if'/'if not'/'while'/'while not'/'skip if'/'do if'
         ///    value = constant/variable/literal/null
         ///    assignment = variable '='
         ///    unary_operator = '!'/'-'/'~'
         /// 
         ///    expression = (assignment/conditional) unary_operator? value (operator value)*
         ///
         /// </remarks>
         /// <exception cref="Logic::AlgorithmException">Error in expression parsing algorithm</exception>
         CommandNodePtr  ScriptParser::ReadExpression(const CommandLexer& lex, bool comment)
         {
            ErrorArray&    errQueue = (comment ? CommentErrors : Errors);  // Set appropriate error queue
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

               // Store infix 
               for (const auto& tok : expr.InfixParams)
                  params += ScriptParameter(ParameterSyntax::ExpressionParameter, tok);

               // Store postfix
               for (const auto& tok : expr.PostfixParams)
                  postfix += ScriptParameter(ParameterSyntax::ExpressionParameter, tok);
            }
            catch (ExpressionParserException& e) {
               // Add to error collection
               errQueue += (lex.Valid(e.Position) ? MakeError(e.Message, e.Position) : MakeError(e.Message, lex));

               // DEBUG: print tokens
               if (!comment)
                  for (auto it = lex.begin(); it != lex.end(); ++it)
                     Console << (it==pos?L"  <*>":L"  ") << it->Text << L" " << GetString(it->Type) << ENDL;
            }

            // Comment: Return nothing if command cannot be properly parsed
            if (comment && !errQueue.empty())
            {
               errQueue.clear();
               return nullptr;
            }

            // Return new expression / commented-expression
            return new CommandNode(condition, syntax, params, postfix, lex, LineNumber, comment);
         }
         
         
         /// <summary>Parses a line into a command node, and advances the line iterator</summary>
         /// <param name="parent">Parent node</param>
         /// <param name="line">The line.</param>
         /// <returns>Single command node, or nullptr if EOF</returns>
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
         /// <exception cref="Logic::AlgorithmException">Error in parsing algorithm</exception>
         CommandNodePtr ScriptParser::ReadLine()
         {
            // EOF: Return
            if (CurrentLine == Input.end())
               return nullptr;

            // Lex current line
            CommandLexer   lex(*CurrentLine);
            CommandNodePtr node;
            
            // NOP/Comment/CmdComment:
            if (MatchComment(lex))
               node = ReadComment(lex);

            // Command:
            else if (MatchCommand(lex))
               node = ReadCommand(lex, false);

            // Expression:
            else if (MatchExpression(lex))
               node = ReadExpression(lex, false);
            
            else
            {
               // UNRECOGNISED: Generate empty node
               Errors += MakeError(L"Unable to parse command", lex);
               node = new CommandNode(Conditional::NONE, CommandSyntax::Unrecognised, ParameterArray(), lex, LineNumber, false);
            }

            // Consume line + return node
            ++CurrentLine;
            return node;
         }


         
      }
   }
}
