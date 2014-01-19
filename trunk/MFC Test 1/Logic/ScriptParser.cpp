#include "stdafx.h"
#include "ScriptParser.h"
#include "ExpressionParser.h"


namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         // -------------------------------- CONSTRUCTION --------------------------------

         ScriptParser::ScriptParser(const LineArray& lines, GameVersion  v) : Input(lines), Version(v)
         {
            if (lines.size() == 0)
               throw ArgumentException(HERE, L"lines", L"Line count cannot be zero");
         }


         ScriptParser::~ScriptParser()
         {
         }

         // ------------------------------- STATIC METHODS -------------------------------

         // ------------------------------- PUBLIC METHODS -------------------------------

         /// <summary>Reads all commands in the script</summary>
         /// <returns></returns>
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

         
         ScriptParser::CommandTree ScriptParser::ParseNode(LineIterator& line)
         {
            LineIterator  text = line++;  // consume line
            CommandLexer  lex(*text);
            //ScriptCommand cmd;            // 'Unknown' syntax

            /*
            Grammar:

            conditional = 'if'/'if not'/'while'/'while not'/'skip if'/'do if'
            value = constant/variable/literal/null
            assignment = variable '='
            comment = '*' text?
            nop = ws*

            line = nop/comment/command/expression
            command = (assignment/conditional)? (constant/variable/null '->')? text/keyword/label
            expression = (assignment/conditional) unary_operator? value (operator value)*
            */

            // DEBUG:
            Console << GetLineNumber(text) << L": " << *text << ENDL;
            auto num = GetLineNumber(text);

            // Comment/NOP:
            if (MatchComment(lex))
               //cmd = ReadComment(lex, text);
               return CommandTree( new CommandNode(ReadComment(lex, text), GetLineNumber(text), ErrorArray()) );

            // Command:
            else if (MatchCommand(lex))
               //cmd = ReadCommand(lex, text);
               return ReadCommand(lex, text);

            // Expression:
            else if (MatchExpression(lex))
               //cmd = ReadExpression(lex, text);
               return CommandTree( new CommandNode(ReadExpression(lex, text), GetLineNumber(text), ErrorArray()) );
            
            // * could potentially validate parameters at this point

            // DEBUG:
            Console << Colour::Yellow << L"FAILED" << ENDL;
            for (auto tok : lex.Tokens)
               Console << Colour::Yellow << (UINT)tok.Type << L" : " << tok.Text << ENDL;

            // Generate node, advance line
            return CommandTree( new CommandNode(ScriptCommand(), GetLineNumber(text), ErrorArray()) );

         }


         
         bool  ScriptParser::MatchAssignment(const CommandLexer& lex, TokenIterator& pos) const
         {
            TokenIterator start = pos;

            // Assignment: variable '='
            return lex.Match(pos, TokenType::Variable) && lex.Match(pos, TokenType::Operator, L"=") ? true : (pos=start, false);
         }

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
         
         bool ScriptParser::MatchReferenceObject(const CommandLexer& lex, TokenIterator& pos) const
         {
            TokenIterator start = pos;

            // (constant/variable/null '->')?
            return (lex.Match(pos, TokenType::ScriptObject) || lex.Match(pos, TokenType::Variable) || lex.Match(pos, TokenType::Null))
                 && lex.Match(pos, TokenType::Operator, L"->") ? true : (pos=start, false);
         }

         
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














         

         TokenIterator  ScriptParser::ReadAssignment(const CommandLexer& lex, TokenIterator& pos)
         {
            TokenIterator retVar = pos;
            return (pos += 2, retVar);
         }

         
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
         
         TokenIterator ScriptParser::ReadReferenceObject(const CommandLexer& lex, TokenIterator& pos)
         {
            // RefObj '->'
            TokenIterator refObj = pos;
            return (pos+=2, refObj);
         }

         
         ScriptCommand  ScriptParser::ReadComment(const CommandLexer& lex, const LineIterator& line)
         {
            UINT id = (lex.count() == 0 ? CMD_NOP : CMD_COMMENT);
            
            // DEBUG:
            Console << Colour::Green << (id == CMD_NOP ? L"nop" : L"comment") << ENDL;

            // Return NOP/Comment
            return ScriptCommand(*line, SyntaxLib.Find(id, Version), lex.Tokens);
         }


         ScriptParser::CommandTree  ScriptParser::ReadCommand(const CommandLexer& lex, const LineIterator& line)
         {
            /*
            conditional = 'if'/'if not'/'while'/'while not'/'skip if'/'do if'
            assignment = variable '='
            
            command = (assignment/conditional)? (constant/variable/null '->')? text/keyword/label
            */
            ErrorArray    errors;
            Conditional   condition = Conditional::NONE;
            TokenIterator refObj = lex.end(), 
                          retVar = lex.end(),
                          pos = lex.begin();

            // (assignment/conditional)? 
            if (MatchAssignment(lex, lex.begin()))
                retVar = ReadAssignment(lex, pos);

            // (assignment/conditional)? 
            else if (MatchConditional(lex, lex.begin()))
                condition = ReadConditional(lex, pos);

            // (constant/variable/null '->')?
            TokenIterator dummy = pos;
            if (MatchReferenceObject(lex, dummy))
               refObj = ReadReferenceObject(lex, pos);

            // Lookup command using remaining tokens
            CommandSyntax syntax = SyntaxLib.Identify(pos, lex.end(), Version);
            
            // TODO: Arrange parameters
            // TokenArray params;
            ScriptCommand cmd(*line, syntax, TokenArray());

            // DEBUG:
            if (syntax != SyntaxLib.Unknown)
               Console << Colour::Green << L"MATCH: " << syntax.Text << ENDL;
            else
            {
               Console << Colour::Red << L"UNRECOGNISED:" << ENDL;

               if (pos != lex.end())
                  errors.push_back(ErrorToken(GetLineNumber(line), pos->Start, pos->End));
            }

            // Create node
            return CommandTree( new CommandNode(cmd, GetLineNumber(line), errors) );
         }

         ScriptCommand  ScriptParser::ReadExpression(const CommandLexer& lex, const LineIterator& line)
         {
            /*
            conditional = 'if'/'if not'/'while'/'while not'/'skip if'/'do if'
            value = constant/variable/literal/null
            assignment = variable '='
            unary_operator = '!'/'-'/'~'

            expression = (assignment/conditional) unary_operator? value (operator value)*
            */
            Conditional   condition = Conditional::NONE;
            TokenIterator retVar = lex.end(),
                          pos = lex.begin();

            // (assignment/conditional)
            if (MatchAssignment(lex, lex.begin()))
                retVar = ReadAssignment(lex, pos);
            else 
                condition = ReadConditional(lex, pos);

            // (unary_operator? value (operator value)*)
            ExpressionParser exp(pos, lex.end());
            exp.Parse();  // nb: may throw 
            
            // TODO: Arrange parameters?
            //TokenArray params(hash.Parameters);

            // DEBUG:
            Console << Colour::Green << L"expression" << ENDL;

            // Create expression
            return ScriptCommand(*line, SyntaxLib.Find(CMD_EXPRESSION, Version), exp.InfixParams);
         }

         
      }
   }
}
