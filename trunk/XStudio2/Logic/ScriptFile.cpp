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
      /// <exception cref="Logic::ScriptObjectNotFoundException">Command matching ID is not present</exception>
      wstring  ScriptFile::GetCommandName() const
      {
         if (CommandID.Type == ValueType::Int && CommandID.Int > 0)
            return ScriptObjectLib.Find(ScriptObjectGroup::ObjectCommand, CommandID.Int).Text;

         else if (CommandID.Type == ValueType::String && CommandID.String.length() > 0)
            return ScriptObjectLib.Find(CommandID.String).Text;

         return wstring();
      }

      /// <summary>Gets whether script implements an object command.</summary>
      bool  ScriptFile::HasCommandName() const
      {
         return CommandID.Type != ValueType::Int || CommandID.Int > 0;
      }

		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

   }
}
