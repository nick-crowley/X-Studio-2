#include "stdafx.h"
#include "ScriptFile.h"
#include "RtfWriter.h"
#include "ScriptObjectLibrary.h"

namespace Logic
{
   namespace Scripts
   {
      // -------------------------------- CONSTRUCTION --------------------------------

      ScriptFile::ScriptFile() : Version(0), Game(GameVersion::Threat), LiveData(false) 
      {
      }


      ScriptFile::~ScriptFile()
      {
      }

      // ------------------------------- STATIC METHODS -------------------------------

      /// <summary>Get variable type string</summary>
      GuiString  GetString(VariableType t)
      {
         return t == VariableType::Variable ? L"Variable"
              : t == VariableType::Argument ? L"Argument" 
                                            : L"Invalid";
      }

      /// <summary>Write script variable to the console</summary>
      ConsoleWnd& operator<<(ConsoleWnd& c, const ScriptVariable& v)
      {
         return c << "{ScriptVariable: Name=" << v.Name << " ID=" << v.ID << " ValueType=" << GetString(v.ValueType) << " Type=" << GetString(v.Type) << "}";
      }

      // ------------------------------- PUBLIC METHODS -------------------------------
      
      /// <summary>Clears commands, labels and non-argument variables</summary>
      void  ScriptFile::Clear()
      {
         // Clear labels & commands
         Commands.Clear();
         Labels.clear();

         // Clear script-calls
         ScriptCalls.Clear();

         // Clear variables, keep arguments
         Variables.clear();
      }

      /// <summary>Finds the location of the next match, if any</summary>
      /// <param name="src">search data</param>
      /// <returns>True if found, false otherwise</returns>
      bool  ScriptFile::FindNext(SearchOperation& src)
      {
         GuiString text;

         // Flatten translated command text. Mimic the format of RichEdit (double-char line break), so char indicies match up
         for (const auto& cmd : Commands.Input)
         {
            text += cmd.Text;
            text += L"\r\n";
         }
         
         // Find from last match + set location if found
         auto pos = text.find(src.Term, src.LastMatch.cpMax);
         src.SetMatch(pos != GuiString::npos ? pos : -1);
         
         // Return result
         return pos != GuiString::npos;
      }

      /// <summary>Finds the name of the label that represents the scope of a line number</summary>
      /// <param name="line">The 1-based line number</param>
      /// <returns>Label name, or empty string if global</returns>
      wstring  ScriptFile::FindScope(UINT line)
      {
         // Sort labels into descending order
         vector<ScriptLabel> coll(Labels.begin(), Labels.end());
         sort(coll.begin(), coll.end(), [](ScriptLabel& a, ScriptLabel&b){ return a.LineNumber > b.LineNumber; });

         // Determine current scope
         auto scope = find_if(coll.begin(), coll.end(), [line](ScriptLabel& l) {return line >= l.LineNumber;} );
         return scope != coll.end() ? scope->Name : L"";
      }

      /// <summary>Gets the name of the object command.</summary>
      /// <returns>Name of command if any, otherwise empty string</returns>
      wstring  ScriptFile::GetCommandName() const
      {
         const ScriptObject* cmd;

         if (CommandID.Type == ValueType::String)
            return CommandID.String;

         else if (CommandID.Int != 0 && ScriptObjectLib.TryFind(ScriptObjectGroup::ObjectCommand, CommandID.Int, cmd))
            return cmd->Text;
         
         return L"";   
      }

		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

   }
}
