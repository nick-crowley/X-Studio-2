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


         ScriptParser::CommandTree ScriptParser::ReadCommand(LineIterator& line, BranchLogic logic)
         {
            CommandLexer  lex(*line);
            TokenIterator pos = lex.Tokens.begin();
            Conditional   c = Conditional::NONE;

            /*

            New command grammar:

            conditional = 'if'/'if not'/'while'/'while not'/'skip if'/'do if'
            value = constant/variable/literal/null
            assignment = variable '='
            comment = '*' text?
            nop = ws*

            line = nop/comment/command/expression
            command = (assignment/conditional)? (constant/variable/null '->')? text
            expression = (assignment/conditional) unary_operator? value (operator value)+

            */

            // Identify conditional. Consume only tokens that match.  [prev identified logic has matched the initial token(s)]
            switch (logic)
            {
            case BranchLogic::If:      
               c = (lex.Match(pos+1, TokenType::Keyword, L"not") ? (pos+=2, Conditional::IF_NOT) : (++pos, Conditional::IF));
               break;

            case BranchLogic::While:  
               c = (lex.Match(pos+1, TokenType::Keyword, L"not") ? (pos+=2, Conditional::WHILE_NOT) : (++pos, Conditional::WHILE));
               break;

            case BranchLogic::ElseIf:  
               c = (lex.Match(pos+2, TokenType::Keyword, L"not") ? (pos+=3, Conditional::ELSE_IF_NOT) : (pos+=2, Conditional::ELSE_IF));
               break;

            case BranchLogic::SkipIf:  
               if (lex.Match(pos, TokenType::Keyword, L"skip")) 
                  c = (lex.Match(pos+1, TokenType::Keyword, L"if") ? (pos+=2, Conditional::SKIP_IF) : (++pos, Conditional::NONE));

               else if (lex.Match(pos, TokenType::Keyword, L"do")) 
                  c = (lex.Match(pos+1, TokenType::Keyword, L"if") ? (pos+=2, Conditional::SKIP_IF_NOT) : (++pos, Conditional::NONE));
               break;

            case BranchLogic::Else: 
            case BranchLogic::Break: 
            case BranchLogic::Continue: 
               break;

            // Detect Assignment
            case BranchLogic::None:
               if (lex.Match(pos, TokenType::Variable) && lex.Match(pos+1, TokenType::Operator, L"=")) 
                  pos += 2;
               break;
            }
            
            // Generate hash from remaining tokens
            CommandHash hash(pos, lex.Tokens.end());
            
            // Lookup hash / supply parameters
            ScriptCommand cmd(*line, SyntaxLib.Identify(hash, Version), hash.Parameters);

            // DEBUG:
            Console << GetLineNumber(line) << L": " << *line << ENDL;
            Console << (cmd.Syntax == SyntaxLib.Unknown ? Colour::Red : Colour::Green) << hash.Hash << ENDL;
            
            // Generate CommandNode?
            return CommandTree(new CommandNode(logic, cmd, GetLineNumber(line++)));

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
