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
