#include "stdafx.h"
#include "RtfScriptTextWriter.h"
#include "CommandLexer.h"
#include "IndentationStack.h"

namespace Logic
{
   namespace IO
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Initializes an RtfCommandWriter</summary>
      /// <param name="font">Font name</param>
      /// <param name="size">Font size in points</param>
      RtfScriptTextWriter::RtfCommandWriter::RtfCommandWriter(const wstring& font, UINT size) : RtfWriter(font, size, GetColours())
      {
         RtfWriter::SetColour(White);
      }

      // ------------------------------- STATIC METHODS -------------------------------

      /// <summary>Populates the writer colour table</summary>
      /// <returns></returns>
      list<COLORREF>  RtfScriptTextWriter::RtfCommandWriter::GetColours()
      {
         list<COLORREF> col;
         col.push_back(Black);
         col.push_back(White);
         col.push_back(Red);
         col.push_back(Green);
         col.push_back(Blue);
         col.push_back(Yellow);
         col.push_back(Purple);
         col.push_back(Cyan);
         col.push_back(Grey);
         return col;
      }

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Closes the writer.</summary>
      void  RtfScriptTextWriter::Close()
      {
      }

      /// <summary>Writes a script file to the output</summary>
      /// <param name="f">The script</param>
      void RtfScriptTextWriter::Write(ScriptFile& f)
      {
         IndentationStack indent;
         RtfCommandWriter w(L"Arial", 10);

         // Examine commands
         for (ScriptCommand& cmd : f.Commands.Input)
         {
            indent.PreDisplay(cmd);

            // Write command
            w.Write(cmd, 3*indent.Size);

            indent.PostDisplay(cmd);
         }

         // Write RTF to output
         w.Close();
         Output = w.ToString();
      }

      /// <summary>Writes a command to the output</summary>
      /// <param name="cmd">The command.</param>
      /// <param name="indent">Indent in characters</param>
      void  RtfScriptTextWriter::RtfCommandWriter::Write(const ScriptCommand& cmd, UINT  indent)
      {
         CommandLexer lex(cmd.Text, false);

         // Write indent
         RtfWriter::Write(wstring(indent, L' '));

         // Write command text
         for (const ScriptToken& tok : lex.Tokens)
            Write(tok);

         // Add CRLF
         RtfWriter::WriteLn(L"");
      }

		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

      /// <summary>Writes a token to the output.</summary>
      /// <param name="p">The token</param>
      void  RtfScriptTextWriter::RtfCommandWriter::Write(const ScriptToken& tok)
      {
         COLORREF col;

         // Determine colour
         switch (tok.Type)
         {
         case TokenType::Whitespace:   RtfWriter::Write(tok.Text);  return;

         case TokenType::Comment:      col = Grey;    break;
         case TokenType::Null: 
         case TokenType::Variable:     col = Green;   break;
         case TokenType::Label:        col = Purple;  break;
         case TokenType::Keyword:      col = Blue;    break;
         case TokenType::Number:     
         case TokenType::String:       col = Red;     break;    
         case TokenType::GameObject:   col = Cyan;    break;    
         case TokenType::ScriptObject: col = Yellow;  break;        
         case TokenType::BinaryOp: 
         case TokenType::UnaryOp: 
         case TokenType::Text:         col = White;   break;

         throw ArgumentException(HERE, L"tok", GuiString(L"Unknown token type: %s", tok.Text.c_str()));
         }

         // Write token
         RtfWriter::SetColour(col);
         RtfWriter::Write(tok.Text);

         // Reset colour
         RtfWriter::SetColour(White);
      }

      
   }
}
