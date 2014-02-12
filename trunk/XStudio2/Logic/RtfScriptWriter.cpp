#include "stdafx.h"
#include "ScriptFile.h"
#include "ScriptCommand.h"
#include "ScriptToken.h"
#include "CommandLexer.h"
#include "IndentationStack.h"
#include "RtfScriptWriter.h"

namespace Logic
{
   namespace IO
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Create an Rtf script writer</summary>
      /// <param name="out">Output stream</param>
      RtfScriptWriter::RtfScriptWriter(string&  out) : RtfWriter(out)
      {
         // Define colours
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

         // Init writer
         RtfWriter::Open(L"Arial", 10, col);

         // Set properties
         RtfWriter::SetForeColour(White);
         RtfWriter::SetBackColour(Black);
         RtfWriter::SetAlignment(Alignment::Left);
         RtfWriter::SetLeftMargin(500);
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Closes the writer.</summary>
      void  RtfScriptWriter::Close()
      {
         RtfWriter::Close();
      }

      /// <summary>Writes a script file to the output</summary>
      /// <param name="f">The script</param>
      void RtfScriptWriter::Write(ScriptFile& f)
      {
         IndentationStack indent;

         // Examine commands
         for (ScriptCommand& cmd : f.Commands.Input)
         {
            // Identation
            indent.PreDisplay(cmd);

            // Write command
            WriteCommand(cmd, 3*indent.Size);

            // Identation
            indent.PostDisplay(cmd);
         }
      }

		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------
      
      /// <summary>Writes a command to the output</summary>
      /// <param name="cmd">The command.</param>
      /// <param name="indent">Indent in characters</param>
      void  RtfScriptWriter::WriteCommand(const ScriptCommand& cmd, UINT  indent)
      {
         CommandLexer lex(cmd.Text, false);

         // Write indent
         RtfWriter::Write(wstring(indent, L' '));

         // Write command text
         for (const ScriptToken& tok : lex.Tokens)
            WriteToken(tok);

         // Add CRLF
         RtfWriter::WriteLn(L"");
      }

      /// <summary>Writes a token to the output.</summary>
      /// <param name="p">The token</param>
      void  RtfScriptWriter::WriteToken(const ScriptToken& tok)
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
         RtfWriter::SetForeColour(col);
         RtfWriter::Write(tok.Text);

         // Reset colour
         RtfWriter::SetForeColour(White);
      }

      
   }
}
