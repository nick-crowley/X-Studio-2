#pragma once
#include "Common.h"
#include "CommandSyntax.h"
#include "ParameterSyntax.h"
#include "ScriptCommand.h"
#include <algorithm>

namespace Logic
{
   namespace Scripts
   {
      /// <summary>Distinguishes variables from arguments</summary>
      enum class VariableType : UINT { Argument, Variable };

      /// <summary>An argument or variable used within a script</summary>
      class ScriptVariable
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         ScriptVariable(VariableType t, const wstring& name, UINT index) 
            : Type(t), Name(name), ID(index), ValueType(ParameterType::UNDETERMINED)
         {}

         // -------------------- REPRESENTATION ---------------------

         wstring        Name,
                        Description;
         ParameterType  ValueType;
         VariableType   Type;
         UINT           ID;       // 1-based index/ID
      };

      /// <summary>Vector of ScriptVariables</summary>
      typedef vector<ScriptVariable>  VariableArray;


      /// <summary></summary>
      class ScriptLabel
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         ScriptLabel(wstring name, UINT line) : Name(name), LineNumber(line)
         {}

         // -------------------- REPRESENTATION ---------------------

         wstring  Name;
         UINT     LineNumber;    // 1-based line number
      };

      /// <summary>Vector of ScriptLabels</summary>
      typedef vector<ScriptLabel>  LabelArray;


      /// <summary></summary>
      class ScriptFile
      {
         // ------------------------ TYPES -------------------------
      public:
         class ScriptCallCollection : public map<wstring, ScriptFile>
         {
         public:
            /// <summary>Checks for presence of script (without extension, case sensitive)</summary>
            /// <param name="name">script name.</param>
            /// <returns></returns>
            bool  Contains(const wstring& name) const
            {
               return find(name) != end();
            }

            /// <summary>Adds a script by name (without extension, case sensitive).</summary>
            /// <param name="name">script name.</param>
            /// <param name="f">script properties</param>
            void  Add(const wstring& name, ScriptFile& f)
            {
               insert(value_type(name, f));
            }

            /// <summary>Finds a script by name (without extension, case sensitive).</summary>
            /// <param name="name">script name</param>
            /// <returns></returns>
            /// <exception cref="Logic::InvalidOperationException">Script not present</exception>
            const ScriptFile&  Find(const wstring& name)
            {
               const_iterator pos;
               // Lookup and return
               if ((pos = find(name)) != end())
                  return pos->second;

               // Not found
               throw InvalidOperationException(HERE, GuiString(L"No properties loaded for external script '%s'", name.c_str()));
            }

            /// <summary>Finds a name of script argument by index.</summary>
            /// <param name="name">script name (without extension, case sensitive)</param>
            /// <param name="index">Zero based index</param>
            /// <returns>Argument name if found, otherwise placeholder</returns>
            /// <exception cref="Logic::InvalidOperationException">Script not present</exception>
            wstring  FindArgument(const wstring& name, UINT index)
            {
               // Lookup name + Validate index
               if (Contains(name) && index < Find(name).Variables.size())
                  return Find(name).Variables[index].Name;
               
               // Missing/Invalid: 
               return GuiString(L"argument%d", index+1);
            }
         };

         // --------------------- CONSTRUCTION ----------------------
      public:
         ScriptFile();
         virtual ~ScriptFile();

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
			
         PROPERTY_GET(wstring,CommandName,GetCommandName);

		   // ---------------------- ACCESSORS ------------------------
			
         /// <summary>Gets the name of the object command.</summary>
         /// <returns>Name of command if any, otherwise empty string</returns>
         /// <exception cref="Logic::ScriptObjectNotFoundException">Command matching ID is not present</exception>
         wstring GetCommandName() const;

		   // ----------------------- MUTATORS ------------------------

         /// <summary>Clears commands, labels and non-argument variables</summary>
         void  Clear()
         {
            // Clear labels & commands
            Commands.clear();
            Labels.clear();

            // Clear script-calls
            ScriptCalls.clear();

            // Clear variables, keep arguments
            auto var = find_if(Variables.begin(), Variables.end(), [](const ScriptVariable& v) { return v.Type == VariableType::Variable; });
            Variables.erase(var, Variables.end());
         }

         /// <summary>Finds the index of the label that represents the scope of a line number</summary>
         /// <param name="line">The 1-based line number</param>
         /// <returns>Index into the labels array, or -1 for global scope</returns>
         int  FindScope(UINT line)
         {
            // Determine current scope
            auto scope = find_if(Labels.rbegin(), Labels.rend(), [line](ScriptLabel& l) {return line >= l.LineNumber;} );

            // Convert to index
            return distance(scope, Labels.rend()) - 1;
         }

		   // -------------------- REPRESENTATION ---------------------

         wstring        Name,
                        Description;
         UINT           Version;
         GameVersion    Game;
         bool           LiveData;
         ParameterValue CommandID;

         CommandList    Commands;
         LabelArray     Labels;
         VariableArray  Variables;
         ScriptCallCollection ScriptCalls;
      private:
      };

   }
}

using namespace Logic::Scripts;
