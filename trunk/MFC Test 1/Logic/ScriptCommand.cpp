#include "stdafx.h"
#include "ScriptCommand.h"
#include "ScriptFile.h"
#include <algorithm>

namespace Logic
{
   namespace Scripts
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      ScriptCommand::ScriptCommand(CommandSyntax& syntax, ParameterArray& params) 
         : Syntax(syntax), RefIndex(0), Parameters(params) 
      {
         //Translate();
      }

      ScriptCommand::ScriptCommand(CommandSyntax& syntax, UINT  ref, ParameterArray& params) 
         : Syntax(syntax), RefIndex(ref), Parameters(params) 
      {
         //Translate();
      }
      
      ScriptCommand::~ScriptCommand()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      // ------------------------------- PUBLIC METHODS -------------------------------

      class SyntaxInserter
      {
      public:
         SyntaxInserter(ScriptCommand& c) : Command(c), Marker(false)
         {}

         void  operator()(const WCHAR& ch)
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
                  Command.Text.append(Command.Parameters[ch-48].Text);
               else
                  Command.Text.push_back(ch);
               break;
               
            // Superscript marker: Ignore
            case L'º': case L'¹': case L'²': case L'³': case L'ª':
               break;

            // Possible Superscript marker: Ignore iff superscript marker
            case 'o': case 'x': case 'y': case 'z': case 'a':
               if (!Marker)
                  Command.Text.push_back(ch);
               break;
            
            // Text: Insert verbatim
            default:
               Marker = false;
               Command.Text.push_back(ch);
               break;
            }
         }

      private:
         ScriptCommand&  Command;
         bool  Marker;
      };

      void   ScriptCommand::Translate(ScriptFile& f)
      {
         // Translate parameters
         for (ScriptParameter& p : Parameters)
            p.Translate(f);

         // Populate command text
         SyntaxInserter inserter(*this);
         for_each(Syntax.Text.begin(), Syntax.Text.end(), inserter);
      }

		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

   }
}

