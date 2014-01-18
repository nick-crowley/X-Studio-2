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
         }


         ScriptParser::~ScriptParser()
         {
         }

         // ------------------------------- STATIC METHODS -------------------------------

         // ------------------------------- PUBLIC METHODS -------------------------------

         /// <summary>Reads all commands in the script</summary>
         /// <returns></returns>
         ScriptParser::CommandTree  ScriptParser::ReadScript()
         {
            CommandTree tree(new CommandNode());
            
            // Iterate over lines
            for (LineIterator line = Input.begin(); line < Input.end(); )
            {
               // DEBUG:
               //Console.WriteLnf(L"Parsing line '%s'", line->c_str());

               // Peek logic
               switch (BranchLogic logic = PeekCommand(line))
               {
               // Conditional (Valid/Invalid): Read branch
               case BranchLogic::If:      
               case BranchLogic::While:  
               case BranchLogic::ElseIf:  
               case BranchLogic::Else:    
               case BranchLogic::SkipIf:  
                  tree->Add( ReadBranch(line, logic) );
                  break;

               // Command (Valid/Invalid): Read command
               default:
                  tree->Add( ReadCommand(line, logic) );
                  break;
               }
            }

            return tree;
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

         
         BranchLogic  ScriptParser::PeekCommand(const LineIterator& line) const
         {
            // Lex line to determine branch logic
            CommandLexer   lex(*line);
            TokenIterator  begin = lex.Tokens.begin();

            // NOP/Comment
            if (lex.Tokens.size() == 0 || lex.Match(begin, TokenType::Operator, L"*"))
               return BranchLogic::NOP;

            // If/SkipIf/DoIf
            if (lex.Match(begin, TokenType::Keyword, L"if"))
               return BranchLogic::If;

            if (lex.Match(begin, TokenType::Keyword, L"skip") || lex.Match(begin, TokenType::Keyword, L"do"))
               return BranchLogic::SkipIf;

            // End
            else if (lex.Match(begin, TokenType::Keyword, L"end"))
               return BranchLogic::End;

            // While
            else if (lex.Match(begin, TokenType::Keyword, L"while"))
               return BranchLogic::While;

            // Break/Continue
            else if (lex.Match(begin, TokenType::Keyword, L"break"))
               return BranchLogic::Break;

            else if (lex.Match(begin, TokenType::Keyword, L"continue"))
               return BranchLogic::Continue;

            // Else/ElseIf
            else if (lex.Match(begin, TokenType::Keyword, L"else"))
               return lex.Match(begin+1, TokenType::Keyword, L"if") ? BranchLogic::ElseIf : BranchLogic::Else;

            // Command/Unrecognised:
            return BranchLogic::None;
         }



         bool  ScriptParser::MatchAssignment(const CommandLexer& lex, TokenIterator& pos)
         {
            // Assignment: variable '='
            return lex.Match(pos, TokenType::Variable) && lex.Match(++pos, TokenType::Operator, L"=") ? (++pos, true) : false;
         }

         bool  ScriptParser::MatchConditional(const CommandLexer& lex)
         {
            TokenIterator pos = lex.begin();

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

         
         bool  ScriptParser::MatchComment(const CommandLexer& lex)
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


         bool  ScriptParser::MatchCommand(const CommandLexer& lex)
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














         

         TokenIterator  ScriptParser::ReadAssignment(CommandLexer& lex, TokenIterator& pos)
         {
            TokenIterator retVar = pos;
            return (pos += 2, retVar);
         }

         
         Conditional ScriptParser::ReadConditional(CommandLexer& lex, TokenIterator& pos)
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
               
            throw "Invalid conditional - use sentinel syntax";
         }
         
         TokenIterator ScriptParser::ReadReferenceObject(CommandLexer& lex, TokenIterator& pos)
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


         ScriptCommand  ScriptParser::ReadCommand2(const CommandLexer& lex, const LineIterator& line)
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
            if (MatchReferenceObject(lex, pos))
               refObj = ReadReferenceObject(lex, pos);

            // text: Hash remaining tokens
            CommandHash hash(pos, lex.end());
            CommandSyntax syntax = SyntaxLib.Identify(hash, Version);
            
            // TODO: Arrange parameters
            TokenArray params(hash.Parameters);

            // DEBUG:
            Console << GetLineNumber(line) << L": " << *line << ENDL;
            Console << (cmd.Syntax == SyntaxLib.Unknown ? Colour::Red : Colour::Green) << hash.Hash << ENDL;

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
            exp.Parse();   // throws
            
            // TODO: Arrange parameters?
            TokenArray params(hash.Parameters);

            // DEBUG:
            Console << GetLineNumber(line) << L": " << *line << ENDL;
            Console << (cmd.Syntax == SyntaxLib.Unknown ? Colour::Red : Colour::Green) << hash.Hash << ENDL;

            // Create expression
            return ScriptCommand(*line, SyntaxLib.Find(CMD_EXPRESSION, Version), exp.InfixParams, exp.PostfixParams);
         }


         ScriptParser::CommandTree ScriptParser::ReadCommand(LineIterator& line, BranchLogic logic)
         {
            /*
            conditional = 'if'/'if not'/'while'/'while not'/'skip if'/'do if'
            value = constant/variable/literal/null
            assignment = variable '='
            comment = '*' text?
            nop = ws*

            line = nop/comment/command/expression
            command = (assignment/conditional)? (constant/variable/null '->')? text
            expression = (assignment/conditional) unary_operator? value (operator value)+
            */

            
            LineIterator  text = line++;  // consume line
            CommandLexer  lex(*text);
            ScriptCommand cmd;            // 'Unknown' syntax

            // Comment/NOP:
            if (MatchComment(lex))
               cmd = ReadComment(lex, text);

            // Command:
            else if (MatchCommand(lex))
               cmd = ReadCommand2(lex, text);

            // Expression:
            else if (MatchExpression(lex))
               cmd = ReadExpression(lex, text);
            
            // * could potentially validate parameters at this point


            // Generate node, advance line
            return CommandTree( new CommandNode(cmd, GetLineNumber(text)) );

         }

         ScriptParser::CommandTree ScriptParser::ReadBranch(LineIterator& line, BranchLogic logic)
         {
            // Read conditional command
            CommandTree tree(ReadCommand(line, logic));

            // Read children
            while (line < Input.end())
            {
               switch (BranchLogic logic = PeekCommand(line))
               {
               // Conditional (Valid/Invalid): Read command tree
               case BranchLogic::If:      
               case BranchLogic::While:  
               case BranchLogic::ElseIf:  
               case BranchLogic::Else:    
               case BranchLogic::SkipIf:  
                  tree->Add( ReadBranch(line, logic) );
                  break;

               // Comment: Always valid, read any/all
               case BranchLogic::NOP: 
                  tree->Add( ReadCommand(line, BranchLogic::NOP) );
                  break;

               // End: Read for if/while, otherwise kick back to parent
               case BranchLogic::End:     
                  if (tree->Logic == BranchLogic::If || tree->Logic == BranchLogic::While)
                     tree->Add( ReadCommand(line, BranchLogic::End) );
                  return tree;

               // Command: Read only 1 command under skip-if
               default: 
                  tree->Add( ReadCommand(line, logic) );
                  if (tree->Logic == BranchLogic::SkipIf)
                     return tree;
                  break;
               }
            }

            return CommandTree(tree.get());
         }
      }
   }
}
