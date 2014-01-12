#include "stdafx.h"
#include "ScriptWriter.h"
#include "ScriptFile.h"
#include "ScriptCommand.h"
#include "RtfWriter.h"

namespace Logic
{
   namespace IO
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      ScriptWriter::ScriptWriter()
      {
      }


      ScriptWriter::~ScriptWriter()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

      //class CommandTextWriter
      //{
      //public:
      //   CommandTextWriter(const ScriptCommand& c) : Command(c), Marker(false)
      //   {}

      //   /// <summary>Notification that a parameter should be appended</summary>
      //   /// <param name="p">The parameter to append</param>
      //   virtual void  onParameter(const ScriptParameter&  p)
      //   {
      //      Output.append(p.Text);
      //   }

      //   /// <summary>Examines each letter of the syntax</summary>
      //   /// <param name="ch">The letter</param>
      //   virtual void  operator()(const WCHAR& ch)
      //   {
      //      switch (ch)
      //      {
      //      // Param Marker: Switch mode to marker
      //      case '$':
      //         Marker = true;
      //         break;

      //      // ParamID/Literal: Insert appropriate parameter 
      //      case '0': case '1': case '2': case '3': case '4': 
      //      case '5': case '6': case '7': case '8': case '9': 
      //         if (Marker)
      //            onParameter( Command.Parameters[ch-48] );
      //         else
      //            Output.push_back(ch);
      //         break;
      //         
      //      // Superscript marker: Ignore
      //      case L'º': case L'¹': case L'²': case L'³': case L'ª':
      //         break;

      //      // Possible Superscript marker: Ignore iff superscript marker
      //      case 'o': case 'x': case 'y': case 'z': case 'a':
      //         if (!Marker)
      //            Output.push_back(ch);
      //         break;
      //      
      //      // Text: Insert verbatim
      //      default:
      //         Marker = false;
      //         Output.push_back(ch);
      //         break;
      //      }
      //   }

      //   /// <summary>Get populated command text</summary>
      //   /// <returns>Command text</returns>
      //   wstring  ToString()
      //   {
      //      return Output;
      //   }

      //protected:
      //   const ScriptCommand&  Command;
      //   wstring Output;

      //private:
      //   bool  Marker;
      //};


      //
      //class RtfCommandTextWriter : public CommandTextWriter
      //{
      //public:
      //   RtfCommandTextWriter(const ScriptCommand& c, RtfWriter& w) : SyntaxInserter(c), Writer(w)
      //   {}

      //   /// <summary>Notification that a parameter should be appended</summary>
      //   /// <param name="p">The parameter to append</param>
      //   void  onParameter(const ScriptParameter&  p)
      //   {
      //      Command.Text.append(L"\\cf2 ");
      //      Command.Text.append(p.Text);
      //      Command.Text.append(L"\\cf0 ");
      //   }

      //private:
      //   RtfWriter& Writer;
      //};



      class RtfCommandWriter : public RtfWriter
      {
      public:
         RtfCommandWriter() : RtfWriter(L"Arial", 10, GetColours()) 
         {}

         void  Write(const ScriptCommand& cmd, UINT  indent)
         {
            bool Marker = false;

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
                     Write( cmd.Parameters[ch-48] );
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

      private:
         static list<COLORREF>  GetColours()
         {
            list<COLORREF> col;
            col.push_back(RGB(0,0,0));
            col.push_back(RGB(0,0,255));
            col.push_back(RGB(0,255,0));
            return col;
         }

         void  Write(const ScriptParameter& p)
         {
            switch (p.Type)
            {
            case DataType::DT_VARIABLE:
               RtfWriter::SetColour(RGB(0,255,0));
               RtfWriter::Write(p.Text);
               RtfWriter::SetColour(RGB(0,0,0));
               break;
            }
         }

         /*/// <summary>Notification that a parameter should be appended</summary>
         /// <param name="p">The parameter to append</param>
         void  onParameter(const ScriptParameter&  p)
         {
            RtfWriter::Write(L"\\cf2 ");
            RtfWriter::Write(p.Text);
            RtfWriter::Write(L"\\cf0 ");
         }*/
      };

      
      //void   ScriptCommand::TranslateRtf(ScriptFile& f, RtfWriter& w)
      //{
      //   // Translate parameters
      //   /*for (ScriptParameter& p : Parameters)
      //      p.Translate(f);*/

      //   // Populate command text
      //   RtfSyntaxInserter inserter(*this, w);
      //   for_each(Syntax.Text.begin(), Syntax.Text.end(), inserter);
      //}


      void RtfScriptWriter::Write(ScriptFile& f)
      {
         deque<BranchLogic> branch;
         RtfCommandWriter   w;

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

            case BranchLogic::Else:    
            case BranchLogic::End:      
               branch.pop_back();       
               break;
            }

            // Write command
            w.Write(cmd, 3*branch.size());

            //// Write Indent
            //w.Write(wstring(branch.size()*indent, L' '));

            //// Write command
            //cmd.TranslateRtf(*this, w);
            //w.Write(c.Text);

            // Re-Examine stack
            switch (logic)
            {
            case BranchLogic::If:
            case BranchLogic::Else:
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
   }
}
