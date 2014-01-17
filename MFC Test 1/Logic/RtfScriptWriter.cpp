#include "stdafx.h"
#include "ScriptWriter.h"

namespace Logic
{
   namespace IO
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      /// <summary>Initializes an RtfCommandWriter</summary>
      /// <param name="font">Font name</param>
      /// <param name="size">Font size in points</param>
      RtfScriptWriter::RtfCommandWriter::RtfCommandWriter(const wstring& font, UINT size) : RtfWriter(font, size, GetColours())
      {
         RtfWriter::SetColour(White);
      }

      // ------------------------------- STATIC METHODS -------------------------------

      /// <summary>Populates the writer colour table</summary>
      /// <returns></returns>
      list<COLORREF>  RtfScriptWriter::RtfCommandWriter::GetColours()
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

      /// <summary>Writes a script file to the output</summary>
      /// <param name="f">The script</param>
      void RtfScriptWriter::Write(ScriptFile& f)
      {
         deque<BranchLogic> branch;
         RtfCommandWriter   w(L"Arial", 10);

         // Examine commands
         for (ScriptCommand& cmd : f.Commands)
         {
            BranchLogic logic = cmd.GetConditional();

            // Push/pop branching logic from stack
            switch (logic)
            {
            case BranchLogic::Break:
            case BranchLogic::Continue: 
               branch.push_back(logic); 
               break;

            case BranchLogic::ElseIf:
            case BranchLogic::Else:    
            case BranchLogic::End:      
               branch.pop_back();       
               break;
            }

            // Write command
            w.Write(cmd, 3*branch.size());

            // Re-Examine stack
            switch (logic)
            {
            case BranchLogic::If:
            case BranchLogic::While:
            case BranchLogic::Else:
            case BranchLogic::ElseIf:
               branch.push_back(logic);
               break;

            case BranchLogic::Break:
            case BranchLogic::Continue:
               branch.pop_back();
               break;

            case BranchLogic::SkipIf:
               branch.push_back(logic); 
               continue;
            }

            // Pop 'SkipIf' after next standard command
            if (branch.size() > 0 && cmd.Syntax.Type == CommandType::Standard && branch.back() == BranchLogic::SkipIf)
               branch.pop_back();
         }

         // Write RTF to output
         w.Close();
         Output = w.ToString();
      }

      /// <summary>Writes a command to the output</summary>
      /// <param name="cmd">The command.</param>
      /// <param name="indent">Indent in characters</param>
      void  RtfScriptWriter::RtfCommandWriter::Write(const ScriptCommand& cmd, UINT  indent)
      {
         bool Marker = false;

         // Set colour
         RtfWriter::SetColour(cmd.Syntax.IsKeyword() ? Blue : White);

         // Write indent
         RtfWriter::Write(wstring(indent, L' '));

         // Write command text
         for (const WCHAR& ch : cmd.Syntax.Text)
         {
            switch (ch)
            {
            // Param Marker: Switch mode to marker
            case '$':
               Marker = true;
               break;

            // ParamID/Literal: Insert appropriate parameter 
            case '0': case '1': case '2': case '3': case '4': 
            case '5': case '6': case '7': case '8': case '9': 
               if (Marker)
                  Write(cmd, cmd.Parameters[ch-48]);
               else
                  RtfWriter::Write(ch);
               break;
               
            // Superscript marker: Ignore
            case L'º': case L'¹': case L'²': case L'³': case L'ª':
               break;

            // Possible Superscript marker: Ignore iff superscript marker
            case 'o': case 'x': case 'y': case 'z': case 'a':
               if (!Marker)
                  RtfWriter::Write(ch);
               break;
            
            // Text: Insert verbatim
            default:
               Marker = false;
               RtfWriter::Write(ch);
               break;
            }
         }

         // Add CRLF
         RtfWriter::WriteLn(L"");
      }

		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

      /// <summary>Writes a parameter to the output.</summary>
      /// <param name="cmd">The command containing the parameter</param>
      /// <param name="p">The parameter</param>
      void  RtfScriptWriter::RtfCommandWriter::Write(const ScriptCommand& cmd, const ScriptParameter& p)
      {
         COLORREF col;

         // Determine colour
         switch (p.Type)
         {
         case DataType::VARIABLE:  
            col = (p.Syntax.IsRetVar() && cmd.GetConditional() != BranchLogic::None ? Blue : Green);  
            break;

         case DataType::STRING:    
            col = (p.Syntax.Type == ParameterType::COMMENT ? Grey 
                  : p.Syntax.Type == ParameterType::LABEL_NAME ? Purple
                  : Red);      
            break;

         case DataType::INTEGER:   col = Red;      break;
         case DataType::WARE:      col = Cyan;     break;
         case DataType::Null:      col = Green;    break;
         case DataType::OPERATOR:  col = White;    break;

         case DataType::SECTOR:
         case DataType::SCRIPTDEF:     
         case DataType::STATIONSERIAL: 
         case DataType::TRANSPORTCLASS:
         case DataType::DATATYPE:      
         case DataType::FLIGHTRETURN:  
         case DataType::OBJECTCLASS:   
         case DataType::OBJECTCOMMAND:  
         case DataType::WINGCOMMAND:    
         case DataType::RACE:          
         case DataType::CONSTANT:  col = Yellow;   break;
            
         default:  
            RtfWriter::Write(p.Text);  
            return;
         }

         // Write parameter
         RtfWriter::SetColour(col);
         RtfWriter::Write(p.Text);

         // Reset colour
         RtfWriter::SetColour(White);
      }

      
   }
}
