#pragma once

#include "Common.h"
#include "CommandSyntax.h"
#include "CommandLexer.h"
#include "ScriptCommand.h"
#include "ScriptParameter.h"
#include "ScriptToken.h"
#include <algorithm>


namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         class CommandNode;

         /// <summary>Script error</summary>
         class ErrorToken : public TokenBase
         {
         public:
            /// <summary>Create error for token</summary>
            ErrorToken(const GuiString& msg, UINT line, const ScriptToken& tok) 
               : TokenBase(tok), Message(msg), Text(tok.Text), Line(line)
            {}
            /// <summary>Create error for a character range</summary>
            ErrorToken(const GuiString& msg, UINT line, const GuiString& txt, const CHARRANGE& r) 
               : TokenBase(r.cpMin, r.cpMax), Message(msg), Text(txt), Line(line)
            {}
            /// <summary>Create error for a character range</summary>
            ErrorToken(const GuiString& msg, UINT line, const GuiString& txt, UINT start, UINT end) 
               : TokenBase(start,end), Message(msg), Text(txt), Line(line)
            {}

            const GuiString Message,   // Error message
                            Text;      // Erroneous token/line text
            const UINT      Line;      // 1-base line number
         };

         /// <summary>Vector of error tokens</summary>
         class ErrorArray : public vector<ErrorToken> 
         {
         public:
            /// <summary>Create empty array</summary>
            ErrorArray()
            {}
            /// <summary>Create array from single token</summary>
            ErrorArray(const ErrorToken& e) {
               push_back(e);
            }

            /// <summary>Add an error to the array</summary>
            ErrorArray& operator+=(const ErrorToken& t)
            {
               push_back(t);
               return *this;
            }
         };

         /// <summary>Shared pointer to a parse tree node</summary>
         class CommandNodePtr : public shared_ptr<CommandNode> 
         {
         public:
            CommandNodePtr() : shared_ptr<CommandNode>(nullptr)
            {}
            CommandNodePtr(CommandNode* node) : shared_ptr<CommandNode>(node)
            {}
         };

         /// <summary>Types of symbol available</summary>
         enum class SymbolType { Variable, Label };

         /// <summary>Represents a symbol used during refactoring process</summary>
         class Symbol
         {
         public:
            /// <summary>Create symbol match.</summary>
            /// <param name="name">name without operators.</param>
            /// <param name="t">type.</param>
            /// <param name="line">1-based line number</param>
            /// <param name="txt">line text</param>
            /// <param name="comment">Whether line is commented</param>
            Symbol(const ScriptToken& tok, SymbolType t, UINT line, const wstring& txt, bool comment) 
               : Token(tok), Type(t), LineNumber(line), LineText(txt), Commented(comment)
            {}

         public:
            const SymbolType  Type;          // Type
            const ScriptToken Token;         // Name
            const UINT        LineNumber;    // 1-based line number
            const GuiString   LineText;      // Line text
            const bool        Commented;     // Whether commented
         };

         /// <summary></summary>
         typedef list<Symbol>  SymbolList;
            
         /// <summary>Represents a script command and its descendants, if any</summary>
         class CommandNode 
         {
            // ------------------------ TYPES --------------------------
         public:
            typedef vector<CommandNodePtr>     NodeList;
            typedef NodeList::const_iterator   NodeIterator;

         private:
            typedef function<bool (const CommandNodePtr&)>  NodeDelegate;

            /// <summary>Distinguishes tree state when printed to the console</summary>
            enum class InputState { Raw, Verified, Compiled };

            // --------------------- CONSTRUCTION ----------------------
         public:
            CommandNode();
            CommandNode(CommandNode* parent, const CommandNode* target);
            CommandNode(Conditional cnd, CommandSyntaxRef syntax, ParameterArray& params, const CommandLexer& lex, UINT line, bool commented);
            CommandNode(Conditional cnd, CommandSyntaxRef syntax, ParameterArray& infix, ParameterArray& postfix, const CommandLexer& lex, UINT line, bool commented);
            ~CommandNode();

            // ------------------------ STATIC -------------------------
         private:
            static NodeDelegate  isConditionalAlternate,
                                 isConditionalEnd,
                                 isExecutableCommand,
                                 isStandardCommand,
                                 isSkipIfCompatible;
            
            static const wchar*  GetString(InputState s);

#ifdef VALIDATION
            /// <summary>An invisible node that functions as a jump target with address 'script_length+1'</summary>
            static CommandNode  EndOfScript;
#endif
            // --------------------- PROPERTIES ------------------------
         public:
            PROPERTY_GET(BranchLogic,Logic,GetBranchLogic);
         private:
            PROPERTY_GET(GuiString,DebugText,GetDebugText);

            // ---------------------- ACCESSORS ------------------------		
         public:
            void          FindAll(const wstring& name, SymbolType type, SymbolList& results) const;
            BranchLogic   GetBranchLogic() const;
            bool          Is(UINT ID) const;
            bool          Is(CommandType t) const;
            void          Print(int depth = 0) const;
               
         private:
            CommandNode*  FindAncestor(BranchLogic l) const;
            NodeIterator  FindChild(const CommandNode* child) const;
            CommandNode*  FindConditionalAlternate() const;
            CommandNode*  FindConditionalEnd() const;
            CommandNode*  FindLabel(const wstring& name) const;
            CommandNode*  FindNextCommand() const;
            CommandNode*  FindNextSibling() const;
            CommandNode*  FindPrevSibling() const;
            CommandNode*  FindRoot() const;
            CommandNode*  FindSibling(NodeDelegate d, const wchar* help) const;
            GuiString     GetDebugText() const;
            CommandNode*  GetLastExecutableChild() const;
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
            
         private:
            void  FinalizeLinkage(ErrorArray& errors);
            void  GenerateCommands(ScriptFile& script, ErrorArray& errors);
            void  IdentifyVariables(ScriptFile& script, ErrorArray& errors);
            void  IndexCommands(UINT& next);
            void  InsertJump(NodeIterator pos, const CommandNode* target);
            void  LinkCommands(ErrorArray& errors);
            void  RevertCommandComment();
            void  VerifyCommand(const ScriptFile& script, ErrorArray& errors);
            
            // -------------------- REPRESENTATION ---------------------
         public:
            CommandNode*  Parent;             // Parent node
            NodeList      Children;           // Child commands

            ParameterArray     Parameters,    // script parameters in display order
                               Postfix;       // expression parameters in postfix order
            CommandSyntaxRef   Syntax;        // command syntax
            const CommandNode* JumpTarget;    // Destination of unconditional-jmp or jump-if-false
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
