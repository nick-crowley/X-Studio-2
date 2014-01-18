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
               script.Add(node = ReadLine(line));
               
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
               branch->Add(node = ReadLine(line));

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


         
         bool  ScriptParser::MatchAssignment(const CommandLexer& lex, TokenIterator& pos) const
         {
            // Assignment: variable '='
            return lex.Match(pos, TokenType::Variable) && lex.Match(pos, TokenType::Operator, L"=");
         }

         bool  ScriptParser::MatchConditional(const CommandLexer& lex, TokenIterator& pos) const
         {
            // If/while
            if (lex.Match(pos, TokenType::Keyword, L"if") || lex.Match(pos, TokenType::Keyword, L"while"))
               return true;

            // SkipIf/DoIf
            if (lex.Match(pos, TokenType::Keyword, L"skip") || lex.Match(pos, TokenType::Keyword, L"do"))
               return lex.Match(pos, TokenType::Keyword, L"if");

            // ElseIf
            return lex.Match(pos, TokenType::Keyword, L"else") && lex.Match(pos, TokenType::Keyword, L"if");
         }
         
         bool ScriptParser::MatchReferenceObject(const CommandLexer& lex, TokenIterator& pos) const
         {
            // (constant/variable/null '->')?
            return (lex.Match(pos, TokenType::ScriptObject) || lex.Match(pos, TokenType::Variable))
                 && lex.Match(pos, TokenType::Operator, L"->");
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
            TokenIterator pos = lex.begin(),
                          dummy = lex.begin();
            
            // (assignment/conditional)?
            if (MatchAssignment(lex, dummy) || MatchConditional(lex, dummy))
               pos = dummy;

            // (constant/variable/null '->')?
            if (MatchReferenceObject(lex, dummy))
               pos = dummy;

            // text
            return lex.Match(pos, TokenType::Text);
         }


         bool ScriptParser::MatchExpression(const CommandLexer& lex) const
         {
            TokenIterator pos = lex.begin(),
                          dummy = lex.begin();

            // Unary_operator?
            if (lex.Match(dummy, TokenType::Operator, L"!") 
             || lex.Match(dummy, TokenType::Operator, L"-") 
             || lex.Match(dummy, TokenType::Operator, L"~"))
               ++pos;

            // Value  {constant/variable/literal/null}
            if (lex.Match(pos, TokenType::Number) 
             || lex.Match(pos, TokenType::String) 
             || lex.Match(pos, TokenType::GameObject) 
             || lex.Match(pos, TokenType::ScriptObject) 
             || lex.Match(pos, TokenType::Variable))
               // (operator value)+   {simplify to check for any non-refobj operator)
               return lex.Match(pos, TokenType::Operator) && (pos-1)->Text != L"->";
            
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

            // 'skip' 'if'
            else if (lex.Match(pos, TokenType::Keyword, L"skip") && lex.Match(pos, TokenType::Keyword, L"if")) 
               return Conditional::SKIP_IF;

            // 'do' 'if'
            else if (lex.Match(pos, TokenType::Keyword, L"do") && lex.Match(pos, TokenType::Keyword, L"if")) 
               return Conditional::SKIP_IF_NOT;
               
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
            // Return NOP/Comment
            CommandSyntax syntax = SyntaxLib.Find(lex.count() == 0 ? CMD_NOP : CMD_COMMENT, Version);
            return ScriptCommand(*line, syntax, lex.Tokens);
         }


         ScriptCommand  ScriptParser::ReadCommand(const CommandLexer& lex, const LineIterator& line)
         {
            /*
            conditional = 'if'/'if not'/'while'/'while not'/'skip if'/'do if'
            assignment = variable '='
            
            command = (assignment/conditional)? (constant/variable/null '->')? text
            */
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

            // text: Hash remaining tokens
            CommandHash hash(pos, lex.end());
            CommandSyntax syntax = SyntaxLib.Identify(hash, Version);
            
            // TODO: Arrange parameters
            TokenArray params(hash.Parameters);

            // DEBUG:
            Console << GetLineNumber(line) << L": " << *line << ENDL;
            Console << (syntax == SyntaxLib.Unknown ? Colour::Red : Colour::Green) << hash.Hash << ENDL;

            // Create command
            return ScriptCommand(*line, syntax, params);
         }

         ScriptCommand  ScriptParser::ReadExpression(const CommandLexer& lex, const LineIterator& line)
         {
            /*
            conditional = 'if'/'if not'/'while'/'while not'/'skip if'/'do if'
            value = constant/variable/literal/null
            assignment = variable '='
            unary_operator = '!'/'-'/'~'

            expression = (assignment/conditional) unary_operator? value (operator value)+
            */
            Conditional   condition = Conditional::NONE;
            TokenIterator retVar = lex.end(),
                          pos = lex.begin();

            // (assignment/conditional)
            if (MatchAssignment(lex, lex.begin()))
                retVar = ReadAssignment(lex, pos);
            else 
                condition = ReadConditional(lex, pos);

            // unary_operator? value (operator value)+
            ExpressionParser exp(pos, lex.end());
            exp.Parse();  // nb: may throw 
            
            // TODO: Arrange parameters?
            //TokenArray params(hash.Parameters);

            // DEBUG:
            Console << GetLineNumber(line) << L": " << *line << ENDL;
            Console << Colour::Green << L"expression" << ENDL;

            // Create expression
            return ScriptCommand(*line, SyntaxLib.Find(CMD_EXPRESSION, Version), exp.InfixParams);
         }

         
         ScriptParser::CommandTree ScriptParser::ReadLine(LineIterator& line)
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
            command = (assignment/conditional)? (constant/variable/null '->')? text
            expression = (assignment/conditional) unary_operator? value (operator value)+
            */

            // Comment/NOP:
            if (MatchComment(lex))
               //cmd = ReadComment(lex, text);
               return CommandTree( new CommandNode(ReadComment(lex, text), GetLineNumber(text)) );

            // Command:
            else if (MatchCommand(lex))
               //cmd = ReadCommand(lex, text);
               return CommandTree( new CommandNode(ReadCommand(lex, text), GetLineNumber(text)) );

            // Expression:
            else if (MatchExpression(lex))
               //cmd = ReadExpression(lex, text);
               return CommandTree( new CommandNode(ReadExpression(lex, text), GetLineNumber(text)) );
            
            // * could potentially validate parameters at this point

            // Generate node, advance line
            return CommandTree( new CommandNode(ScriptCommand(), GetLineNumber(text)) );

         }


      }
   }
}
