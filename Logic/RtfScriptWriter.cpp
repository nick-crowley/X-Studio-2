#include "stdafx.h"
#include "ScriptFile.h"
#include "ScriptCommand.h"
#include "ScriptToken.h"
#include "CommandLexer.h"
#include "IndentationStack.h"
#include "RtfScriptWriter.h"
#include "PreferencesLibrary.h"

namespace Logic
{
   namespace IO
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Create an Rtf script writer</summary>
      /// <param name="out">Output stream</param>
      /// <param name="font">Font name</param>
      /// <param name="size">Size in points</param>
      /// <exception cref="Logic::ArgumentNullException">Output is nullptr</exception>
      RtfScriptWriter::RtfScriptWriter(StreamPtr out, wstring font, UINT size) : RtfWriter(out)
      {
         set<COLORREF> colours;

         //enum class TokenType { Text, Number, String, GameObject, ScriptObject, Keyword, Variable, Null, Label, BinaryOp, UnaryOp, Comment, Whitespace };

         // Define unique colours
         colours.insert(Highlights.GetColour(TokenType::Text));
         colours.insert(Highlights.GetColour(TokenType::Number));
         colours.insert(Highlights.GetColour(TokenType::String));
         colours.insert(Highlights.GetColour(TokenType::GameObject));
         colours.insert(Highlights.GetColour(TokenType::ScriptObject));
         colours.insert(Highlights.GetColour(TokenType::Keyword));
         colours.insert(Highlights.GetColour(TokenType::Variable));
         colours.insert(Highlights.GetColour(TokenType::Null));
         colours.insert(Highlights.GetColour(TokenType::Label));
         colours.insert(Highlights.GetColour(TokenType::BinaryOp));
         colours.insert(Highlights.GetColour(TokenType::UnaryOp));
         colours.insert(Highlights.GetColour(TokenType::Comment));
         colours.insert(PrefsLib.ArgumentColour);
         colours.insert(PrefsLib.ConstantColour);
         colours.insert(PrefsLib.BackgroundColour);

         // Init writer
         RtfWriter::Open(font, size, list<COLORREF>(colours.begin(), colours.end()));

         // Set colours
         RtfWriter::SetForeColour(Highlights.GetColour(TokenType::Text));
         RtfWriter::SetBackColour(PrefsLib.BackgroundColour);

         // Set properties
         RtfWriter::SetAlignment(Alignment::Left);
         RtfWriter::SetLeftMargin(500);
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      /// <summary>Closes the writer.</summary>
      /// <exception cref="Logic::IOException">I/O error occurred</exception>
      void  RtfScriptWriter::Close()
      {
         RtfWriter::Close();
      }

      /// <summary>Writes a script file to the output</summary>
      /// <param name="f">The script</param>
      /// <exception cref="Logic::IOException">I/O error occurred</exception>
      void RtfScriptWriter::Write(ScriptFile& f)
      {
         IndentationStack stack;

         // Examine commands
         for (auto cmd = f.Commands.Input.begin(), end = f.Commands.Input.end(); cmd != end; ++cmd)
         {
            // Calc indent
            bool isSub = f.Commands.Input.IsSubRoutine(cmd);
            stack.PreDisplay(*cmd);

            // Write command
            WriteCommand(f, *cmd, stack.Indentation);

            // Calc indent
            stack.PostDisplay(*cmd, isSub);
         }
      }

		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------
      
      /// <summary>Writes a command to the output</summary>
      /// <param name="cmd">The command.</param>
      /// <param name="indent">Indentation</param>
      /// <exception cref="Logic::IOException">I/O error occurred</exception>
      void  RtfScriptWriter::WriteCommand(const ScriptFile& f, const ScriptCommand& cmd, const Indent& indent)
      {
         CommandLexer lex(cmd.Text, false);

         // Write indent
         RtfWriter::Write(indent);

         // Write command text
         for (const ScriptToken& tok : lex.Tokens)
            WriteToken(f, tok);

         // Add CRLF
         RtfWriter::WriteLn(L"");
      }

      /// <summary>Writes a token to the output.</summary>
      /// <param name="p">The token</param>
      /// <exception cref="Logic::IOException">I/O error occurred</exception>
      void  RtfScriptWriter::WriteToken(const ScriptFile& f, const ScriptToken& tok)
      {
         // Whitespace: Write verbatim
         if (tok.Type == TokenType::Whitespace)
            RtfWriter::Write(tok.Text); 
         else
         {  
            // Token: Determine colour
            RtfWriter::SetForeColour(Highlights.GetColour(f, tok));
            RtfWriter::Write(tok.Text);

            // Reset colour
            RtfWriter::SetForeColour(Highlights.GetColour(TokenType::Text));
         }
      }

      
   }
}
