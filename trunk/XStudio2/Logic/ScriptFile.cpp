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

      // ------------------------------- PUBLIC METHODS -------------------------------
      
      /// <summary>Clears commands, labels and non-argument variables</summary>
      void  ScriptFile::Clear()
      {
         // Clear labels & commands
         Commands.clear();
         Labels.clear();

         // Clear script-calls
         ScriptCalls.clear();

         // Clear variables, keep arguments
         Variables.clear();
      }

      /// <summary>Finds the index of the label that represents the scope of a line number</summary>
      /// <param name="line">The 1-based line number</param>
      /// <returns>Index into the labels array, or -1 for global scope</returns>
      int  ScriptFile::FindScope(UINT line)
      {
         // Determine current scope
         auto scope = find_if(Labels.rbegin(), Labels.rend(), [line](ScriptLabel& l) {return line >= l.LineNumber;} );

         // Convert to index
         return distance(scope, Labels.rend()) - 1;
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

		// ------------------------------ PROTECTED METHODS -----------------------------

		// ------------------------------- PRIVATE METHODS ------------------------------

   }
}
