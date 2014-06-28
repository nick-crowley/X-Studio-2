#pragma once
#include "CommandSyntax.h"
#include "CommandLexer.h"
#include "CommandNodeList.h"
#include "ScriptCommand.h"
#include "ScriptParameter.h"
#include "ScriptToken.h"
#include "ErrorToken.h"
#include "Symbol.h"
#include "TreeTraversal.h"
#include <algorithm>

namespace Testing
{
   class LogicTests;
}

namespace Logic
{
   namespace Scripts
   {
      namespace Compiler
      {
         class CommandNode;
         
         /// <summary>Shared pointer to a parse tree node</summary>
         class LogicExport CommandNodePtr : public shared_ptr<CommandNode> 
         {
            // --------------------- CONSTRUCTION ----------------------
         public:
            CommandNodePtr() : shared_ptr<CommandNode>(nullptr)
            {}
            CommandNodePtr(CommandNode* node) : shared_ptr<CommandNode>(node)
            {}

            // ---------------------- ACCESSORS ------------------------	

            // ----------------------- MUTATORS ------------------------

            // -------------------- REPRESENTATION ---------------------
         };

            
         /// <summary>Represents a script command and its descendants, if any</summary>
         class LogicExport CommandNode 
         {
            friend class ::Testing::LogicTests;

            // ------------------------ TYPES --------------------------
         protected:
            /// <summary>CommandNode array iterator</summary>
            typedef CommandNodeList::const_iterator   NodeIterator;

            /// <summary>CommandNode predicate</summary>
            typedef function<bool (const CommandNodePtr&)>  NodeDelegate;


         public:
            /// <summary>Base-class for all visitors</summary>
            class Visitor
            {
               // --------------------- CONSTRUCTION ----------------------
            public:
               virtual ~Visitor() {}
               
               // ---------------------- ACCESSORS ------------------------			
               
               // ----------------------- MUTATORS ------------------------
            public:
               virtual void VisitNode(CommandNode* n) = 0;
               virtual void VisitRoot(CommandNode* n) {}
            };

            // --------------------- CONSTRUCTION ----------------------
         public:
            CommandNode();
            CommandNode(Conditional cnd, CommandSyntaxRef syntax, ParameterArray& params, const CommandLexer& lex, UINT line, bool commented);
            CommandNode(Conditional cnd, CommandSyntaxRef syntax, ParameterArray& infix, ParameterArray& postfix, const CommandLexer& lex, UINT line, bool commented);
            virtual ~CommandNode();

         protected:
            CommandNode(const CommandNode& parent, const CommandNode* target);
            CommandNode(const CommandNode& macro, const CommandNode& expanded);

            // ------------------------ STATIC -------------------------
         protected:
            static NodeDelegate  isConditionalAlternate;
            static NodeDelegate  isConditionalEnd;
            static NodeDelegate  isExecutableCommand;
            static NodeDelegate  isStandardCommand;
            static NodeDelegate  isSkipIfCompatible;
            
#ifdef VALIDATION
            /// <summary>An invisible node that functions as a jump target with address 'script_length+1'</summary>
            static CommandNode  EndOfScript;
#endif
            // --------------------- PROPERTIES ------------------------
         public:
            PROPERTY_GET(bool,Empty,IsEmpty);
            PROPERTY_GET(BranchLogic,Logic,GetBranchLogic);
            PROPERTY_GET(GuiString,LineCode,GetLineCode);

            // ---------------------- ACCESSORS ------------------------		
         public:
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
            BranchLogic   GetBranchLogic() const;
            CommandNode*  GetLastExecutableChild() const;
            GuiString     GetLineCode() const;
            wstring       GetScriptCallName() const;
            bool          Is(UINT ID) const;
            bool          Is(CommandType t) const;
            bool          IsEmpty() const;
            bool          HasExecutableChild() const;
            bool          IsRoot() const;
            ErrorToken    MakeError(const GuiString& msg) const;
            ErrorToken    MakeError(const GuiString& msg, const ScriptToken& tok) const;
            
            // ----------------------- MUTATORS ------------------------
         public:
            void           Accept(Visitor& v);
            CommandNodePtr Add(CommandNodePtr node);
            void           InsertJump(NodeIterator pos, const CommandNode* target);
            void           MoveChildren(CommandNode& n);
            void           RevertCommandComment(CommandNode* child);
         
         protected:
            void           ReplaceChild(CommandNode* oldChild, CommandNode* newChild);
            
            // -------------------- REPRESENTATION ---------------------
         public:
            CommandNode*       Parent;        // Parent node
            CommandNodeList    Children;      // Child commands

            ParameterArray     Parameters,    // script parameters in display order
                               Postfix;       // expression parameters in postfix order
            CommandSyntaxRef   Syntax;        // command syntax
            bool               CmdComment;    // Whether a command comment  [false for ordinary comments]
            GuiString          LineText;      // line text

            const UINT         LineNumber;    // 1-based line number
            const CHARRANGE    Extent;        // Start/end character offsets

         protected:
            Conditional        Condition;     // Conditional
            const CommandNode* JumpTarget;    // Destination of unconditional-jmp or jump-if-false
            UINT               Index;         // 0-based standard codearray index
         };
      }
   }
}

using namespace Logic::Scripts::Compiler;
