#pragma once
#include "Common.h"
#include "CommandSyntax.h"
#include "ParameterSyntax.h"
#include "ScriptCommand.h"
#include "SearchOperation.h"
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

         // ---------------------- ACCESSORS ------------------------	

         /// <summary>Sort by line number</summary>
         bool operator<(const ScriptLabel& r)   { return LineNumber < r.LineNumber; }
         bool operator>(const ScriptLabel& r)   { return LineNumber > r.LineNumber; }

         // -------------------- REPRESENTATION ---------------------

         wstring  Name;
         UINT     LineNumber;    // 1-based line number
      };


      /// <summary>Distinguishes variables from arguments</summary>
      enum class VariableType : UINT { Argument, Variable };

      /// <summary>Get variable type string</summary>
      GuiString  GetString(VariableType t);

      /// <summary>An argument or variable used within a script</summary>
      class ScriptVariable
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         /// <summary>Create a named variable</summary>
         /// <param name="name">name.</param>
         /// <param name="id">1-based ID.</param>
         ScriptVariable(const wstring& name, UINT id)
            : Type(VariableType::Variable), Name(name), ID(id), ValueType(ParameterType::UNRECOGNISED)
         {}

         // -------------------- REPRESENTATION ---------------------

         wstring        Name,
                        Description;
         ParameterType  ValueType;
         VariableType   Type;
         UINT           ID;       // 1-based ID
      };

      /// <summary>Write script variable to the console</summary>
      ConsoleWnd& operator<<(ConsoleWnd& c, const ScriptVariable& v);

      /// <summary>Vector of script variables</summary>
      class VariableArray : public vector<ScriptVariable>
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         // --------------------- PROPERTIES ------------------------

         PROPERTY_GET(VariableArray,SortByID,GetSortByID);

         // ---------------------- ACCESSORS ------------------------	
      public:
         VariableArray GetSortByID() const
         {
            VariableArray vars;
            // Copy all, sort by ID
            copy(begin(), end(), back_inserter(vars));
            sort(vars.begin(), vars.end(), [](const ScriptVariable& a,const ScriptVariable& b) {return a.ID < b.ID;} );
            return vars;
         }

         // -------------------- REPRESENTATION ---------------------
      };


      /// <summary>Occurs when a script label is missing</summary>
      class LabelNotFoundException : public ExceptionBase
      {
      public:
         /// <summary>Create from a label name</summary>
         /// <param name="src">Location of throw</param>
         /// <param name="name">Label name</param>
         LabelNotFoundException(wstring  src, const wstring& name) 
            : ExceptionBase(src, GuiString(L"Cannot find label '$%s'", name.c_str()))
         {}
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
         class LabelCollection : private map<wstring, ScriptLabel, less<wstring>>
         {
            // ------------------------ TYPES --------------------------
         private:
            typedef pair<const wstring,ScriptLabel>           element;
            typedef map<wstring, ScriptLabel, less<wstring>>  base;

         public:
            typedef MapIterator<ScriptLabel, LabelCollection, LabelCollection::iterator> LabelIterator;
            typedef MapIterator<const ScriptLabel, LabelCollection, LabelCollection::const_iterator> ConstIterator;

            // --------------------- CONSTRUCTION ----------------------

         public:
            LabelCollection()
            {}

            DEFAULT_COPY(LabelCollection);	// Default copy semantics
            DEFAULT_MOVE(LabelCollection);	// Default move semantics

            // ------------------------ STATIC -------------------------

            // --------------------- PROPERTIES ------------------------
         public:
            PROPERTY_GET(size_type,Count,GetCount);

            // ---------------------- ACCESSORS ------------------------			
            
            /// <summary>Get start iterator</summary>
            LabelIterator begin()       { return LabelIterator(*this, base::begin());  }
            ConstIterator begin() const { return ConstIterator(*this, base::cbegin()); }
            
            /// <summary>Get end iterator</summary>
            LabelIterator end()         { return LabelIterator(*this, base::end());  }
            ConstIterator end() const   { return ConstIterator(*this, base::cend()); }
            
            /// <summary>Query presence of a label</summary>
            /// <param name="name">name.</param>
            /// <returns></returns>
            bool Contains(const wstring& name) const
            {
               return find(name) != base::end();
            }

            /// <summary>Get number of labels</summary>
            /// <returns></returns>
            size_type  GetCount() const
            {
               return base::size();
            }

            /// <summary>Get label by name</summary>
            /// <param name="name">name without ':' operator</param>
            /// <returns></returns>
            /// <exception cref="Logic::LabelNotFoundException">Not found</exception>
            ScriptLabel& operator[](const wstring& name)
            {
               iterator v;

               // Lookup label by name
               if ((v=find(name)) != base::end())
                  return v->second;

               // Not found:
               throw LabelNotFoundException(HERE, name);
            }

            /// <summary>Get label by index</summary>
            /// <param name="index">zero based index</param>
            /// <returns></returns>
            /// <exception cref="Logic::IndexOutOfRangeException">Not found</exception>
            ScriptLabel& operator[](const int index)
            {
               int i = 0;

               // Lookup label by index
               for (LabelIterator it = begin(); it != end(); ++it)
                  if (index == i++)
                     return *it;

               // Not found:
               throw IndexOutOfRangeException(HERE, index, Count);
            }

            // ----------------------- MUTATORS ------------------------

            /// <summary>Adds a label name to the collection</summary>
            /// <param name="name">label name</param>
            /// <param name="line">1-based line number</param>
            /// <returns>True if inserted, False if already present</returns>
            bool  Add(const wstring& name, UINT line)
            {
               return insert( value_type(name, ScriptLabel(name, line)) ).second;
            }

            /// <summary>Clears all labels</summary>
            void  clear()
            {
               base::clear();
            }

            // -------------------- REPRESENTATION ---------------------

         private:
         };

         /// <summary></summary>
         class VariableCollection : private map<wstring, ScriptVariable, less<wstring>>
         {
            // ------------------------ TYPES --------------------------
         private:
            typedef pair<const wstring,ScriptVariable>  element;
            typedef map<wstring, ScriptVariable, less<wstring>>  base;

         public:
            typedef MapIterator<ScriptVariable, VariableCollection, VariableCollection::iterator> VarIterator;
            typedef MapIterator<const ScriptVariable, VariableCollection, VariableCollection::const_iterator> ConstIterator;

            // --------------------- CONSTRUCTION ----------------------

         public:
            VariableCollection()
            {}

            DEFAULT_COPY(VariableCollection);	// Default copy semantics
            DEFAULT_MOVE(VariableCollection);	// Default move semantics

            // ------------------------ STATIC -------------------------

            // --------------------- PROPERTIES ------------------------

            PROPERTY_GET(VariableArray,All,GetAll);
            PROPERTY_GET(VariableArray,Arguments,GetArguments);
            PROPERTY_GET(size_type,Count,GetCount);
            
            // ---------------------- ACCESSORS ------------------------			
         public:
            /// <summary>Get start iterator</summary>
            VarIterator begin()           { return VarIterator(*this, base::begin()); }
            ConstIterator begin() const   { return ConstIterator(*this, base::cbegin()); }
            
            /// <summary>Get end iterator</summary>
            VarIterator end()             { return VarIterator(*this, base::end()); }
            ConstIterator end() const     { return ConstIterator(*this, base::cend()); }

            /// <summary>Query presence of an argument or variable</summary>
            /// <param name="name">name without $ prefix</param>
            bool Contains(const wstring& name) const
            { 
               return find(name) != base::end(); 
            }
            
            /// <summary>Get arguments only</summary>
            /// <returns></returns>
            VariableArray GetArguments() const
            {
               // Copy arguments
               VariableArray args;
               copy_if(begin(), end(), back_inserter(args), [](const ScriptVariable& v) {return v.Type == VariableType::Argument;} );
               return args;
            }

            /// <summary>Get number of variables and arguments</summary>
            size_type  GetCount() const 
            { 
               return base::size(); 
            }
            
            /// <summary>Get arguments and variables</summary>
            /// <returns></returns>
            VariableArray GetAll() const
            {
               // Copy all
               VariableArray vars;
               copy(begin(), end(), back_inserter(vars));
               return vars;
            }

            /// <summary>Get argument/variable by ID</summary>
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

            /// <summary>Get argument/variable by ID</summary>
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

            /// <summary>Get argument/variable by name</summary>
            /// <param name="name">name without $ prefix</param>
            /// <returns></returns>
            /// <exception cref="Logic::VariableNotFoundException">Not found</exception>
            const ScriptVariable& operator[](const wstring& name) const
            {
               const_iterator v;

               // Lookup variable by name
               if ((v=find(name)) != base::end())
                  return v->second;

               // Not found:
               throw VariableNotFoundException(HERE, name);
            }

         private:
            /// <summary>Gets the next free variable identifier.</summary>
            /// <returns>0-based variable ID</returns>
            UINT  GetNextID() const
            {
               return size();
            }

            // ----------------------- MUTATORS ------------------------
         public:
            /// <summary>Adds a variable using the next available ID. If name already exists no changes are made</summary>
            /// <param name="name">variable name (case sensitive)</param>
            void  Add(const wstring& name)
            {
               if (!Contains(name))
                  insert( value_type(name, ScriptVariable(name, GetNextID())) );
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

         /// <summary></summary>
         class ScriptCallCollection : private map<wstring, ScriptFile>
         {
            // ---------------------- ACCESSORS ------------------------
         public:
            /// <summary>Checks for presence of script (without extension, case sensitive)</summary>
            /// <param name="name">script name.</param>
            /// <returns></returns>
            bool  Contains(const wstring& name) const
            {
               return find(name) != end();
            }

            // ----------------------- MUTATORS ------------------------
         public:
            /// <summary>Adds a script by name (without extension, case sensitive).</summary>
            /// <param name="name">script name.</param>
            /// <param name="f">script properties</param>
            void  Add(const wstring& name, ScriptFile& f)
            {
               insert(value_type(name, f));
            }

            /// <summary>Clears collection</summary>
            void  Clear()
            {
               clear();
            }

            /// <summary>Finds a script by name (without extension, case sensitive).</summary>
            /// <param name="name">script name</param>
            /// <returns></returns>
            /// <exception cref="Logic::InvalidOperationException">Script not present</exception>
            const ScriptFile&  Find(const wstring& name) const
            {
               const_iterator pos;
               // Lookup and return
               if ((pos = find(name)) != end())
                  return pos->second;

               // Not found
               throw InvalidOperationException(HERE, GuiString(L"No properties loaded for external script '%s'", name.c_str()));
            }

            /// <summary>Finds a name of script argument by index.</summary>
            /// <param name="script">script name (without extension, case sensitive)</param>
            /// <param name="index">Zero based index</param>
            /// <returns>Argument name if found, otherwise placeholder</returns>
            wstring  FindArgumentName(const wstring& script, UINT index) const
            {
               // Lookup script + Validate index
               if (Contains(script) && index < Find(script).Variables.GetCount())
                  return Find(script).Variables[index].Name;
               
               // Missing/Invalid: 
               return GuiString(L"arg%d", index+1);
            }

            /// <summary>Finds a type of script argument by name</summary>
            /// <param name="script">script name (without extension, case sensitive)</param>
            /// <param name="arg">Argument name</param>
            /// <returns>Argument type if found, otherwise PS_VALUE  (Which is immune from static type checking)</returns>
            ParameterType  FindArgumentType(const wstring& script, const wstring& arg) const
            {
               // Lookup script + Validate index
               if (Contains(script))
                  return Find(script).Variables[arg].ValueType;
               
               // Missing/Invalid: 
               return ParameterType::VALUE;
            }
         };

         /// <summary></summary>
         class CommandCollection
         {
            // ------------------------ TYPES --------------------------
         private:

            // --------------------- CONSTRUCTION ----------------------

         public:
            CommandCollection()
            {}
            virtual ~CommandCollection()
            {}

            DEFAULT_COPY(CommandCollection);	// Default copy semantics
            DEFAULT_MOVE(CommandCollection);	// Default move semantics

            // ------------------------ STATIC -------------------------

            // --------------------- PROPERTIES ------------------------

            // ---------------------- ACCESSORS ------------------------			

            // ----------------------- MUTATORS ------------------------
         public:
            /// <summary>Adds a command to the input list.</summary>
            /// <param name="cmd">The command.</param>
            void  AddInput(ScriptCommand& cmd)
            {
               Input.push_back(cmd);
            }

            /// <summary>Adds a command to the appropriate output list</summary>
            /// <param name="cmd">The command.</param>
            void  AddOutput(ScriptCommand& cmd)
            {
               // Standard: Append
               if (cmd.Is(CommandType::Standard) && !cmd.Commented)
                  StdOutput.push_back(cmd);
               else
               {  
                  // Auxiliary: Set reference index 
                  cmd.RefIndex = StdOutput.size();
                  
                  // Append Aux
                  AuxOutput.push_back(cmd);
               }
            }

            /// <summary>Clears all commands</summary>
            void  Clear()
            {
               Input.clear();
               StdOutput.clear();
               AuxOutput.clear();
            }

            // -------------------- REPRESENTATION ---------------------

         public:
            CommandList  Input,
                         StdOutput,
                         AuxOutput;
         };

         // --------------------- CONSTRUCTION ----------------------
      public:
         ScriptFile();
         virtual ~ScriptFile();

         // ------------------------ STATIC -------------------------

         // --------------------- PROPERTIES ------------------------
			
         PROPERTY_GET(wstring,CommandName,GetCommandName);

		   // ---------------------- ACCESSORS ------------------------
			
         wstring GetCommandName() const;

		   // ----------------------- MUTATORS ------------------------

         void    Clear();
         bool    FindNext(SearchOperation& src);
         wstring FindScope(UINT line);

		   // -------------------- REPRESENTATION ---------------------

         // Properties
         wstring        Name,
                        Description;
         UINT           Version;
         GameVersion    Game;
         bool           LiveData;
         ParameterValue CommandID;

         // Collections
         CommandCollection    Commands;
         LabelCollection      Labels;
         VariableCollection   Variables;
         ScriptCallCollection ScriptCalls;
      private:
      };

   }
}

using namespace Logic::Scripts;
