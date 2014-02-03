#pragma once
#include "Common.h"
#include "CommandSyntax.h"
#include "ParameterSyntax.h"
#include "ScriptCommand.h"
#include "MapIterator.hpp"
#include <algorithm>

namespace Logic
{
   namespace Scripts
   {
      
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


      /// <summary>Occurs when a script variable is missing</summary>
      class VariableNotFoundException : public ExceptionBase
      {
      public:
         /// <summary>Create from a variable name</summary>
         /// <param name="src">Location of throw</param>
         /// <param name="name">Variable name</param>
         VariableNotFoundException(wstring  src, const wstring& name) 
            : ExceptionBase(src, GuiString(L"Cannot find variable '$%s'", name.c_str()))
         {}

         /// <summary>Create from a variable ID</summary>
         /// <param name="src">Location of throw</param>
         /// <param name="id">Variable id</param>
         VariableNotFoundException(wstring  src, UINT id) 
            : ExceptionBase(src, GuiString(L"Cannot find variable with id=%d", id))
         {}
      };

      /// <summary></summary>
      class ScriptFile
      {
         // ------------------------ TYPES -------------------------
      public:
         /// <summary></summary>
         class VariableCollection : private map<wstring, ScriptVariable, less<wstring>>
         {
            // ------------------------ TYPES --------------------------
         private:
            typedef pair<const wstring,ScriptVariable>  element;
            typedef map<wstring, ScriptVariable, less<wstring>>  base;

         public:
            typedef MapIterator<ScriptVariable, VariableCollection, VariableCollection::iterator> VarIterator;

            // --------------------- CONSTRUCTION ----------------------

         public:
            VariableCollection();
            virtual ~VariableCollection();

            DEFAULT_COPY(VariableCollection);	// Default copy semantics
            DEFAULT_MOVE(VariableCollection);	// Default move semantics

            // ------------------------ STATIC -------------------------

            // --------------------- PROPERTIES ------------------------

            // ---------------------- ACCESSORS ------------------------			
         public:
            /// <summary>Get start iterator</summary>
            VarIterator begin()
            {
               return VarIterator(*this, base::begin());
            }
            
            /// <summary>Query presence of a variable</summary>
            /// <param name="name">name.</param>
            /// <returns></returns>
            bool Contains(const wstring& name)
            {
               return find(name) != base::end();
            }

            /// <summary>Get end iterator</summary>
            VarIterator end() 
            {
               return VarIterator(*this, base::end());
            }

            /// <summary>Get number of variables and arguments</summary>
            /// <returns></returns>
            size_type  GetCount() const
            {
               return base::size();
            }

            /// <summary>Get variable by ID</summary>
            /// <param name="id">id</param>
            /// <returns></returns>
            /// <exception cref="Logic::VariableNotFoundException">Not found</exception>
            ScriptVariable& operator[](UINT id)
            {
               // Lookup variable by ID
               iterator v = find_if(base::begin(), base::end(), [id](element& pair) {return pair.second.ID == id;});
               if (v != base::end())
                  return v->second;

               // Not found:
               throw VariableNotFoundException(HERE, id);
            }

            /// <summary>Get variable by ID</summary>
            /// <param name="id">id</param>
            /// <returns></returns>
            /// <exception cref="Logic::VariableNotFoundException">Not found</exception>
            const ScriptVariable& operator[](UINT id) const
            {
               // Lookup variable by ID
               const_iterator v = find_if(base::begin(), base::end(), [id](const element& pair) {return pair.second.ID == id;});
               if (v != base::end())
                  return v->second;

               // Not found:
               throw VariableNotFoundException(HERE, id);
            }

            /// <summary>Get variable by name</summary>
            /// <param name="name">name without $ prefix</param>
            /// <returns></returns>
            /// <exception cref="Logic::VariableNotFoundException">Not found</exception>
            ScriptVariable& operator[](const wstring& name)
            {
               iterator v;

               // Lookup variable by name
               if ((v=find(name)) != base::end())
                  return v->second;

               // Not found:
               throw VariableNotFoundException(HERE, name);
            }

         private:
            /// <summary>Gets the next free variable identifier.</summary>
            /// <returns>1-based variable ID</returns>
            UINT  GetNextID() const
            {
               return 1+size();
            }

            // ----------------------- MUTATORS ------------------------
         public:
            /// <summary>Adds a variable using the next available ID. If name already exists no changes are made</summary>
            /// <param name="name">variable name (case sensitive)</param>
            void  Add(const wstring& name)
            {
               if (!Contains(name))
                  insert( value_type(name, ScriptVariable(VariableType::Variable, name, GetNextID())) );
            }

            /// <summary>Clears all variables, but leaves arguments</summary>
            void  clear()
            {
               // Remove all variables
               for (iterator it = base::begin(); it != base::end(); )
                  if (it->second.Type == VariableType::Variable)
                     erase(it++);
                  else
                     ++it;
            }

            // -------------------- REPRESENTATION ---------------------

         private:
         };

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
               if (Contains(name) && index < Find(name).Variables.GetCount())
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

         /// <summary>Adds a label definition.</summary>
         /// <param name="name">label name.</param>
         /// <param name="name">line number.</param>
         void  AddLabel(const wstring& name, UINT line)
         {
            // Add label
            if (find_if(Labels.begin(), Labels.end(), [&name](const ScriptLabel& l){return l.Name==name;}) == Labels.end())
               Labels.push_back(ScriptLabel(name, line));
         }

         /// <summary>Adds variable.</summary>
         /// <param name="name">The name.</param>
         /*void  AddVariable(const wstring& name)
         {
            if (find_if(Variables.begin(), Variables.end(), [&name](const ScriptVariable& v){return v.Name==name;}) == Variables.end())
               Variables.push_back(ScriptVariable(VariableType::Variable, name, Variables.size()));
         }*/

         void  Clear();
         int   FindScope(UINT line);

		   // -------------------- REPRESENTATION ---------------------

         wstring        Name,
                        Description;
         UINT           Version;
         GameVersion    Game;
         bool           LiveData;
         ParameterValue CommandID;

         CommandList    Commands;
         LabelArray     Labels;
         VariableCollection   Variables;
         ScriptCallCollection ScriptCalls;
      private:
      };

   }
}

using namespace Logic::Scripts;
