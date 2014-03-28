#pragma once
#include "CommandSyntax.h"
#include "CommandLexer.h"
#include "ScriptCommand.h"
#include "ScriptParameter.h"
#include "ScriptToken.h"
#include "ErrorToken.h"
#include "Symbol.h"
#include <algorithm>

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         class CommandTree;
         class CommandNodeList;

         
         /// <summary>Shared pointer to a parse tree node</summary>
         class LogicExport CommandNodePtr : public shared_ptr<CommandTree> 
         {
            // --------------------- CONSTRUCTION ----------------------
         public:
            CommandNodePtr() : shared_ptr<CommandTree>(nullptr)
            {}
            CommandNodePtr(CommandTree* node) : shared_ptr<CommandTree>(node)
            {}

            // ---------------------- ACCESSORS ------------------------	

            // ----------------------- MUTATORS ------------------------

            // -------------------- REPRESENTATION ---------------------
         };

            
         /// <summary>Represents a script command and its descendants, if any</summary>
         class LogicExport CommandTree 
         {
            // ------------------------ TYPES --------------------------
         protected:
            /// <summary>List of parse tree node pointers</summary>
            typedef list<CommandNodePtr>   CommandNodeList;

            /// <summary>CommandTree array iterator</summary>
            typedef CommandNodeList::const_iterator   NodeIterator;

            /// <summary>CommandTree predicate</summary>
            typedef function<bool (const CommandNodePtr&)>  NodeDelegate;

            /// <summary>Distinguishes tree state when printed to the console</summary>
            enum class InputState { Raw, Verified, Compiled };

            // --------------------- CONSTRUCTION ----------------------
         public:
            CommandTree();
            CommandTree(CommandTree* parent, const CommandTree* target);
            CommandTree(Conditional cnd, CommandSyntaxRef syntax, ParameterArray& params, const CommandLexer& lex, UINT line, bool commented);
            CommandTree(Conditional cnd, CommandSyntaxRef syntax, ParameterArray& infix, ParameterArray& postfix, const CommandLexer& lex, UINT line, bool commented);
            ~CommandTree();

            // ------------------------ STATIC -------------------------
         protected:
            static NodeDelegate  isConditionalAlternate;
            static NodeDelegate  isConditionalEnd;
            static NodeDelegate  isExecutableCommand;
            static NodeDelegate  isStandardCommand;
            static NodeDelegate  isSkipIfCompatible;
            
            static const wchar*  GetString(InputState s);

#ifdef VALIDATION
            /// <summary>An invisible node that functions as a jump target with address 'script_length+1'</summary>
            static CommandTree  EndOfScript;
#endif
            // --------------------- PROPERTIES ------------------------
         public:
            PROPERTY_GET(BranchLogic,Logic,GetBranchLogic);
            PROPERTY_GET(GuiString,LineCode,GetLineCode);

            // ---------------------- ACCESSORS ------------------------		
         public:
            void          FindAll(const wstring& name, SymbolType type, SymbolList& results) const;
            BranchLogic   GetBranchLogic() const;
            GuiString     GetLineCode() const;
            bool          Is(UINT ID) const;
            bool          Is(CommandType t) const;
            void          Print(int depth = 0) const;
            void          ToList(CommandNodeList& l) const;
               
         protected:
            CommandTree*  FindAncestor(BranchLogic l) const;
            NodeIterator  FindChild(const CommandTree* child) const;
            CommandTree*  FindConditionalAlternate() const;
            CommandTree*  FindConditionalEnd() const;
            CommandTree*  FindLabel(const wstring& name) const;
            CommandTree*  FindNextCommand() const;
            CommandTree*  FindNextSibling() const;
            CommandTree*  FindPrevSibling() const;
            CommandTree*  FindRoot() const;
            CommandTree*  FindSibling(NodeDelegate d, const wchar* help) const;
            CommandTree*  GetLastExecutableChild() const;
            wstring       GetScriptCallName() const;
            bool          HasExecutableChild() const;
            bool          IsRoot() const;
            ErrorToken    MakeError(const GuiString& msg) const;
            ErrorToken    MakeError(const GuiString& msg, const ScriptToken& tok) const;
            void          VerifyLogic(ErrorArray& errors) const;
            void          VerifyParameter(const ScriptParameter& p, UINT index, const ScriptFile& script, ErrorArray& errors) const;
            void          VerifyTermination(ErrorArray& errors) const;
               
            // ----------------------- MUTATORS ------------------------
         public:
            CommandNodePtr Add(CommandNodePtr node);
            void           Compile(ScriptFile& script, ErrorArray& errors);
            void           Verify(ScriptFile& script, ErrorArray& errors);
            
         protected:
            void  ExpandMacros(ScriptFile& script, ErrorArray& errors);
            void  FinalizeLinkage(ErrorArray& errors);
            void  GenerateCommands(ScriptFile& script, ErrorArray& errors);
            void  IdentifyConstants(ScriptFile& script, ErrorArray& errors);
            void  IdentifyVariables(ScriptFile& script, ErrorArray& errors);
            void  IndexCommands(UINT& next);
            void  InsertJump(NodeIterator pos, const CommandTree* target);
            void  LinkCommands(ErrorArray& errors);
            void  RevertCommandComment(CommandTree* child);
            void  ReplaceChild(CommandTree* oldChild, CommandTree* newChild);
            void  VerifyCommand(const ScriptFile& script, ErrorArray& errors);
            
            // -------------------- REPRESENTATION ---------------------
         public:
            CommandTree*       Parent;        // Parent node
            CommandNodeList    Children;      // Child commands

            ParameterArray     Parameters,    // script parameters in display order
                               Postfix;       // expression parameters in postfix order
            CommandSyntaxRef   Syntax;        // command syntax
            const CommandTree* JumpTarget;    // Destination of unconditional-jmp or jump-if-false
            UINT               Index;         // 0-based standard codearray index
            bool               CmdComment;    // Whether a command comment  [false for ordinary comments]

            Conditional     Condition;        // Conditional
            const UINT      LineNumber;       // 1-based line number
            const CHARRANGE Extent;           // Start/end character offsets
            // Debug
            GuiString       LineText;         // Debug: line text
            InputState      State;            // Debug: processing state
         };
      }
   }
}

using namespace Logic::Scripts::Compiler;
