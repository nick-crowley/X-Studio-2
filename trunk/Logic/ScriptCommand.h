#pragma once

#include "CommandSyntax.h"
#include "ScriptParameter.h"
#include "ScriptToken.h"
#include "RtfWriter.h"

namespace Logic
{
   namespace Scripts
   {
      class ScriptFile;

      /// <summary>Branching logic implemented by a script command</summary>
      enum BranchLogic   { None, NOP, If, While, SkipIf, Else, ElseIf, End, Break, Continue };

      /// <summary>Get branch logic name</summary>
      const WCHAR* GetString(BranchLogic l);

      /// <summary>String ID referenced by a script command</summary>
      class LogicExport StringID
      {
      public:
         /// <summary>Create string reference</summary>
         /// <param name="page">page id parameter.</param>
         /// <param name="id">string id parameter.</param>
         StringID(ScriptParameter page, ScriptParameter id) : Page(page), String(id)
         {}

      public:
         ScriptParameter Page, 
                         String;
      };

      /// <summary>Script Parameter lamda predicates</summary>
      typedef function<bool (const ScriptParameter&)>  ParameterPredicate;

      /// <summary>An MSCI script command</summary>
      class LogicExport ScriptCommand
      {
         // --------------------- CONSTRUCTION ----------------------
      public:
         ScriptCommand();
      public:
         ScriptCommand(const wstring& text, CommandSyntaxRef syntax, ParameterArray& params, bool cmdComment);
         ScriptCommand(const wstring& text, CommandSyntaxRef syntax, ParameterArray& infix, ParameterArray& postfix, bool cmdComment);
         ScriptCommand(CommandSyntaxRef syntax, ParameterArray& params, bool comment);
         ScriptCommand(CommandSyntaxRef syntax, UINT  ref, ParameterArray& params, bool comment);
         virtual ~ScriptCommand();

         // ------------------------ STATIC -------------------------
      public:
         static ScriptCommand  Unrecognised;

         static const ParameterPredicate IsScriptNameParam;
         static const ParameterPredicate IsStringIDParam;
         static const ParameterPredicate IsPageIDParam;

         // --------------------- PROPERTIES ------------------------
			
         PROPERTY_GET(BranchLogic,Logic,GetLogic);

		   // ---------------------- ACCESSORS ------------------------			
      public:
         BranchLogic  GetLogic() const;
         wstring      GetLabelName() const;
         UINT         GetJumpDestination() const;
         wstring      GetScriptCallName() const;
         StringID     GetStringReference() const;

         bool  Is(UINT ID) const;
         bool  Is(CommandType t) const;

         operator bool() const {
            throw NotImplementedException(HERE, L"StaticLib compiler fix");
         }
         const ScriptCommand& operator==(const ScriptCommand& r) const {
            throw NotImplementedException(HERE, L"StaticLib compiler fix");
         }

		   // ----------------------- MUTATORS ------------------------
      public:
         void  SetLabelName(const wstring& name);
         void  Translate(ScriptFile& f);

		   // -------------------- REPRESENTATION ---------------------
      public:
         CommandSyntaxRef Syntax;         // Syntax
         bool             Commented;      // Whether command is commented  (false for comments)
         ParameterArray   Parameters;     // Parameters in physical syntax order
         GuiString        Text;           // Translated/Generated command Text
         UINT             RefIndex;       // [Auxiliary] Index of associated standard command
      };

      /// <summary>List of script commands</summary>
      class LogicExport CommandList : public list<ScriptCommand>
      {
      public:
         /// <summary>Find command by index.</summary>
         /// <param name="index">The index.</param>
         /// <returns></returns>
         /// <exception cref="Logic::IndexOutOfRangeException">Invalid index</exception>
         ScriptCommand& operator[](UINT index)
         {
            UINT i = 0;

            // Linear search
            for (auto pos = begin(); pos != end(); ++pos)
               if (index == i++)
                  return *pos;

            // Invalid index
            throw IndexOutOfRangeException(HERE, index, size());
         }
      };

      /// <summary>Vector of script commands</summary>
      typedef vector<ScriptCommand> CommandArray;
   }
}

using namespace Logic::Scripts;
