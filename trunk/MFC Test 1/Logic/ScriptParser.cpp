#include "stdafx.h"
#include "ScriptParser.h"
#include "ExpressionParser.h"

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
         ScriptParser::ScriptParser(const LineArray& lines, GameVersion  v) : Input(lines), Version(v)
         {
            if (lines.size() == 0)
               throw ArgumentException(HERE, L"lines", L"Line count cannot be zero");
         }


         ScriptParser::~ScriptParser()
         {
         }

         // ------------------------------- STATIC METHODS -------------------------------
         
         ScriptParser::ErrorToken  ScriptParser::MakeError(const CommandLexer& lex, const LineIterator& line) const
         {
            return ErrorToken(GetLineNumber(line), 0, line->length()-1);
         }

         ScriptParser::ErrorToken  ScriptParser::MakeError(const CommandLexer& lex, const LineIterator& line, const TokenIterator& tok) const
         {
            return lex.Valid(tok) ? ErrorToken(GetLineNumber(line), tok->Start, tok->End) : MakeError(lex, line);
         }

         // ------------------------------- PUBLIC METHODS -------------------------------

         /// <summary>Reads all commands in the script</summary>
         /// <returns>Tree of command nodes</returns>
         /// <exception cref="Logic::ArgumentException">Error in parsing algorithm</exception>
         /// <exception cref="Logic::InvalidOperationException">Error in parsing algorithm</exception>
         /// <exception cref="Logic::ScriptSyntaxException">Syntax error in expression</exception>
         ScriptParser::ScriptTree  ScriptParser::ParseScript()
         {
            ScriptTree  script;
            CommandTree node;
            
            // Iterate over lines  (count >= 1)
            for (LineIterator line = Input.begin(); line < Input.end(); )
            {
               // Read command, add to script
               script.Add(node = ParseNode(line));
               
               // Examine command
               switch (node->Logic)
               {
               // Conditional: Read child commands into node
               case BranchLogic::If:      
               case BranchLogic::While:  
               case BranchLogic::ElseIf:  
               case BranchLogic::Else:    
               case BranchLogic::SkipIf:  
                  ParseBranch(node, line);
                  break;
               }
            }

            return script;
         }

         // ------------------------------ PROTECTED METHODS -----------------------------

         // ------------------------------- PRIVATE METHODS ------------------------------
         
         /// <summary>Get the one-based line number of a line</summary>
         /// <param name="line">The line</param>
         /// <returns>One based line number</returns>
         UINT  ScriptParser::GetLineNumber(const LineIterator& line) const
         {
            return line - Input.begin() + 1;
         }


         /// <summary>Parses the descendant commands of a branching command</summary>
         /// <param name="branch">Branching command</param>
         /// <param name="line">The line containing first descendant</param>
         /// <exception cref="Logic::ArgumentException">Error in parsing algorithm</exception>
         /// <exception cref="Logic::InvalidOperationException">Error in parsing algorithm</exception>
         /// <exception cref="Logic::ScriptSyntaxException">Syntax error in expression</exception>
         void ScriptParser::ParseBranch(CommandTree& branch, LineIterator& line)
         {
            CommandTree node;

            // Read children
            while (line < Input.end())
            {
               // Read command, add to branch
               branch->Add(node = ParseNode(line));

               // Examine command
               switch (node->Logic)
               {
               // Conditional: Read child commands into node
               case BranchLogic::If:      
               case BranchLogic::While:  
               case BranchLogic::ElseIf:  
               case BranchLogic::Else:    
               case BranchLogic::SkipIf:  
                  ParseBranch(node, line);
                  break;

               // Comment: Always valid, read any/all
               case BranchLogic::NOP: 
                  break;

               // End: Abort if branch == if/while
               case BranchLogic::End:     
                  if (branch->Logic == BranchLogic::If || branch->Logic == BranchLogic::While)
                     return;
                  break;

               // Command: Abort if branch == SkipIf
               default: 
                  if (branch->Logic == BranchLogic::SkipIf)
                     return;
                  break;
               }
            }
         }

         
         /// <summary>Parses a line into a command node, and advances the line iterator</summary>
         /// <param name="line">The line.</param>
         /// <returns>Single command node</returns>
         /// <exception cref="Logic::ArgumentException">Error in parsing algorithm</exception>
         /// <exception cref="Logic::InvalidOperationException">Error in parsing algorithm</exception>
         /// <exception cref="Logic::ScriptSyntaxException">Syntax error in expression</exception>
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
         ///    expression = (assignment/conditional) unary_operator? value (operator value)*</remarks>
         ScriptParser::CommandTree ScriptParser::ParseNode(LineIterator& line)
         {
            LineIterator  text = line++;  // consume line
            CommandLexer  lex(*text);
            CommandNode*  node = nullptr;
            
            // DEBUG:
            #ifdef PRINT_CONSOLE
               Console << GetLineNumber(text) << L": " << *text << ENDL;
               auto num = GetLineNumber(text);
            #endif

            // Comment/NOP:
            if (MatchComment(lex))
               node = ReadComment(lex, text);

            // Command:
            else if (MatchCommand(lex))
               node = ReadCommand(lex, text);

            // Expression:
            else if (MatchExpression(lex))
               node = ReadExpression(lex, text);
            
            else
            {  // UNRECOGNISED: Generate empty node
               node = new CommandNode(ScriptCommand::Unknown, GetLineNumber(text), MakeError(lex, line));
            
               // DEBUG:
               #ifdef PRINT_CONSOLE
                  Console << Colour::Yellow << L"FAILED" << ENDL;
                  for (auto tok : lex.Tokens)
                     Console << Colour::Yellow << (UINT)tok.Type << L" : " << tok.Text << ENDL;
               #endif
            }

            // Wrap node
            return CommandTree(node);
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

            // text/keyword/label
            return lex.Match(pos, TokenType::Text) || lex.Match(pos, TokenType::Keyword) || lex.Match(pos, TokenType::Label);
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
         /// <exception cref="Logic::ScriptSyntaxException">Unrecognised conditional</exception>
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
               
            throw ScriptSyntaxException(HERE, L"Invalid conditional - use sentinel syntax");
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
         /// <param name="line">The line</param>
         /// <returns>NOP/Comment command</returns>
         ScriptParser::CommandNode*  ScriptParser::ReadComment(const CommandLexer& lex, const LineIterator& line)
         {
            UINT id = (lex.count() == 0 ? CMD_NOP : CMD_COMMENT);
            
            // DEBUG:
            #ifdef PRINT_CONSOLE
               Console << Colour::Green << (id == CMD_NOP ? L"nop" : L"comment") << ENDL;
            #endif

            // Return NOP/Comment
            return new CommandNode(ScriptCommand(*line, SyntaxLib.Find(id, Version), lex.Tokens), GetLineNumber(line));
         }


         /// <summary>Reads an entire non-expression command</summary>
         /// <param name="lex">The lexer</param>
         /// <param name="line">The line</param>
         /// <returns>Non-expression command</returns>
         /// <remarks>Grammar:
         ///    conditional = 'if'/'if not'/'while'/'while not'/'skip if'/'do if'
         ///    assignment = variable '='
         ///    
         ///    command = (assignment/conditional)? (constant/variable/null '->')? text/keyword/label</remarks>
         ScriptParser::CommandNode*  ScriptParser::ReadCommand(const CommandLexer& lex, const LineIterator& line)
         {
            ErrorArray    errors;
            Conditional   condition = Conditional::NONE;
            TokenIterator refObj = lex.end(), 
                          retVar = lex.end(),
                          pos = lex.begin();

            // Match: (assignment/conditional)? 
            if (MatchAssignment(lex, lex.begin()))
                retVar = ReadAssignment(lex, pos);

            // Match: (assignment/conditional)? 
            else if (MatchConditional(lex, lex.begin()))
                condition = ReadConditional(lex, pos);

            // Match: (constant/variable/null '->')?
            if (MatchReferenceObject(lex, TokenIterator(pos)))
               refObj = ReadReferenceObject(lex, pos);

            // Lookup command using remaining tokens
            CommandSyntax syntax = SyntaxLib.Identify(pos, lex.end(), Version);
            
            // UNKNOWN:
            if (syntax == CommandSyntax::Unknown)
               errors.push_back(MakeError(lex, line, pos));

            // TODO: Arrange parameters
            // TokenArray params;
            ScriptCommand cmd(*line, syntax, TokenArray());

            // DEBUG:
            #ifdef PRINT_CONSOLE
               if (syntax != CommandSyntax::Unknown)
                  Console << Colour::Green << L"MATCH: " << syntax.Text << ENDL;
               else
                  Console << Colour::Red << L"UNRECOGNISED:" << ENDL;
            #endif

            // Create node
            return new CommandNode(cmd, GetLineNumber(line), errors);
         }

         /// <summary>Reads an entire expression command</summary>
         /// <param name="lex">The lexer</param>
         /// <param name="line">The line</param>
         /// <returns>Expression command</returns>
         /// <exception cref="Logic::ArgumentException">Error in parsing algorithm</exception>
         /// <exception cref="Logic::InvalidOperationException">Error in parsing algorithm</exception>
         /// <exception cref="Logic::ScriptSyntaxException">Syntax error in expression</exception>
         /// <remarks>Grammar:
         ///    conditional = 'if'/'if not'/'while'/'while not'/'skip if'/'do if'
         ///    value = constant/variable/literal/null
         ///    assignment = variable '='
         ///    unary_operator = '!'/'-'/'~'
         /// 
         ///    expression = (assignment/conditional) unary_operator? value (operator value)*</remarks>
         ScriptParser::CommandNode*  ScriptParser::ReadExpression(const CommandLexer& lex, const LineIterator& line)
         {
            Conditional   condition = Conditional::NONE;
            TokenIterator retVar = lex.end(),
                          pos = lex.begin();
            CommandSyntax syntax = SyntaxLib.Find(CMD_EXPRESSION, Version);

            // Match: (assignment/conditional)
            if (MatchAssignment(lex, lex.begin()))
                retVar = ReadAssignment(lex, pos);
            else 
                condition = ReadConditional(lex, pos);

            // DEBUG:
            #ifdef PRINT_CONSOLE
               Console << Colour::Green << L"expression" << ENDL;
            #endif

            try
            {
               // Match: (unary_operator? value (operator value)*)
               ExpressionParser expr(pos, lex.end());
               expr.Parse();  
               
               // TODO: Arrange parameters?
               //TokenArray params(hash.Parameters);

               // Create expression
               return new CommandNode(ScriptCommand(*line, syntax, expr.InfixParams), GetLineNumber(line));
            }
            catch (ScriptSyntaxException& )
            {
               // Expression syntax error
               return new CommandNode(ScriptCommand(*line, syntax, lex.Tokens), GetLineNumber(line), MakeError(lex, line));
            }
         }

         
      }
   }
}
