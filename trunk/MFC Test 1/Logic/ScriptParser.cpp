#include "stdafx.h"
#include "ScriptParser.h"
#include "ScriptExpressionParser.h"
#include "SyntaxLibrary.h"

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
            ScriptCommandLexer lex(*line);
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


         /// <summary>Reads all commands in the script</summary>
         /// <returns></returns>
         ScriptParser::CommandTree  ScriptParser::ReadScript()
         {
            shared_ptr<BranchNode> tree(new BranchNode());
            
            // Iterate over lines
            for (LineIterator line = Input.begin(); line < Input.end(); )
            {
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

         ScriptParser::CommandTree ScriptParser::ReadCommand(LineIterator& line, BranchLogic logic)
         {
            ScriptCommandLexer lex(*line);
            TokenIterator pos = lex.Tokens.begin();
            Conditional c = Conditional::NONE;
            
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

            case BranchLogic::None:
               // Assignment
               if (lex.Match(pos, TokenType::Variable) && lex.Match(pos, TokenType::Operator, L"=")) 
                  pos += 2;
            }
            
            // Generate hash from remaining tokens
            CommandHash hash(pos, lex.Tokens.end());
            
            // Lookup hash / supply parameters
            ScriptCommand cmd(SyntaxLib.Identify(hash), hash.Parameters);
            
            // Generate CommandNode?
            return CommandTree(new CommandNode(c, cmd, GetLineNumber(line)));

         }

         ScriptParser::CommandTree ScriptParser::ReadBranch(LineIterator& line, BranchLogic logic)
         {
            // Read conditional command
            shared_ptr<BranchNode> tree(ReadCommand(line, logic));

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


         // ------------------------------ PROTECTED METHODS -----------------------------

         // ------------------------------- PRIVATE METHODS ------------------------------


         //ScriptCommand  ScriptParser::GenerateCommand(UINT id, Conditional c, TokenArray& params)
         //{
         //   throw "Not impl";
         //}
         //ScriptCommand  ScriptParser::GenerateCommand(UINT id, const wstring& retVar, TokenArray& params)
         //{
         //   throw "Not impl";
         //}
         //ScriptCommand  ScriptParser::GenerateCommand(UINT id, TokenArray& params)
         //{
         //   throw "Not impl";
         //}

         //bool  ScriptParser::Match(const TokenIterator& pos, TokenType  type)
         //{
         //   return pos < Lexer.Tokens.end() && pos->Type == type;
         //}

         //bool  ScriptParser::Match(const TokenIterator& pos, TokenType  type, const TCHAR* txt)
         //{
         //   return Match(pos, type) && pos->Text == txt;
         //}

         //ScriptCommand  ScriptParser::MatchLine()
         //{
         //   TokenIterator  pos = Lexer.Tokens.begin();
         //   TokenArray     params;
         //   UINT           id;

         //   // NOP:
         //   if (MatchNOP(pos))
         //      return GenerateCommand(CMD_NOP);

         //   // Comment:
         //   if (MatchComment(pos))
         //      return GenerateCommand(CMD_COMMENT, Lexer.Tokens);

         //   // Command/Expr Assignment
         //   else if (MatchAssignment(pos))
         //      return ReadCommand(pos, ReadAssignment(pos));

         //   // Command/Expr Conditional
         //   else if (MatchConditional(pos))
         //      return ReadCommand(pos, ReadConditional(pos));
         //   
         //   // Command
         //   ReadCommand(pos);
         //}

         //bool  ScriptParser::MatchAssignment(const TokenIterator& pos) const
         //{
         //   // Match: variable, '='
         //   return Match(pos, TokenType::Variable) && Match(pos[1], TokenType::Operator, L"=");
         //}

         //bool  ScriptParser::MatchConditional(const TokenIterator& pos) const
         //{
         //   // if, if not
         //   return Match(pos, TokenType::Keyword, L"if")
         //       || Match(pos, TokenType::Keyword, L"while")
         //       || Match(pos, TokenType::Keyword, L"skip")
         //       || Match(pos, TokenType::Keyword, L"do");
         //}

         //bool  ScriptParser::MatchNOP(const TokenIterator& pos) const
         //{
         //   // Match zero tokens
         //   return pos == Lexer.Tokens.begin() && pos == Lexer.Tokens.end();
         //}

         //const ScriptToken&  ScriptParser::ReadAssignment(TokenIterator& pos)
         //{
         //   const ScriptToken& retVar = *pos;
         //   return (pos += 2, retVar);
         //}

         //bool  ScriptParser::MatchCommand(TokenIterator& pos, UINT& id, TokenArray& params)
         //{
         //   // Clear params
         //   params.clear();
         //   id = CMD_NONE;

         //   // Hash remaining tokens
         //   CommandHash hash(pos, Lexer.Tokens.end());
         //   
         //   // Lookup hash, copy parameters and consume all tokens
         //   //return id = SyntaxLib.Identify(hash.Hash, Version) ? (pos=Lexer.Tokens.end(), params=hash.Parameters, true) : false;
         //   throw "not impl";
         //}

         //bool ScriptParser::MatchExpression(TokenIterator& pos, TokenArray& params)
         //{
         //   //return ScriptExpressionParser(pos, Lexer.Tokens.end(), params).Parse();
         //   throw "not impl";
         //}

         
      }
   }
}
